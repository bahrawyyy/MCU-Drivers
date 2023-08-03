/*
 * Stm32_F103C6_I2C_driver.c
 *
 *  Created on: Jul 30, 2023
 *      Author: Abdallah
 */


#include "Stm32_F103C6_I2C_driver.h"



/** ====================================================================== /
 * 						Generic Variables
====================================================================== /
 */


// A copy to be used among the whole file
I2C_Config_t Global_I2C_Config[2] = {NULL,NULL};

#define I2C1_INDEX			0
#define I2C2_INDEX			1




/** ====================================================================== /
 * 						APIs
====================================================================== /
 */



// -------------------------------------------------------------------------
// ======================= Initialization APIs  ================================
// -------------------------------------------------------------------------



/**================================================================
 * @Fn					- MCAL_I2C_Init
 * @brief 				- Initializes I2C according to the specified parameters
 * @param [in] 			- I2Cx: where x can be (1 or 2) depending on the device used
 * @param [in] 			- I2C_Cfg: All the I2C configurations
 * @retval 				- None
 * Note					- Supports Master in polling mechanism & slave interrupt Mechanism
 *
 */

void MCAL_I2C_Init(I2C_Typedef* I2Cx, I2C_Config_t *I2C_Cfg)
{

	uint16_t tmpreg = 0, freqrange=0;
	uint32_t pclk1 = 8000000;
	uint16_t result = 0;


	// Enable RCC clock
	if(I2Cx == I2C1)
	{
		Global_I2C_Config[I2C1_INDEX] = *I2C_Cfg;
		RCC_I2C1_CLK_EN();
	}else
	{
		Global_I2C_Config[I2C2_INDEX] = *I2C_Cfg;
		RCC_I2C2_CLK_EN();
	}


	// Mode
	if(I2C_Cfg->I2C_Mode == I2C_MODE_I2C)
	{
		/**************** 1- Initialize timing ************/

		// 1- in I2C_CR2 Bits 5:0 FREQ[5:0]: Peripheral clock frequency from APB1
		// Get the I2Cx CR2 Value
		tmpreg = I2Cx->CR2;
		// Clear Frequency FREQ[5:0] bits
		tmpreg &= ~(I2C_CR2_FREQ_Msk);
		// Get Pclk1 frequency value
		pclk1 = MCAL_RCC_GetPCLK1Freq();
		// Set the frequency bits depending on pclk1 value
		freqrange = (uint16_t)(pclk1 / 1000000);

		tmpreg |= freqrange;
		I2Cx->CR2 = tmpreg;

		// 2- Configure the clock control register (Thigh = CCR * TPCLK1)

		// Disable the selected I2C Peripheral to configure time
		I2Cx->CR1 &= ~(I2C_CR1_PE);

		tmpreg = 0;

		// Configure speed in standard mode
		if(I2C_Cfg->I2C_Clock_Speed == I2C_SCLK_SM_50K || I2C_Cfg->I2C_Clock_Speed == I2C_SCLK_SM_100K)
		{
			// By default the bit responsible for SM is 0, as we already made tmpreg = 0

			// Standard Mode speed Calculations

			// Tclki2c / 2 = CCR * Tpclk1
			// CCR = Tclki2c / (2 * Tpclk1)
			// CCR = Fpclk1 / (2 * Fclki2c)

			result = (uint16_t)(pclk1/(I2C_Cfg->I2C_Clock_Speed << 1));
			tmpreg |= result;

			// Write to CCR
			I2Cx->CCR = tmpreg;


			// 3- Configure the rise time register (I2C_TRISE)

			//	If, in the I2C_CR2 register, the value of FREQ[5:0] bits is equal to 0x08 and TPCLK1 = 125 ns
			//	therefore the TRISE[5:0] bits must be programmed with 09h.
			//	(1000 ns / 125 ns = 8 + 1)

			I2Cx->TRISE = freqrange + 1;

		}else
		{
			// (Fast Mode) FM not supported yet
		}


		// CR1 Configuration

		tmpreg = 0;

		tmpreg = I2Cx->CR1;

		tmpreg |= (uint16_t)(I2C_Cfg->ACK_Control | I2C_Cfg->General_Call_Address_Detection |I2C_Cfg->I2C_Mode| I2C_Cfg->Stretch_Mode);

		I2Cx->CR1 = tmpreg;


		// If you were slave
		// ********************* I2Cx OAR1 & I2Cx OAR2 Configuration

		tmpreg = 0;
		if(I2C_Cfg->I2C_Slave_Address.Dual_Address_Enable == 1)
		{
			tmpreg = I2C_OAR2_ENDUAL;
			tmpreg |= I2C_Cfg->I2C_Slave_Address.Secondary_Slave_Address << I2C_OAR2_ADD2_Pos;
			I2Cx->OAR2 = tmpreg;
		}

		tmpreg = 0;
		tmpreg |= I2C_Cfg->I2C_Slave_Address.Primary_Slave_Address << 1;

		tmpreg |= I2C_Cfg->I2C_Slave_Address.Addressing_Bits;

		I2Cx->OAR1 = tmpreg;






	}else
	{
		// SMBUS Not supported yet
	}



	// Interrupt mode (Slave mode) // Check callBack != NULL, then you're a slave
	if(I2C_Cfg->P_Slave_Event_CallBack != NULL)
	{
		// Enable all IRQs
		I2Cx->CR2 |= (I2C_CR2_ITERREN);
		I2Cx->CR2 |= (I2C_CR2_ITEVTEN);
		I2Cx->CR2 |= (I2C_CR2_ITBUFEN);

		if(I2Cx == I2C1)
		{
			NVIC_IRQ31_I2C1_EV_Enable;
			NVIC_IRQ32_I2C1_ER_Enable;

		}else if(I2Cx == I2C2)
		{
			NVIC_IRQ33_I2C2_EV_Enable;
			NVIC_IRQ34_I2C2_ER_Enable;
		}

		I2Cx->SR1 = 0;
		I2Cx->SR2 = 0;


	}




	// Enable the selected I2C peripheral
	I2Cx->CR1 = I2C_CR1_PE;
}



