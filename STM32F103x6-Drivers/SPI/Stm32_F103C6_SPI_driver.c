/*
 * Stm32_F103C6_SPI_driver.c
 *
 *  Created on: Jul 25, 2023
 *      Author: Abdallah
 */



#include "Stm32_F103C6_SPI_driver.h"





/**
 * -----------------
 * Generic Variables
 * -----------------
 *
 */


SPI_Config* Global_SPI_Config[2] = {NULL,NULL};
// Due to the problem of stacks in case of calling
// the function init outside main
SPI_Config Global_SPI1_Config;
SPI_Config Global_SPI2_Config;

#define SPI1_INDEX		0
#define SPI2_INDEX		1


#define SPI_SR_TXE			(uint8_t)(0x02)		// Transmit buffer empty
#define SPI_SR_RXNE			(uint8_t)(0x01)		// Receive buffer not empty


/**================================================================
 * @Fn					- MCAL_SPI_Init
 * @brief 				- Initializes SPI according to the specified parameters
 * @param [in] 			- SPIx: where x can be (1 or 2) depending on the device used
 * @param [in] 			- SPI_Cfg: All the SPI configurations
 * @retval 				- None
 * Note					- Supports SPI full duplex Master/Slave only & NSS HW/SW
 * 						in case of master you have to configure pin and drive it
 */


void MCAL_SPI_Init(SPI_Typedef* SPIx, SPI_Config* SPI_Cfg)
{
	// Safety for registers
	// To prevent SPI from running before completing all configurations
	uint16_t tmpreg_CR1 = 0;
	uint16_t tmpreg_CR2 = 0;


	if(SPIx == SPI1)
	{
		Global_SPI1_Config = *SPI_Cfg;
		Global_SPI_Config[SPI1_INDEX] = &Global_SPI1_Config;
		RCC_SPI1_CLK_EN();
	}else if(SPIx == SPI2)
	{
		Global_SPI2_Config = *SPI_Cfg;
		Global_SPI_Config[SPI2_INDEX] = &Global_SPI2_Config;
		RCC_SPI2_CLK_EN();
	}


	// Enable SPI (CR1:6 SPE)
	tmpreg_CR1 |= (1<<6);


	// Master or slave
	tmpreg_CR1 |= SPI_Cfg->Device_Mode;


	// SPI Communication mode
	tmpreg_CR1 |= SPI_Cfg->Communication_Mode;


	// SPI frame format
	tmpreg_CR1 |= SPI_Cfg->Frame_Format;


	// Data size
	tmpreg_CR1 |= SPI_Cfg->DataSize;


	// Clock polarity
	tmpreg_CR1 |= SPI_Cfg->CLKPolarity;


	// Clock phase
	tmpreg_CR1 |= SPI_Cfg->CLKPhase;


	// ===================== NSS ===================

	if(SPI_Cfg->NSS == SPI_NSS_HW_MASTER_Output_Enabled)
	{
		tmpreg_CR2 |= SPI_Cfg->NSS;
	}else if(SPI_Cfg->NSS == SPI_NSS_HW_MASTER_Output_Disabled)
	{
		tmpreg_CR2 &= SPI_Cfg->NSS;
	}else
	{
		tmpreg_CR1 |= SPI_Cfg->NSS;
	}


	// SPI Baud rate
	tmpreg_CR1 |= SPI_Cfg->Baudrate_Prescalar;



	// ===================== IRQ ==================

	if(SPI_Cfg->IRQ_Enable != SPI_IRQ_NONE)
	{
		tmpreg_CR2 |= SPI_Cfg->IRQ_Enable;

		if(SPIx == SPI1){
			NVIC_IRQ35_SPI1_Enable;
		}else if(SPIx == SPI2){
			NVIC_IRQ36_SPI2_Enable;
		}
	}
	SPIx->CR1 = tmpreg_CR1;
	SPIx->CR2 = tmpreg_CR2;
}



/**================================================================
 * @Fn					- MCAL_SPI_DeInit
 * @brief 				- Initializes SPI according to the specified parameters
 * @param [in] 			- SPIx: where x can be (1 or 2) depending on the device used
 * @param [in] 			- SPI_Cfg: All the SPI configurations
 * @retval 				- None
 * Note					- Supports SPI full duplex Master/Slave only & NSS HW/SW
 * 						in case of master you have to configure pin and drive it
 */

