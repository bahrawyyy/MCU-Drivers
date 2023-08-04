/*
 * ADC.c
 *
 *  Created on: Sep 24, 2022
 *      Author: 10
 */

#include "ADC.h"

/*
 * 1)Configure a port as input for ADC --> Configuration
 * For ADCSRA
 * 2)SET The enable of ADC (ADEN)
 * 3)Choose the Prescalar 8.000.000 / 64 = 125.000 So, it is 64
 * 4)SET the ADPS1,ADPS2 (from data sheet)
 * 5)Choose the reference voltage, we choose the internal voltage 2.65v
 * 6)SET REFS1,REFS0 in ADMUX
 */

void ADC_init(void){
	//ADCSRA |= (1<<ADEN) | (1<<ADPS1) | (1<<ADPS2);
	//ADMUX |= (1<<REFS1) | (1<<REFS0);
#if	ADC_ENABLE == ENABLE
	SET_BIT(ADCSRA,ADEN);  // Enable the ADC

#if Prescalar_Type == Prescalar_2
	SET_BIT(ADCSRA,ADPS0);
#elif Prescalar_Type == Prescalar_4
	SET_BIT(ADCSRA,ADPS1);
#elif Prescalar_Type == Prescalar_8
	SET_BIT(ADCSRA,ADPS0);
	SET_BIT(ADCSRA,ADPS1);
#elif Prescalar_Type == Prescalar_16
	SET_BIT(ADCSRA,ADPS2);
#elif Prescalar_Type == Prescalar_32
	SET_BIT(ADCSRA,ADPS0);
	SET_BIT(ADCSRA,ADPS2);
#elif Prescalar_Type == Prescalar_64
	SET_BIT(ADCSRA,ADPS1); // Choose the Prescalar
	SET_BIT(ADCSRA,ADPS2);
#elif Prescalar_Type == Prescalar_128
	SET_BIT(ADCSRA,ADPS0);
	SET_BIT(ADCSRA,ADPS1);
	SET_BIT(ADCSRA,ADPS2);
#endif

#if ADC_SOURCE == ADC_SOURCE_VREF
	CLR_BIT(ADMUX,REFS0);
	CLR_BIT(ADMUX,REFS1);
#elif ADC_SOURCE == ADC_SOURCE_AVCC
	SET_BIT(ADMUX,REFS0);
	CLR_BIT(ADMUX,REFS1);
#elif ADC_SOURCE == ADC_SOURCE_RESERVED
	CLR_BIT(ADMUX,REFS0);
	SET_BIT(ADMUX,REFS1);
#elif ADC_SOURCE == ADC_SOURCE_INTERNAL
	SET_BIT(ADMUX,REFS0);
	SET_BIT(ADMUX,REFS1);
#endif

#endif
}

/* To read
 * 1) Select the channel to work on in ADMUX
 * 2) Start conversion by setting the ADSC bit
 * 3) Check the flag ADIF for conversion complete
 * 4) Clear the conversion bit by writing it 1
 * 5) Read the data from ADC
 */




/*
The ADMUX register is responsible for selecting the input channel to the ADC, and it has 8 bits in total,
where the lower 4 bits (bits 0-3) are used for selecting the channel and the upper 4 bits (bits 4-7) are used for selecting the reference voltage.

The code uses a bitwise AND operation to mask the upper 4 bits of the ADMUX register by applying a bit mask of 0xF0,
 which preserves the current value of the upper 4 bits and sets the lower 4 bits to 0. Then it performs a bitwise OR operation
  to set the lower 4 bits of the ADMUX register to the desired channel by applying a bit mask of 0x0F to the channel parameter.


 */



u16 ADC_read(u8 channel) {
	// Select ADC channel with safety mask
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	// Start single conversion
	ADCSRA |= (1 << ADSC);
	// Wait for conversion to complete
	while (!GET_BIT(ADCSRA,ADIF));
	CLR_BIT(ADCSRA,ADIF);

	u8 lowByte = ADCL;
	u8 highByte = ADCH;

	// Combine the two 8-bit registers to get the 10-bit ADC result
	u16 result = (highByte << 8) | lowByte;


	return result;
}