/**================================================================
 * @Fn					- MCAL_I2C_DeInit
 * @brief 				- DeInitializes I2C
 * @param [in] 			- I2Cx: where x can be (1 or 2) depending on the device used
 * @param [in] 			- I2C_Cfg: All the I2C configurations
 * @retval 				- None
 * Note					- Supports Master in polling mechanism & slave interrupt Mechanism
 *
 */


void MCAL_I2C_DeInit(I2C_Typedef* I2Cx, I2C_Config_t *I2C_Cfg)
{
	if(I2Cx == I2C1)
	{
		NVIC_IRQ31_I2C1_EV_Disable;
		NVIC_IRQ32_I2C1_ER_Disable;
		RCC_I2C1_Reset();

	}else if(I2Cx == I2C2)
	{
		NVIC_IRQ33_I2C2_EV_Disable;
		NVIC_IRQ34_I2C2_ER_Disable;
		RCC_I2C2_Reset();
	}
}



/**================================================================
 * @Fn					- MCAL_I2C_GPIO_Set_Pins
 * @brief 				- Sets Pins of I2C accoring to the recommended settings in data sheet
 * @param [in] 			- I2Cx: where x can be (1 or 2) depending on the device used
 * @retval 				- None
 * Note					- None
 *
 */


void MCAL_I2C_GPIO_Set_Pins(I2C_Typedef* I2Cx)
{
	GPIO_PinConfig_t PinCfg;

	if(I2Cx == I2C1)
	{
		// PB6: I2C1_SCL --> Open drain
		// PB7: I2C1_SDA --> Open drain

		PinCfg.GPIO_PinNumber = GPIO_PIN_6;
		PinCfg.GPIO_MODE = GPIO_MODE_AF_OUTPUT_OPENDRAIN;
		PinCfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10MHZ;
		MCAL_GPIO_Init(GPIOB, &PinCfg);


		PinCfg.GPIO_PinNumber = GPIO_PIN_7;
		PinCfg.GPIO_MODE = GPIO_MODE_AF_OUTPUT_OPENDRAIN;
		PinCfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10MHZ;
		MCAL_GPIO_Init(GPIOB, &PinCfg);


	}else if(I2Cx == I2C2)
	{
		// PB10: I2C2_SCL --> Open drain
		// PB11: I2C2_SDA --> Open drain

		PinCfg.GPIO_PinNumber = GPIO_PIN_10;
		PinCfg.GPIO_MODE = GPIO_MODE_AF_OUTPUT_OPENDRAIN;
		PinCfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10MHZ;
		MCAL_GPIO_Init(GPIOB, &PinCfg);


		PinCfg.GPIO_PinNumber = GPIO_PIN_11;
		PinCfg.GPIO_MODE = GPIO_MODE_AF_OUTPUT_OPENDRAIN;
		PinCfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10MHZ;
		MCAL_GPIO_Init(GPIOB, &PinCfg);

	}

}


