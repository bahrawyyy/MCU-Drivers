/*
 * I2C.h
 *
 *  Created on: Jul 29, 2023
 *      Author: Abdallah
 */

#ifndef MCAL_I2C_I2C_H_
#define MCAL_I2C_I2C_H_

#include "../Registers.h"
#include "../../LIB/Bit_Math.h"
#include "../../LIB/STD_Types.h"

#define F_CPU 16000000UL


// TWSR STATUS CODES
#define TWSR_START_TRANSMITTED	0x08
#define TWSR_Add_ACK_RETURNED	0x60
#define TWSR_Data_ACK_RETURNED  0x80

// Prescalares used in Bit rate
#define I2C_PRESCALAR_1			1
#define I2C_PRESCALAR_4			4
#define I2C_PRESCALAR_16		16
#define I2C_PRESCALAR_64		64

#define I2C_PRESCALAR	I2C_PRESCALAR_1



// APIs
void I2C_Init(unsigned long SCL_Clock);
void I2C_Start(void);
void I2C_Write(u8 cByte);
void I2C_Stop(void);
void I2C_Set_Address(u8 slaveAddress);
u8 I2C_Slave_Read(u8 *status);

u8 I2C_Read(void);


#endif /* MCAL_I2C_I2C_H_ */