void MCAL_SPI_DeInit(SPI_Typedef* SPIx, SPI_Config* SPI_Cfg)
{
	if(SPIx == SPI1){
		NVIC_IRQ35_SPI1_Disable;
		RCC_SPI1_Reset();
	}else if(SPIx == SPI2){
		NVIC_IRQ36_SPI2_Disable;
		RCC_SPI2_Reset();
	}
}


/**================================================================
 * @Fn					- MCAL_SPI_Send_Data
 * @brief 				- Send data via SPI
 * @param [in] 			- SPIx: where x can be (1 or 2) depending on the device used
 * @param [in] 			- SPI_Cfg: All the SPI configurations
 * @param [in] 			- PollingEn: Whether you're using polling or interrupt
 * @retval 				- None
 * Note					- None
 */

void MCAL_SPI_Send_Data(SPI_Typedef* SPIx, uint16_t *pTxBuffer, enum PollingMechanism PollingEn)
{
	// todo receive only or transmit only, but here we support full duplex only

	if(PollingEn == Polling_Enable)
		while(!((SPIx->SR) & SPI_SR_TXE));
	SPIx->DR = *pTxBuffer;


}


/**================================================================
 * @Fn					- MCAL_SPI_Receive_Data
 * @brief 				- Receive data via SPI
 * @param [in] 			- SPIx: where x can be (1 or 2) depending on the device used
 * @param [in] 			- SPI_Cfg: All the SPI configurations
 * @param [in] 			- PollingEn: Whether you're using polling or interrupt
 * @retval 				- None
 * Note					- None
 */


void MCAL_SPI_Receive_Data(SPI_Typedef* SPIx, uint16_t *pRxBuffer, enum PollingMechanism PollingEn)
{
	if(PollingEn == Polling_Enable)
		while(!((SPIx->SR) & SPI_SR_RXNE));
	*pRxBuffer = SPIx->DR;

}



/**================================================================
 * @Fn					- MCAL_SPI_TX_RX
 * @brief 				- To transmit and receive data via SPI
 * @param [in] 			- SPIx: where x can be (1 or 2) depending on the device used
 * @param [in] 			- SPI_Cfg: All the SPI configurations
 * @param [in] 			- PollingEn: Whether you're using polling or interrupt
 * @retval 				- None
 * Note					- None
 */


void MCAL_SPI_TX_RX(SPI_Typedef* SPIx, uint16_t *pTxBuffer, enum PollingMechanism PollingEn)
{
	if(PollingEn == Polling_Enable)
		while(!((SPIx->SR) & SPI_SR_TXE));
	SPIx->DR = *pTxBuffer;

	if(PollingEn == Polling_Enable)
		while(!((SPIx->SR) & SPI_SR_RXNE));
	*pTxBuffer = SPIx->DR;

}


/**================================================================
 * @Fn					- MCAL_SPI_GPIO_Set_Pins
 * @brief 				- Configure the pins as recommended in the data sheer
 * @param [in] 			- SPIx: where x can be (1 or 2) depending on the device used
 * @retval 				- None
 * Note					- None
 */