// -------------------------------------------------------------------------
// ======================= Master APIs : Polling Mechanism  ================================
// -------------------------------------------------------------------------


/**================================================================
 * @Fn					- MCAL_I2C_Master_Transmit
 * @brief 				- Master transmit function
 * @param [in] 			- I2Cx: where x can be (1 or 2) depending on the device used
 * @param [in] 			- slaveAddress: the address of slave you want to transmit to
 * @param [in] 			- dataOut: A buffer holds the data to be sent
 * @param [in] 			- dataLen: Length of data in bytes
 * @param [in] 			- Stop: Enabling or disabling stop condition
 * @param [in] 			- Start: If you want to send a Start or repeated start
 * @retval 				- None
 * Note					- Supports Master in polling mechanism
 *
 */


void MCAL_I2C_Master_Transmit(I2C_Typedef *I2Cx, uint16_t slaveAddress, uint8_t *dataOut, uint32_t dataLen, Stop_Condition Stop, Repeated_Start Start)
{
	int i = 0;
	// todo
	// Support timeout (Configure timer working for specific duration, raise interrupt
	// timer_isr(){flag = 1}
	// So any code exist in while (check flag || Any polling condition)

	// 1- Set START bit in I2C_CRQ to generate a start condition
	I2C_Generate_START(I2Cx, ENABLE, Start);

	// 2- Wait for EV5: SB=1, cleared by reading SR1 register followed by writing DR register with Address
	while(!I2C_GetFlagStatus(I2Cx, EV5));


	// 3- Send Address
	I2C_SendAddress(I2Cx, slaveAddress, I2C_Direction_Transmitter);


	// .. Wait for EV6: ADDR=1, cleared by reading SR1 register followed by reading SR2.
	while(!I2C_GetFlagStatus(I2Cx, EV6));


	// 4- Check : TRA, BUSY, MSL, TXE Flags
	while(!I2C_GetFlagStatus(I2Cx, MASTER_BYTE_TRANSMITTING));

	// 5- Send Data
	for(i=0;i<dataLen;i++)
	{
		// Write in the DR register the data to be sent
		I2Cx->DR = dataOut[i];

		// Wait EV8: TxE=1, shift register not empty, d . ata register empty, cleared by writing DR register
		while(!I2C_GetFlagStatus(I2Cx, EV8));
	}


	// Already the last byte checked in the for loop
	//	EV8_2: TxE=1, BTF = 1, Program Stop request. TxE and BTF are cleared by hardware by the Stop condition
	//	while(!I2C_GetFlagStatus(I2Cx, EV8_2));


	// 6- Stop Condition
	if(Stop == With_Stop)
	{
		I2C_Generate_STOP(I2Cx,ENABLE);
	}

}



