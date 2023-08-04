/*
 * ADC.h
 *
 *  Created on: Sep 24, 2022
 *      Author: 10
 */

#ifndef MCAL_ADC_ADC_H_
#define MCAL_ADC_ADC_H_

#include "../../LIB/STD_Types.h"
#include "../../LIB/Bit_Math.h"
#include "../../MCAL/Registers.h"

#include "ADCconfig.h"

void ADC_init(void);
u16 ADC_read(u8 channel);

#endif /* MCAL_ADC_ADC_H_ */
