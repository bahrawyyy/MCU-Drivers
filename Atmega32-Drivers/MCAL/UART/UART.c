/*
 * UART.c
 *
 *  Created on: Jul 18, 2023
 *      Author: Abdallah
 */


#include "UART.h"
#include "UART_Cfg.h"

// Private declaration
static void Func_TX(void);
static void Receive_Int_Function(void);

// Variables
static u8  *Asynch_Send_Str;
static u8  *Asynch_receive_str;


// Baud rate array
const u16 BaudRateArray[TOTAL_SPEED_MODE][TOTAL_CPU_F][TOTAL_BAUD_RATE]=
{{{103, 51, 25, 16, 12,  8},{207,103, 51, 34, 25, 16},{416,207,103, 68 ,51, 34}},
{{207,103, 51, 34, 25, 16},{416,207,103, 68 ,51, 34},{832,416,207,138,103, 68}}};


// ==============================================
// 		Pointer to functions (used in ISR)
// ==============================================


static void (*UART_RX_Fptr) (void);
static void (*UART_TX_Fptr) (void);

#define NULLPTR   ((void*)0)   // 0 casted to an address

// ==============================================
// 		Initialization
// ==============================================


void UART_Init(void)
{
	u8  UCSRC_var=0;
	u16 UBRR_var=0;

#if (SPEED_MODE==NORMAL_SPEED)
	CLR_BIT(UCSRA,U2X);
#elif (SPEED_MODE==DOUBLE_SPEED)
	SET_BIT(UCSRA,U2X);
#endif

	// Set URSEL to write data to UCSRC register
	SET_BIT(UCSRC_var,URSEL);


#if (SYNCH_MODE==SYNCH)
	SET_BIT(UCSRC_var,UMSEL)
#elif (SYNCH_MODE==ASYNCH)
	CLR_BIT(UCSRC_var,UMSEL);
#endif

#if (PARITY_MODE==NO_PARITY)
	CLR_BIT(UCSRC_var,UPM0);
	CLR_BIT(UCSRC_var,UPM1);
#elif (PARITY_MODE==EVEN_PARITY)
	CLR_BIT(UCSRC_var,UPM0);
	SET_BIT(UCSRC_var,UPM1);
#elif (PARITY_MODE==ODD_PARITY)
	SET_BIT(UCSRC_var,UPM0);
	SET_BIT(UCSRC_var,UPM1);
#endif

#if (N_DATA_BITS==_5_DATA_BITS)
	CLR_BIT(UCSRC_var,UCSZ0);
	CLR_BIT(UCSRC_var,UCSZ1);
	CLR_BIT(UCSRC_var,UCSZ2);
#elif (N_DATA_BITS==_6_DATA_BITS)
	SET_BIT(UCSRC_var,UCSZ0);
	CLR_BIT(UCSRC_var,UCSZ1);
	CLR_BIT(UCSRC_var,UCSZ2);
#elif (N_DATA_BITS==_7_DATA_BITS)
	CLR_BIT(UCSRC_var,UCSZ0);
	SET_BIT(UCSRC_var,UCSZ1);
	CLR_BIT(UCSRB,UCSZ2);
#elif (N_DATA_BITS==_8_DATA_BITS)
	SET_BIT(UCSRC_var,UCSZ0);
	SET_BIT(UCSRC_var,UCSZ1);
	CLR_BIT(UCSRB,UCSZ2);
#elif (N_DATA_BITS==_9_DATA_BITS)
	SET_BIT(UCSRC_var,UCSZ0);
	SET_BIT(UCSRC_var,UCSZ1);
	SET_BIT(UCSRB,UCSZ2);
#endif

#if (N_STOP_BITS==ONE_STOP_BIT)
	CLR_BIT(UCSRC_var,USBS);
#elif (N_STOP_BITS==TWO_STOP_BIT)
	SET_BIT(UCSRC_var,USBS);
#endif


	// Set UCSRC value
	UCSRC=UCSRC_var;


	// Baud rate
	UBRR_var = BaudRateArray[SPEED_MODE][CPU_F][BUAD_RATE];
	UBRRH = (u8)(UBRR_var>>8);
	UBRRL = (u8)UBRR_var;


	// Enabling UART
	//enable UART  receiver
	SET_BIT(UCSRB,RXEN);
	//enable UART  transmitter
	SET_BIT(UCSRB,TXEN);

}