/**================================================================
 * @Fn					- MCAL_I2C_Master_Receive
 * @brief 				- Master transmit function
 * @param [in] 			- I2Cx: where x can be (1 or 2) depending on the device used
 * @param [in] 			- slaveAddress: the address of slave you want to transmit to
 * @param [in] 			- dataOut: A buffer holds the received data
 * @param [in] 			- dataLen: Length of data in bytes
 * @param [in] 			- Stop: Enabling or disabling stop condition
 * @param [in] 			- Start: If you want to send a Start or repeated start
 * @retval 				- None
 * Note					- Supports Master in polling mechanism
 *
 */




void MCAL_I2C_Master_Receive(I2C_Typedef *I2Cx, uint16_t slaveAddress, uint8_t *dataOut, uint32_t dataLen, Stop_Condition Stop, Repeated_Start Start)
{
	int i=0;

	uint8_t index = I2Cx == I2C1 ? I2C1_INDEX : I2C2_INDEX;


	// 1- Set START bit in I2C_CRQ to generate a start condition
	I2C_Generate_START(I2Cx, ENABLE, RepeatedStart);

	// 2- Wait for EV5: SB=1, cleared by reading SR1 register followed by writing DR register with Address
	while(!I2C_GetFlagStatus(I2Cx, EV5));


	// 3- Send Address
	I2C_SendAddress(I2Cx, slaveAddress, I2C_Direction_Reciever);


	// .. Wait for EV6: ADDR=1, cleared by reading SR1 register followed by reading SR2.
	while(!I2C_GetFlagStatus(I2Cx, EV6));


	// Enabling Acknowledge
	I2C_AcknowledgeConfig(I2Cx,ENABLE);


	if(dataLen)
	{
		for(i=dataLen;i>1;i--)
		{
			// Wait for EV7: RXNE=1, Cleared by reading DR
			while(!I2C_GetFlagStatus(I2Cx, EV7));
			// Read data from register
			*dataOut = I2Cx->DR;
			dataOut++;
		}
		// Once receiving the last byte, we want to send NACK
		I2C_AcknowledgeConfig(I2Cx,DISABLE);
	}

	// Generate Stop Condition
	if(Stop == With_Stop)
	{
		I2C_Generate_STOP(I2Cx, ENABLE);
	}


	// Re-enabling the acknowldege According to the original configuration
	if(Global_I2C_Config[index].ACK_Control == I2C_ACK_ENABLE)
	{
		I2C_AcknowledgeConfig(I2Cx,ENABLE);
	}

}




// -------------------------------------------------------------------------
// ======================= Slave APIs : Interrupt Mechanism  ================================
// -------------------------------------------------------------------------


/**================================================================
 * @Fn					- MCAL_I2C_Slave_Transmit
 * @brief 				- To send data in case you were a slave
 * @param [in] 			- I2Cx: where x can be (1 or 2) depending on the device used
 * @param [in] 			- data: the data to be sent
 * @retval 				- None
 * Note					- None
 *
 */


void MCAL_I2C_Slave_Transmit(I2C_Typedef *I2Cx, uint8_t data)
{
	I2Cx->DR = data;
}




/**================================================================
 * @Fn					- MCAL_I2C_Slave_Recieve
 * @brief 				- To send data in case you were a slave
 * @param [in] 			- I2Cx: where x can be (1 or 2) depending on the device used
 * @retval 				- the received data
 * Note					- None
 *
 */


uint8_t MCAL_I2C_Slave_Recieve(I2C_Typedef *I2Cx)
{
	return ((uint8_t)(I2Cx->DR));
}


// -------------------------------------------------------------------------
// ======================= Generic APIs  ================================
// -------------------------------------------------------------------------



