/*
 * I2C_Slave_EEPROM.h
 *
 *  Created on: Jul 31, 2023
 *      Author: Abdallah
 */

#ifndef INCLUDES_I2C_SLAVE_EEPROM_H_
#define INCLUDES_I2C_SLAVE_EEPROM_H_

#include "Stm32_F103C6_I2C_driver.h"

/**
 * E2PROM is an I2C slave
 * Idle Mode : Device is in high impedance state and waits for data
 * Master Transmitter Mode : the device transmits data to slave reciever
 * Master Reciever Mode : the device receives data from a slave transmitter
 */


#define EEPROM_Slave_Address		0x2A


void EEPROM_Init(void);
unsigned char EEPROM_Write_MBytes(unsigned int Memory_Address, unsigned char *bytes,uint8_t DataLength);
unsigned char EEPROM_Read_Bytes(unsigned int Memory_Address, uint8_t *data_out,uint8_t DataLength);



#endif /* INCLUDES_I2C_SLAVE_EEPROM_H_ */
