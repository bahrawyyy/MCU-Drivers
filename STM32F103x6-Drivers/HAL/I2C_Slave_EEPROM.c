/*
 * I2C_Slave_EEPROM.c
 *
 *  Created on: Jul 31, 2023
 *      Author: Abdallah
 */


#include "I2C_Slave_EEPROM.h"



void EEPROM_Init(void)
{
	// I2C_init
	// PB6: I2C1_SCL
	// PB7: I2C1_SDA

	I2C_Config_t I2C_Cfg;

	// I2C Controller act as a master
	I2C_Cfg.General_Call_Address_Detection = I2C_ENGC_ENABLE;
	I2C_Cfg.ACK_Control = I2C_ACK_ENABLE;
	I2C_Cfg.I2C_Clock_Speed = I2C_SCLK_SM_100K;
	I2C_Cfg.I2C_Mode = I2C_MODE_I2C;
	I2C_Cfg.P_Slave_Event_CallBack = NULL;
	I2C_Cfg.Stretch_Mode = I2C_CLK_Stretch_Enable;

	MCAL_I2C_GPIO_Set_Pins(I2C1);
	MCAL_I2C_Init(I2C1, &I2C_Cfg);

}



unsigned char EEPROM_Write_MBytes(unsigned int Memory_Address, unsigned char *bytes,uint8_t DataLength)
{
	uint8_t i=0;

	uint8_t buffer[256];

	buffer[0] = (uint8_t)(Memory_Address >> 8);			// Upper byte memory address
	buffer[1] = (uint8_t)(Memory_Address);				// Lower byte memory address

	for(i=2;i<(DataLength+2);i++)
	{
		buffer[i] = bytes[i-2];
	}
	MCAL_I2C_Master_Transmit(I2C1, EEPROM_Slave_Address, buffer, DataLength+2, With_Stop, Start);

	return 0;
}



unsigned char EEPROM_Read_Bytes(unsigned int Memory_Address, uint8_t *data_out,uint8_t DataLength)
{
	unsigned char buffer[2];
	buffer[0] = (uint8_t)(Memory_Address >> 8);			// Upper byte memory address
	buffer[1] = (uint8_t)(Memory_Address);				// Lower byte memory address


	// Write address only
	MCAL_I2C_Master_Transmit(I2C1, EEPROM_Slave_Address, buffer, 2, Without_Stop, Start);

	// Read Data
	MCAL_I2C_Master_Receive(I2C1, EEPROM_Slave_Address, data_out, DataLength, With_Stop, RepeatedStart);


	return 0;


}