/**================================================================
 * @Fn					- I2C_Generate_START
 * @brief 				- Generate start or repeated start
 * @param [in] 			- I2Cx: where x can be (1 or 2) depending on the device used
 * @param [in] 			- FunctionalState: Enabling or disabling start
 * @param [in] 		    - Start: If you want to send a Start or repeated start
 * @retval 				- None
 * Note					- None
 *
 */


void I2C_Generate_START(I2C_Typedef *I2Cx, FunctionalState NewState,Repeated_Start Start)
{
	// Check if start or repeated start
	if(Start != RepeatedStart)
	{
		// Check if bus is idle
		while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
	}

	//	Bit 8 START: Start generation
	//	This bit is set and cleared by software and cleared by hardware when start is sent or PE=0.
	//	In Master Mode:
	//	0: No Start generation
	//	1: Repeated start generation
	//	In Slave mode:
	//	0: No Start generation
	//	1: Start generation when the bus is free

	// Enable start condition
	if(NewState != DISABLE)
	{
		I2Cx->CR1 |= I2C_CR1_START;
	}else
	{
		// Disable Start condition
		I2Cx->CR1 &= ~(I2C_CR1_START);
	}


}



/**================================================================
 * @Fn					- I2C_GetFlagStatus
 * @brief 				- Gets the status of specific flag
 * @param [in] 			- I2Cx: where x can be (1 or 2) depending on the device used
 * @param [in] 			- flag: the flag you want to check its status
 * @retval 				- None
 * Note					- None
 *
 */


Flag_Status I2C_GetFlagStatus(I2C_Typedef* I2Cx, Status flag)
{
	volatile uint32_t dummyRead;   // To read from
	uint32_t flag1=0,flag2=0;
	uint32_t lastEvent = 0;
	Flag_Status BitStatus = RESET;
	switch(flag)
	{
	case I2C_FLAG_BUSY: // (Bit 1 BUSY: Bus busy in SR2)
	{
		//		0: No communication on the bus
		//		1: Communication ongoing on the bus
		//		– Set by hardware on detection of SDA or SCL low
		//		– cleared by hardware on detection of a Stop condition.
		//		It indicates a communication in progress on the bus. This information is still updated when
		//		the interface is disabled (PE=0)
		if(I2Cx->SR2 & (I2C_SR2_BUSY))
			BitStatus = SET;
		else
			BitStatus = RESET;
		break;
	}
	case EV5:
	{
		//		Bit 0 SB: Start bit (Master mode)
		//		0: No Start condition
		//		1: Start condition generated.
		//		– Set when a Start condition generated.
		//		– Cleared by software by reading the SR1 register followed by writing the DR register, or by
		//		hardware when PE=0
		if(I2Cx->SR1 & (I2C_SR1_SB))
			BitStatus = SET;
		else
			BitStatus = RESET;
		break;
	}
	case EV6:
	{
		//		Address sent (Master)
		//		0: No end of address transmission
		//		1: End of address transmission
		if(I2Cx->SR1 & (I2C_SR1_ADDR))
			BitStatus = SET;
		else
			BitStatus = RESET;
//		//		ADDR=1, cleared by reading SR1 register followed by reading SR2
//		dummyRead = I2Cx->SR2;
		break;
	}
	case MASTER_BYTE_TRANSMITTING:
	{
		flag1 = I2Cx->SR1;
		flag2 = I2Cx->SR2;
		flag2 = flag2 << 16;
		// Concatenate the two registers
		lastEvent = (flag1 | flag2) & ((uint32_t)(0x00FFFFFF));
		if((lastEvent & flag) == flag)
		{
			BitStatus = SET;
		}else{
			BitStatus = RESET;
		}
		break;
	}
	case EV8_1:
	case EV8:
	{
		//		EV8_1: TxE=1, shift register empty, data register empty, write Data1 in DR.
		//		EV8: TxE=1, shift register not empty, d . ata register empty, cleared by writing DR register
		if((I2Cx->SR1)& (I2C_SR1_TXE))
		{
			BitStatus = SET;
		}else{
			BitStatus = RESET;
		}
		break;
	}
	case EV7:
	{
		//		Bit 6 RxNE: Data register not empty (receivers)
		//		0: Data register empty
		//		1: Data register not empty
		if((I2Cx->SR1)& (I2C_SR1_RXNE))
		{
			BitStatus = SET;
		}else{
			BitStatus = RESET;
		}


		break;
	}
	}


	return BitStatus;
}