void MCAL_SPI_GPIO_Set_Pins(SPI_Typedef* SPIx)
{
	GPIO_PinConfig_t GPIO_Cfg;


	if(SPIx == SPI1){
		// PA4 : NSS
		if(Global_SPI_Config[SPI1_INDEX]->Device_Mode == SPI_Device_Mode_Master)
		{

			switch(Global_SPI_Config[SPI1_INDEX]->NSS){
			case SPI_NSS_HW_MASTER_Output_Disabled:   // As input to sense if there are other masters
				// Hardware master /slave Input floating
				GPIO_Cfg.GPIO_MODE = GPIO_MODE_INPUT_FLOATING;
				GPIO_Cfg.GPIO_PinNumber = GPIO_PIN_4;
				MCAL_GPIO_Init(GPIOA, &GPIO_Cfg);
				break;
			case SPI_NSS_HW_MASTER_Output_Enabled:
				// Hardware master/ NSS output enabled Alternate function push-pull
				GPIO_Cfg.GPIO_MODE = GPIO_MODE_AF_OUTPUT_PUSHPULL;
				GPIO_Cfg.GPIO_PinNumber = GPIO_PIN_4;
				GPIO_Cfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10MHZ;
				MCAL_GPIO_Init(GPIOA, &GPIO_Cfg);
				break;
			}

			// PA5 : SCK
			// Master Alternate function push-pull
			GPIO_Cfg.GPIO_MODE = GPIO_MODE_AF_OUTPUT_PUSHPULL;
			GPIO_Cfg.GPIO_PinNumber = GPIO_PIN_5;
			GPIO_Cfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10MHZ;
			MCAL_GPIO_Init(GPIOA, &GPIO_Cfg);


			// PA6 : MISO
			// Full duplex / master Input floating
			GPIO_Cfg.GPIO_MODE = GPIO_MODE_INPUT_FLOATING;
			GPIO_Cfg.GPIO_PinNumber = GPIO_PIN_6;
			MCAL_GPIO_Init(GPIOA, &GPIO_Cfg);

			// PA7 : MOSI
			// Full duplex / master Alternate function push-pull
			GPIO_Cfg.GPIO_MODE = GPIO_MODE_AF_OUTPUT_PUSHPULL;
			GPIO_Cfg.GPIO_PinNumber = GPIO_PIN_7;
			GPIO_Cfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10MHZ;
			MCAL_GPIO_Init(GPIOA, &GPIO_Cfg);


		}else{ // Slave


			// PA4 : NSS

			// Just use NSS in case of it is configured as hardware
			if(Global_SPI_Config[SPI1_INDEX]->NSS == SPI_NSS_HW_SLAVE){
				// Slave Input floating
				GPIO_Cfg.GPIO_MODE = GPIO_MODE_INPUT_FLOATING;
				GPIO_Cfg.GPIO_PinNumber = GPIO_PIN_4;
				MCAL_GPIO_Init(GPIOA, &GPIO_Cfg);
			}


			// PA5 : SCK
			// Slave Input floating
			GPIO_Cfg.GPIO_MODE = GPIO_MODE_INPUT_FLOATING;
			GPIO_Cfg.GPIO_PinNumber = GPIO_PIN_5;
			MCAL_GPIO_Init(GPIOA, &GPIO_Cfg);


			// PA6 : MISO
			// Full duplex / slave (point to point) Alternate function push-pull
			GPIO_Cfg.GPIO_MODE = GPIO_MODE_AF_OUTPUT_PUSHPULL;
			GPIO_Cfg.GPIO_PinNumber = GPIO_PIN_6;
			GPIO_Cfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10MHZ;
			MCAL_GPIO_Init(GPIOA, &GPIO_Cfg);


			// PA7 : MOSI
			// Full duplex / slave Input floating
			GPIO_PinConfig_t GPIO_Cfg;
			GPIO_Cfg.GPIO_MODE = GPIO_MODE_INPUT_FLOATING;
			GPIO_Cfg.GPIO_PinNumber = GPIO_PIN_7;
			MCAL_GPIO_Init(GPIOA, &GPIO_Cfg);

		}


	}else if(SPIx == SPI2){
		// PB12 : NSS
		if(Global_SPI_Config[SPI2_INDEX]->Device_Mode == SPI_Device_Mode_Master)
		{

			switch(Global_SPI_Config[SPI1_INDEX]->NSS){
			case SPI_NSS_HW_MASTER_Output_Disabled:   // As input to sense if there are other masters
				// Hardware master /slave Input floating
				GPIO_Cfg.GPIO_MODE = GPIO_MODE_INPUT_FLOATING;
				GPIO_Cfg.GPIO_PinNumber = GPIO_PIN_12;
				MCAL_GPIO_Init(GPIOB, &GPIO_Cfg);
				break;
			case SPI_NSS_HW_MASTER_Output_Enabled:
				// Hardware master/ NSS output enabled Alternate function push-pull
				GPIO_Cfg.GPIO_MODE = GPIO_MODE_AF_OUTPUT_PUSHPULL;
				GPIO_Cfg.GPIO_PinNumber = GPIO_PIN_12;
				GPIO_Cfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10MHZ;
				MCAL_GPIO_Init(GPIOB, &GPIO_Cfg);
				break;
			}

			// PB13 : SCK
			// Master Alternate function push-pull
			GPIO_Cfg.GPIO_MODE = GPIO_MODE_AF_OUTPUT_PUSHPULL;
			GPIO_Cfg.GPIO_PinNumber = GPIO_PIN_13;
			GPIO_Cfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10MHZ;
			MCAL_GPIO_Init(GPIOB, &GPIO_Cfg);


			// PB14 : MISO
			// Full duplex / master Input floating
			GPIO_Cfg.GPIO_MODE = GPIO_MODE_INPUT_FLOATING;
			GPIO_Cfg.GPIO_PinNumber = GPIO_PIN_14;
			MCAL_GPIO_Init(GPIOB, &GPIO_Cfg);

			// PB15 : MOSI
			// Full duplex / master Alternate function push-pull
			GPIO_Cfg.GPIO_MODE = GPIO_MODE_AF_OUTPUT_PUSHPULL;
			GPIO_Cfg.GPIO_PinNumber = GPIO_PIN_15;
			GPIO_Cfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10MHZ;
			MCAL_GPIO_Init(GPIOB, &GPIO_Cfg);


		}else{ // Slave


			// PB12 : NSS

			// Just use NSS in case of it is configured as hardware
			if(Global_SPI_Config[SPI1_INDEX]->NSS == SPI_NSS_HW_SLAVE){
				// Slave Input floating
				GPIO_Cfg.GPIO_MODE = GPIO_MODE_INPUT_FLOATING;
				GPIO_Cfg.GPIO_PinNumber = GPIO_PIN_12;
				MCAL_GPIO_Init(GPIOB, &GPIO_Cfg);
			}


			// PB13 : SCK
			// Slave Input floating
			GPIO_Cfg.GPIO_MODE = GPIO_MODE_INPUT_FLOATING;
			GPIO_Cfg.GPIO_PinNumber = GPIO_PIN_13;
			MCAL_GPIO_Init(GPIOB, &GPIO_Cfg);


			// PB14 : MISO
			// Full duplex / slave (point to point) Alternate function push-pull
			GPIO_Cfg.GPIO_MODE = GPIO_MODE_AF_OUTPUT_PUSHPULL;
			GPIO_Cfg.GPIO_PinNumber = GPIO_PIN_14;
			GPIO_Cfg.GPIO_OUTPUT_SPEED = GPIO_SPEED_10MHZ;
			MCAL_GPIO_Init(GPIOB, &GPIO_Cfg);


			// PB15 : MOSI
			// Full duplex / slave Input floating
			GPIO_PinConfig_t GPIO_Cfg;
			GPIO_Cfg.GPIO_MODE = GPIO_MODE_INPUT_FLOATING;
			GPIO_Cfg.GPIO_PinNumber = GPIO_PIN_15;
			MCAL_GPIO_Init(GPIOB, &GPIO_Cfg);

		}

	}

}



/** ===================================
 * 			Interrupts
 * 	===================================
 */



void SPI1_IRQHandler(void)
{
	struct S_IRQ_SRC irq_src;

	irq_src.TXE = (((SPI1->SR) & (1<<1)) >> 1);
	irq_src.RXNE = (((SPI1->SR) & (1<<0)) >> 0);
	irq_src.ERRI = (((SPI1->SR) & (1<<4)) >> 4);

	Global_SPI_Config[SPI1_INDEX]->P_IRQ_CallBack(irq_src);
}


void SPI2_IRQHandler(void)
{
	struct S_IRQ_SRC irq_src;

	irq_src.TXE = (((SPI2->SR) & (1<<1)) >> 1);
	irq_src.RXNE = (((SPI2->SR) & (1<<0)) >> 0);
	irq_src.ERRI = (((SPI2->SR) & (1<<4)) >> 4);

	Global_SPI_Config[SPI2_INDEX]->P_IRQ_CallBack(irq_src);
}




