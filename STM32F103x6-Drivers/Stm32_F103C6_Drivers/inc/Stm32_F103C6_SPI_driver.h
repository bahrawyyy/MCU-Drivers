/*
 * Stm32_F103C6_SPI_driver.h
 *
 *  Created on: Jul 25, 2023
 *      Author: Abdallah
 */

#ifndef INC_STM32_F103C6_SPI_DRIVER_H_
#define INC_STM32_F103C6_SPI_DRIVER_H_

#include "stm32f103c6.h"
#include "Stm32_F103C6_gpio_driver.h"


// -------------------------------------------------------------------------
// ======================= Configuration structure =========================
// -------------------------------------------------------------------------



// Determines which was interrupt source (TXE, RXNE, ERROR)
struct S_IRQ_SRC
{
	uint8_t TXE:1;   	// Tx buffer empty interrupt
	uint8_t RXNE:1;		// Rx buffer not empty interrupt
	uint8_t ERRI:1;		// Error interrupt
	uint8_t Reserved:5;
};


typedef struct
{
	uint16_t 		Device_Mode;			// Specifies SPI operating mode (master or slave)
											// This parameter must be defined based on @ref SPI_Device_Mode_define

	uint16_t		Communication_Mode;		// Specifies SPI directional mode
											// This parameter must be defined based on @ref SPI_Communication_Mode_define

	uint16_t 		Frame_Format;			// Specifies LSB or MSB first
											// This parameter must be defined based on @ref SPI_Frame_Format_define

	uint16_t		DataSize;				// Specifies data size (8 or 16)
											// This parameter must be defined based on @ref SPI_Data_Size_define

	uint16_t		CLKPolarity;			// Specifies what the idle case is (0 or 1)
											// This parameter must be defined based on @ref SPI_Clock_Polarity_define

	uint16_t		CLKPhase;				// Specifies whether sampling on the first or the second edge
											// This parameter must be defined based on @ref SPI_Clock_Phase_define

	uint16_t 		NSS;					// Specifies whether using hardware pin (NSS) or software using SSI bit enable
											// This parameter must be defined based on @ref SPI_NSS_define

	uint16_t		Baudrate_Prescalar;		// Specifies the baudrate prescalar which will be used to configure SCK
											// This parameter must be defined based on @ref SPI_Baudrate_Prescalar_define
											// Note: The communication clock is derived from the master and it will be an input to the slave
											// Take care you have to configure RCC to enter the correct clock  APB2 --> SPI1, APB1 --> SPI2

	uint16_t		IRQ_Enable;				// Specifies whether using polling or interrupt mechanism
											// This parameter must be defined based on @ref SPI_IRQ_define

	void(*P_IRQ_CallBack)(struct S_IRQ_SRC irq_src);			// Set the C function which will be called once the IRQ happens


}SPI_Config;



// -------------------------------------------------------------------------
// ======================= Reference Macros ================================
// -------------------------------------------------------------------------



// @ref SPI_Device_Mode_define (MSTR:2 in CR1)
#define SPI_Device_Mode_Slave		(uint32_t)(0)
#define SPI_Device_Mode_Master		(uint32_t)(1<<2)


// @ref SPI_Communication_Mode_define (BIDIMODE:15 & BIDIOE:14 & RXONLY:10) in CR1
#define SPI_Communication_Mode_2LINES					(uint32_t)(0)    // Full duplex
#define SPI_Communication_Mode_2LINES_RXONLY			(uint32_t)(1<<10)
#define SPI_Communication_Mode_1LINE_receive_only		(uint32_t)(1<<15)
#define SPI_Communication_Mode_1LINE_transmit_only		(uint32_t)(1<<15 | 1<<14)


// @ref SPI_Frame_Format_define (LSBFIRST: 7) in CR1
#define SPI_Frame_Format_MSBFIRST						(uint32_t)(0)
#define SPI_Frame_Format_LSBFIRST						(uint32_t)(1<<7)



// @ref SPI_Data_Size_define (DFF: 11) in CR1
#define SPI_Data_Size_8BIT								(uint32_t)(0)
#define SPI_Data_Size_16BIT								(uint32_t)(1<<11)


// @ref SPI_Clock_Polarity_define (CPOL: 1) in CR1
#define SPI_Clock_Polarity_IDLE_LOW						(uint32_t)(0)
#define SPI_Clock_Polarity_IDLE_HIGH					(uint32_t)(1<<1)


// @ref SPI_Clock_Phase_define (CPHA: 0) in CR1
#define SPI_Clock_Phase_FIRST_EDGE						(uint32_t)(0)
#define SPI_Clock_Phase_SECOND_EDGE						(uint32_t)(1<<0)

/**
 *  ****************** NSS **********************
 */

// @ref SPI_NSS_define
// Hardware
#define SPI_NSS_HW_SLAVE								(uint32_t)(0)      // SSM in CR1
#define SPI_NSS_HW_MASTER_Output_Enabled				(1<<2)   // SSOE in CR2
#define SPI_NSS_HW_MASTER_Output_Disabled				~(1<<2)	   // SSOE in CR2


// Software (Master or slave) SSM and SSI in CR1
#define SPI_NSS_InternalSlaveSelect_Reset				(uint32_t)(1<<9)
#define SPI_NSS_InternalSlaveSelect_Set					(uint32_t)(1<<9 | 1<<8)


/**
 *  *********************************************
 */

// @ref SPI_Baudrate_Prescalar_define BR[2:0] 3:5 in CR1
#define SPI_Baudrate_Prescalar_2						(uint32_t)(0)
#define SPI_Baudrate_Prescalar_4						(uint32_t)(0b001U << 3)
#define SPI_Baudrate_Prescalar_8						(uint32_t)(0b010U << 3)
#define SPI_Baudrate_Prescalar_16						(uint32_t)(0b011U << 3)
#define SPI_Baudrate_Prescalar_32						(uint32_t)(0b100U << 3)
#define SPI_Baudrate_Prescalar_64						(uint32_t)(0b101U << 3)
#define SPI_Baudrate_Prescalar_128						(uint32_t)(0b110U << 3)
#define SPI_Baudrate_Prescalar_256						(uint32_t)(0b111U << 3)


// @ref SPI_IRQ_define
#define SPI_IRQ_NONE									(uint32_t)(0)
#define SPI_IRQ_TXEIE									(uint32_t)(1<<7)
#define SPI_IRQ_RXNEIE									(uint32_t)(1<<6)
#define SPI_IRQ_ERRIE									(uint32_t)(1<<5)



enum PollingMechanism
{
	Polling_Disable,
	Polling_Enable
};

// -------------------------------------------------------------------------
// ======================= APIs supported by SPI DRIVER  ================================
// -------------------------------------------------------------------------

void MCAL_SPI_Init(SPI_Typedef* SPIx, SPI_Config* SPI_Cfg);
void MCAL_SPI_DeInit(SPI_Typedef* SPIx, SPI_Config* SPI_Cfg);

void MCAL_SPI_GPIO_Set_Pins(SPI_Typedef* SPIx);

void MCAL_SPI_Send_Data(SPI_Typedef* SPIx, uint16_t *pTxBuffer, enum PollingMechanism PollingEn);
void MCAL_SPI_Receive_Data(SPI_Typedef* SPIx, uint16_t *pRxBuffer, enum PollingMechanism PollingEn);

// will send the data in the buffer and writes the received data in this buffer
void MCAL_SPI_TX_RX(SPI_Typedef* SPIx, uint16_t *pTxBuffer, enum PollingMechanism PollingEn);



#endif /* INC_STM32_F103C6_SPI_DRIVER_H_ */