/**================================================================
 * @Fn					- I2C_SendAddress
 * @brief 				- Generate start or repeated start
 * @param [in] 			- I2Cx: where x can be (1 or 2) depending on the device used
 * @param [in] 			- Address: The address you want to send
 * @param [in] 			- direction: Determines whether you are a transmitter or reciever
 * @retval 				- None
 * Note					- Supports only 7-bit address mode
 *
 */


void I2C_SendAddress(I2C_Typedef* I2Cx, uint16_t Address, I2C_Direction direction)
{
	uint8_t index = I2Cx == I2C1 ? I2C1_INDEX : I2C2_INDEX;


	// Check if 7-bit address or 10-bit address
	if(Global_I2C_Config[index].I2C_Slave_Address.Addressing_Bits == I2C_ADDRESSING_7BIT)
	{
		Address = (Address << 1);


		if(direction != I2C_Direction_Transmitter)
		{
			Address |= 1<<0;
		}else
		{
			// Reset address bit0 to write
			Address &= ~(1<<0);
		}

		// Send address
		I2Cx->DR = Address;
	}else
	{
		// Not supported yet
	}


}



/**================================================================
 * @Fn					- I2C_Generate_STOP
 * @brief 				- Generate stop condition
 * @param [in] 			- I2Cx: where x can be (1 or 2) depending on the device used
 * @param [in] 			- FunctionalState: Enabling or disabling stop
 * @retval 				- None
 * Note					- Supports Master in polling mechanism & slave interrupt Mechanism
 *
 */


void I2C_Generate_STOP(I2C_Typedef *I2Cx, FunctionalState NewState)
{
	// STOP: 9 in CR1

	if(NewState != DISABLE)
	{
		I2Cx->CR1 |= I2C_CR1_STOP;
	}else{
		I2Cx->CR1 &= ~(I2C_CR1_STOP);
	}
}


/**================================================================
 * @Fn					- I2C_AcknowledgeConfig
 * @brief 				- Enabling or disbling acknowledge
 * @param [in] 			- I2Cx: where x can be (1 or 2) depending on the device used
 * @param [in] 			- FunctionalState: Enabling or disabling start
 * @retval 				- None
 * Note					- None
 *
 */


void I2C_AcknowledgeConfig(I2C_Typedef* I2Cx, FunctionalState NewState)
{

	if (NewState != DISABLE)
	{
		/* Enable the acknowledgement */
		I2Cx->CR1 |= I2C_CR1_ACK;
	}
	else
	{
		/* Disable the acknowledgement */
		I2Cx->CR1 &= ~(I2C_CR1_ACK);
	}
}



// -------------------------------------------------------------------------
// ======================= Interrupts  ================================
// -------------------------------------------------------------------------




