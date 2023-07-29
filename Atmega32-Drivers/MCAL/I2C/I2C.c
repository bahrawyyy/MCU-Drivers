/*
 * I2C.c
 *
 *  Created on: Jul 29, 2023
 *      Author: Abdallah
 */


#include "I2C.h"



void I2C_Init(unsigned long SCL_Clock)
{
	TWBR = (unsigned char)(((F_CPU/SCL_Clock)-16)/(2*I2C_PRESCALAR));

	switch(I2C_PRESCALAR)
	{
	case I2C_PRESCALAR_1:
		CLR_BIT(TWSR,TWPS0);
		CLR_BIT(TWSR,TWPS1);
		break;
	case I2C_PRESCALAR_4:
		SET_BIT(TWSR,TWPS0);
		CLR_BIT(TWSR,TWPS1);
		break;
	case I2C_PRESCALAR_16:
		CLR_BIT(TWSR,TWPS0);
		SET_BIT(TWSR,TWPS1);
		break;
	case I2C_PRESCALAR_64:
		SET_BIT(TWSR,TWPS0);
		SET_BIT(TWSR,TWPS1);
		break;
	}
}




void I2C_Start(void)
{
	// Send start condition
	// Writing a one to TWINT clears the Flag.
	// The TWI will not start any operation as long as the
	// TWINT bit in TWCR is set
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while(!GET_BIT(TWCR,TWINT));	// wait for transmission of START BIT
	while(TWSR != TWSR_START_TRANSMITTED);	// A START condition has been transmitted
}



void I2C_Write(u8 cByte)
{
	TWDR = cByte;
	// Clear TWINT bit in TWCR to start transmission of (address/data)
	TWCR = (1<<TWINT) | (1<<TWEN);
	//	Wait for TWINT Flag set. This indicates
	//	that the SLA+W has been transmitted,
	//	and ACK/NACK has been received.
	while(!GET_BIT(TWCR,TWINT));
}



void I2C_Stop(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}



void I2C_Set_Address(u8 slaveAddress)
{
	//	The TWAR should be loaded with the 7-bit slave address (in the seven most significant
	//	bits of TWAR)
	TWAR = (slaveAddress << 1);
}





// Read address and continue to read data byte
u8 I2C_Slave_Read(u8 *status)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while(!GET_BIT(TWCR,TWINT));	// After its own slave address and the write bit have been received, the TWINT
	// Flag is set and a valid status code can be read from TWSR

	while (TWSR != TWSR_Add_ACK_RETURNED) {
		// Handle error, expected SLA+R ACK not received
		*status = TWSR_Add_ACK_RETURNED;
	}
	// Continue Listening
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while(!GET_BIT(TWCR,TWINT));
	// Data has been received ACK has been returned
	while(TWSR != TWSR_Data_ACK_RETURNED){
		*status = TWSR_Data_ACK_RETURNED;
	}

	return TWDR;
}


// Just read a byte
u8 I2C_Read(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while(!GET_BIT(TWCR,TWINT));	// After its own slave address and the write bit have been received, the TWINT
	return TWDR;
}
