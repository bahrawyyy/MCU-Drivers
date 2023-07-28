/*
 * Timer.h
 *
 *  Created on: May 6, 2023
 *      Author: Abdallah
 */

#ifndef MCAL_TIMER_TIMER_H_
#define MCAL_TIMER_TIMER_H_

#include "../Registers.h"
#include "../../LIB/STD_Types.h"
#include "../../LIB/Bit_Math.h"

// enum for modes

typedef enum{
	TIMER0_NORMAL_MODE=0,
	TIMER0_PHASE_CORRECT_MODE,
	TIMER0_CTC_MODE,
	TIMER0_FAST_PWM_MODE
}Timer0Mode_type;

// enum for prescalar

typedef enum{
	TIMER0_STOP=0,
	TIMER0_PRESCALAR_1,
	TIMER0_PRESCALAR_8,
	TIMER0_PRESCALAR_64,
	TIMER0_PRESCALAR_256,
	TIMER0_PRESCALAR_1024,
	EXTERNAL_FALLING,
	EXTERNAL_RISING
}Timer0_Prescalar;

// External falling/rising in case of external
// oscillator T1,T2 for timer only not from CPU


// Configurations of pin OC0

typedef enum{
	OC0_DISCONNECTED=0,
	OC0_TOGGLE,
	OC0_NON_INVERTING,
	OC0_INVERTING
}OC0Mode_type;


// Prototypes
void Timer0_Init(Timer0Mode_type Mode, Timer0_Prescalar Prescalar,OC0Mode_type Oc0_Mode);
void Timer0_OVInterrupt_Enable(void);
void Timer0_OVInterrupt_Disable(void);
void Timer0_OVF_SetCallBack(void (*Local_ptr_toFunc));

#endif /* MCAL_TIMER_TIMER_H_ */