void I2C1_EV_IRQHandler(void)
{
	volatile uint8_t dummy_read = 0;
	I2C_Typedef *I2Cx = I2C1;

	uint32_t temp1,temp2,temp3;

	temp1 = I2Cx->CR2 & (I2C_CR2_ITEVTEN);
	temp2 = I2Cx->CR2 & (I2C_CR2_ITBUFEN);

	temp3 = I2Cx->SR1 & (I2C_SR1_STOPF);

	// Handle For interrupt generated by STOPF event
	// Note : Stop detection flag is applicable only slave mode
	if(temp1 && temp3)
	{
		//STOF flag is set
		//Clear the STOPF ( i.e 1) read SR1 2) Write to CR1 )
		I2Cx->CR1 |= 0x0000;
		Slave_States(I2Cx,I2C_EV_STOP);


	}
	//----------------------------------------------------------
	temp3  = I2Cx->SR1 & (I2C_SR1_ADDR);
	// Handle For interrupt generated by ADDR event
	//Note : When master mode : Address is sent
	//		 When Slave mode   : Address matched with own address
	if(temp1 && temp3)
	{
		// interrupt is generated because of ADDR event
		//check for device mode
		if(I2Cx->SR2 & ( I2C_SR2_MSL))
		{
			//master

		}else
		{
			//slave mode
			//clear the ADDR flag ( read SR1 , read SR2)
			dummy_read = I2Cx->SR1;
			dummy_read = I2Cx->SR2;
			Slave_States(I2Cx,I2C_EV_ADDR_Matched);

		}
	}
	//----------------------------------------------------------

	temp3  = I2Cx->SR1 & ( I2C_SR1_TXE);
	// Handle For interrupt generated by TXE event
	if(temp1 && temp2 && temp3)
	{
		//Check for device mode
		if(I2Cx->SR2 & (I2C_SR2_MSL))
		{

		}else
		{
			//slave
			Slave_States(I2Cx,I2C_EV_DATA_REQ);
		}
	}

	//----------------------------------------------------------

	temp3  = I2Cx->SR1 & ( I2C_SR1_RXNE);
	// Handle For interrupt generated by 	SB=1, cleared by reading SR1 register followed by writing DR register with Address.
	if(temp1 && temp2 && temp3)
	{
		//check device mode .
		if(I2Cx->SR2 & ( I2C_SR2_MSL))
		{
			//The device is master

		}else
		{
			//slave
			Slave_States(I2Cx,I2C_EV_DATA_RCV);

		}
	}

}


void I2C1_ER_IRQHandler(void)
{

}


void I2C2_EV_IRQHandler(void)
{

}


void I2C2_ER_IRQHandler(void)
{

}



void Slave_States (I2C_Typedef* I2Cx  ,Slave_State state)
{
	uint8_t index =    I2Cx == I2C1 ? I2C1_INDEX: I2C2_INDEX ;

	switch (state)
	{

	case I2C_ERROR_AF:
	{
		//make sure that the slave is really in transmitter mode
		if(I2Cx->SR2 & ( I2C_SR2_TRA))
		{
			//Slave Shouldn't Send anything else

		}

		break ;
	}

	case I2C_EV_STOP:
	{
		//make sure that the slave is really in transmitter mode
		if(I2Cx->SR2 & ( I2C_SR2_TRA))
		{
			//Notify APP that the Stop Condition is sent by the master
			Global_I2C_Config [index].P_Slave_Event_CallBack (I2C_EV_STOP) ;
		}

		break ;
	}

	case I2C_EV_ADDR_Matched:
	{
		//Notify APP that the Stop Condition is sent by the master
		Global_I2C_Config [index].P_Slave_Event_CallBack (I2C_EV_ADDR_Matched) ;

		break ;
	}

	case I2C_EV_DATA_REQ:
	{
		//make sure that the slave is really in transmitter mode
		if(I2Cx->SR2 & ( I2C_SR2_TRA))
		{
			//the APP layer should send the data (MCAL_I2C_SlaveSendData ) in this state
			Global_I2C_Config [index].P_Slave_Event_CallBack (I2C_EV_DATA_REQ) ;
		}

		break ;
	}
	case I2C_EV_DATA_RCV:
	{
		//make sure that the slave is really in receiver mode
		if(!(I2Cx->SR2 & ( I2C_SR2_TRA)))
		{
			//the APP layer should read the data (MCAL_I2C_SlaveReceiveData ) in this state
			Global_I2C_Config [index].P_Slave_Event_CallBack (I2C_EV_DATA_RCV) ;
		}

		break ;
	}
	}

}
