/*
 * Stm32_F103C6_I2C_driver.h
 *
 *  Created on: Jul 30, 2023
 *      Author: Abdallah
 */

#ifndef INC_STM32_F103C6_I2C_DRIVER_H_
#define INC_STM32_F103C6_I2C_DRIVER_H_


#include "stm32f103c6.h"
#include "Stm32_F103C6_gpio_driver.h"
#include "Stm32_F103C6_RCC_driver.h"

// -------------------------------------------------------------------------
// ======================= Configuration structure =========================
// -------------------------------------------------------------------------


#define I2C_EVENT_MASTER_BYTE_TRANSMITTING		((uint32_t)(0x00070080))	// TRA, BUSY, MSL, TXE

typedef enum
{
	With_Stop,
	Without_Stop
}Stop_Condition;



typedef enum
{
	Start,
	RepeatedStart
}Repeated_Start;


typedef enum
{
	DISABLE,
	ENABLE
}FunctionalState;


typedef enum
{
	RESET,
	SET
}Flag_Status;


typedef enum
{
	I2C_FLAG_BUSY=0,
	EV5,   // EV5 Start condition has been sent
	EV6,    // Address has been sent
	EV8,	// Wait EV8: TxE=1, shift register not empty, d . ata register empty, cleared by writing DR register
	EV8_1,
	EV7,	// RXNE=1, Cleared by reading DR
	MASTER_BYTE_TRANSMITTING = ((uint32_t)(0x00070080))   // TRA, BUSY, MSL, TXE
}Status;


struct S_I2C_SlaveDevice_Address
{
	uint16_t	Dual_Address_Enable;		// Enable or disable
	uint16_t	Primary_Slave_Address;
	uint16_t	Secondary_Slave_Address;
	uint32_t	Addressing_Bits;			// @ref I2C_ADDRESSING_BITS
};


typedef enum
{
	I2C_EV_STOP,
	I2C_ERROR_AF,
	I2C_EV_ADDR_Matched,
	I2C_EV_DATA_REQ,	// The APP Layer should send the data (I2C_Slave_SendData)
	I2C_EV_DATA_RCV     // The APP Layer should read the data (I2C_Slave_ReceiveData)
}Slave_State;


typedef enum
{
	I2C_Direction_Transmitter=0,
	I2C_Direction_Reciever
}I2C_Direction;

typedef struct
{
	uint32_t		I2C_Clock_Speed;	// Specifies the clock frequency of I2C
										// This parameter must be defined based on @ref I2C_SCLCK

	uint32_t		Stretch_Mode;		// Enable or disable clock stretching
										// This parameter must be defined based on @ref I2C_CLK_Stretch

	uint32_t		I2C_Mode;			// Specifies I2C mode or SMBUS
										// This parameter must be defined based on @ref I2C_MODE

	struct S_I2C_SlaveDevice_Address		I2C_Slave_Address;


	uint32_t		ACK_Control;		// Enable or disbale Acknowledge @ref I2C_Ack_define


	uint32_t		General_Call_Address_Detection;			// @ref I2C_ENGC


	void(*P_Slave_Event_CallBack)(Slave_State State);

}I2C_Config_t;

// -------------------------------------------------------------------------
// ======================= Reference Macros ================================
// -------------------------------------------------------------------------

// @ref I2C_SCLCK
//– Standard Speed (up to 100 kHz)
//– Fast Speed (up to 400 kHz)
// To configure clock before enabling the peripheral
// 1- in I2C_CR2 Bits 5:0 FREQ[5:0]: Peripheral clock frequency from APB1
// 2- Configure the clock control register (Thigh = CCR * TPCLK1)
// 			SM or FM
// 3- Configure the rise time register (I2C_TRISE)
#define I2C_SCLK_SM_50K					(50000U)
#define I2C_SCLK_SM_100K				(100000U)
// Fast mode not supported yet
#define I2C_SCLK_FM_200K				(200000U)
#define I2C_SCLK_FM_400K				(400000U)


// @ref I2C_CLK_Stretch (Bit 7 NOSTRETCH: Clock stretching disable (Slave mode) in CR1
#define I2C_CLK_Stretch_Enable			0x00000000U
#define I2C_CLK_Stretch_Disable			I2C_CR1_NOSTRETCH


// @ref I2C_MODE (Bit 1 SMBUS: SMBus mod)
#define I2C_MODE_I2C					0x00000000U
#define I2C_MODE_SMBUS					I2C_CR1_SMBUS


// @ref I2C_ADDRESSING_BITS (Bit 15 ADDMODE Addressing mode (slave mode) in OAR1
#define I2C_ADDRESSING_7BIT				0x00000000U
#define I2C_ADDRESSING_10BIT			(uint16_t)(1<<15)


// @ref I2C_Ack_define
#define I2C_ACK_ENABLE					I2C_CR1_ACK
#define I2C_ACK_DISABLE					((uint16_t)0x0000)


// @ref I2C_ENGC
#define I2C_ENGC_ENABLE					I2C_CR1_ENGC
#define I2C_ENGC_DISABLE				0x00000000U


// -------------------------------------------------------------------------
// ======================= APIs supported by I2C DRIVER  ================================
// -------------------------------------------------------------------------

void MCAL_I2C_Init(I2C_Typedef* I2Cx, I2C_Config_t *I2C_Cfg);
void MCAL_I2C_DeInit(I2C_Typedef* I2Cx, I2C_Config_t *I2C_Cfg);


void MCAL_I2C_GPIO_Set_Pins(I2C_Typedef* I2Cx);


// Master Polling Mechanism
// Stop condition to disable in case of repeated start
void MCAL_I2C_Master_Transmit(I2C_Typedef *I2Cx, uint16_t slaveAddress, uint8_t *dataOut, uint32_t dataLen, Stop_Condition Stop, Repeated_Start Start);
void MCAL_I2C_Master_Receive(I2C_Typedef *I2Cx, uint16_t slaveAddress, uint8_t *dataOut, uint32_t dataLen, Stop_Condition Stop, Repeated_Start Start);


// Slave interrupt Mechanism
void MCAL_I2C_Slave_Transmit(I2C_Typedef *I2Cx, uint8_t data);
uint8_t MCAL_I2C_Slave_Recieve(I2C_Typedef *I2Cx);


void Slave_States (I2C_Typedef* I2Cx  ,Slave_State state);



// Generic APIs
void I2C_Generate_START(I2C_Typedef *I2Cx, FunctionalState NewState,Repeated_Start Start);
void I2C_Generate_STOP(I2C_Typedef *I2Cx, FunctionalState NewState);


Flag_Status I2C_GetFlagStatus(I2C_Typedef* I2Cx, Status flag);

// I2C_Direction: Read or write
void I2C_SendAddress(I2C_Typedef* I2Cx, uint16_t Address, I2C_Direction direction);

void I2C_AcknowledgeConfig(I2C_Typedef* I2Cx, FunctionalState NewState);


#endif /* INC_STM32_F103C6_I2C_DRIVER_H_ */