// ========================================================
// 			Send and receive Functions without checking
// ========================================================



void UART_SendByteNoBlock(u8 data)
{
	UDR = data;
}

u8 UART_ReceiveByteNoBlock(void)
{
    return UDR;
}


// ========================================================
// 			Busy Wait Functions
// ========================================================

void UART_Send_Char(u8 data)
{
	/* UDRE flag is set when the buffer is empty and ready for transmitting
		a new byte so wait until this flag is set to one and it will cleared by
		hardware when u write new data to buffer */
	// Busy wait
	while(!GET_BIT(UCSRA,UDRE));
	UDR = data;
}


u8 UART_Receive_Char(void)
{
	/* RXC flag is set when the UART receives data so wait until this flag
		is set to one and it will be cleared by hardware when u read data  */
	// The CPU is stucking here
	while(!GET_BIT(UCSRA,RXC));
	return UDR;
}


void UART_Send_String(char* string)
{
	u8 i=0;
	while(string[i] != '\0'){
		UART_Send_Char(string[i]);
		i++;
	}
	// Send (Enter) as a terminating char
	UART_Send_Char(DefaultStop);
}


void UART_Receive_String(char *Buffer)
{
    int i = 0;

    Buffer[i] = UART_Receive_Char();
    while (Buffer[i] != DefaultStop) {
        i++;
        Buffer[i] = UART_Receive_Char();

    }

    Buffer[i] = '\0'; // Null-terminate the string
}


// ==============================================
// 		Interrupts functions
// ==============================================


// Receive complete
void UART_RX_InterruptEnable(void)
{
	SET_BIT(UCSRB,RXCIE);
}

// Transmission complete
void UART_TX_InterruptEnable(void)
{
	SET_BIT(UCSRB,TXCIE);
}


// Receive complete
void UART_RX_InterruptDisable(void)
{
	CLR_BIT(UCSRB,RXCIE);
}

// Transmission complete
void UART_TX_InterruptDisable(void)
{
	CLR_BIT(UCSRB,TXCIE);
}



// ==============================================
// 		Call Back functions
// ==============================================


void UART_RX_SetCallBack(void (*LocalFptr)(void))
{
	UART_RX_Fptr = LocalFptr;
}

void UART_TX_SetCallBack(void (*LocalFptr)(void))
{
	UART_TX_Fptr = LocalFptr;
}

// ==============================================
// 		ISR
// ==============================================


ISR(USART_TXC_vect)
{
	if(UART_TX_Fptr!=NULLPTR)
	{
		UART_TX_Fptr();
	}
}


ISR(USART_RXC_vect)
{
	if(UART_RX_Fptr!=NULLPTR)
	{
		UART_RX_Fptr();
	}
}


u8 UART_Receive_NoBlock(u8 *pdata)
{
	u8 status = 0;
	// Check if any data is received and store it in the passed buffer
	if(GET_BIT(UCSRA,RXC))
	{
		*pdata = UDR;
		status=1;
	}else{
		status = 0;
	}
	return status;
}




// ==============================================
// 		Asynchronous functions
// ==============================================


void UART_SendString_ASYNCH(u8 *str)
{
	Asynch_Send_Str = str;
	UART_Send_Char(str[0]);

	UART_TX_SetCallBack(Func_TX);
	UART_TX_InterruptEnable();
}


static void Func_TX(void)
{
	static u8 i=1;
	if(Asynch_Send_Str[i] != '\0')
	{
		UART_SendByteNoBlock(Asynch_Send_Str[i]);
		i++;
	}else{
		i=1;
		UART_TX_InterruptDisable();
	}
}

// -----------------------------------------------

void UART_ReceiveString_ASYNCH(u8* str)
{
	Asynch_receive_str = str;
	UART_RX_InterruptEnable();
	UART_RX_SetCallBack(Receive_Int_Function);
}



static void Receive_Int_Function(void)
{
	static u8 i=0;
	Asynch_receive_str[i] = UART_ReceiveByteNoBlock();
	i++;
}



