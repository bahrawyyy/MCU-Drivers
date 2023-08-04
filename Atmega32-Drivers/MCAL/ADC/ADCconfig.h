/*
 * ADCconfig.h
 *
 *  Created on: Sep 26, 2022
 *      Author: 10
 */

#ifndef MCAL_ADC_ADCCONFIG_H_
#define MCAL_ADC_ADCCONFIG_H_


#define	DISABLE 				0
#define	ENABLE 					1

#define ADC_ENABLE 				ENABLE

#define Prescalar_2 			0
#define Prescalar_4 			1
#define Prescalar_8 			2
#define Prescalar_16  			3
#define Prescalar_32    		4
#define Prescalar_64    		5
#define Prescalar_128   		6

#define Prescalar_Type 			Prescalar_128

#define ADC_SOURCE_INTERNAL 	0
#define ADC_SOURCE_AVCC 		1
#define ADC_SOURCE_RESERVED 	2
#define ADC_SOURCE_VREF 		3

#define ADC_SOURCE 				ADC_SOURCE_AVCC


#define ADC0 					0
#define ADC1 					1
#define ADC2 					2
#define ADC3 					3
#define ADC4 					4
#define ADC5 					5
#define ADC6 					6
#define ADC7 					7
//
#define ADC_Channel ADC3


#endif /* MCAL_ADC_ADCCONFIG_H_ */
