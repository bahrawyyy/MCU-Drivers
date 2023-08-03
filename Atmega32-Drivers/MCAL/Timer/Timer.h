/*
 * Timer.h
 *
 *  Created on: Aug 3, 2023
 *      Author: Abdallah
 */

#ifndef MCAL_TIMER_TIMER_H_
#define MCAL_TIMER_TIMER_H_

#include "../Registers.h"
#include "../../LIB/ErrorState.h"
#include "../../LIB/Bit_Math.h"
#include "../../LIB/STD_Types.h"


/*************************************
 * 		Config Structure : TIMER 0
 ************************************/


// Timer 0 Modes
typedef enum
{
	TIMER0_NORMAL_MODE=0,
	TIMER0_PHASE_CORRECT_MODE,
	TIMER0_CTC_MODE,
	TIMER0_FAST_PWM_MODE
}TIMER0_MODE_t;


// Timer 0 prescalar
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


// OCO Pin Mode (HW Pin)
typedef enum{
	OC0_DISCONNECTED=0,
	OC0_TOGGLE,
	OC0_NON_INVERTING,     // Clear OC0 at Compare match
	OC0_INVERTING		   // Set OC0 at Compare match
}OC0_Mode_t;


typedef enum
{
	TOIE0_DISABLE,
	TOIE0_ENABLE = 0x01
}TOVFIE0_t;

typedef enum
{
	OCIE0_DISABLE,
	OCIE0_ENABLE = 0x02
}TOCMIE0_t;

typedef struct
{
	TIMER0_MODE_t mode;
	Timer0_Prescalar prescalar;
	OC0_Mode_t oc0_mode;
	TOVFIE0_t OVF_Interrupt;
	TOCMIE0_t OCM_Interrupt;
}TIMER0_Config_t;


/****************************
 * 		APIs
 **************************/

ES_t TIMER0_Init(TIMER0_Config_t *TIME0_Cfg);
ES_t TIMER0_Stop(void);

ES_t TIMER0_GetCompareValue(u8 *CM_Value);
ES_t TIMER0_SetCompareValue(u8 CM_Value);

ES_t TIMER0_GetCounterValue(u8 *TCNT_Value);
ES_t TIMER0_SetCounterValue(u8 TCNT_Value);

// In case of larger than 255
ES_t TIMER0_GetOVF_Flag(u8 *OVF_Value);

void Timer0_OVF_SetCallBack(void (*Local_ptr_toFunc));
void Timer0_OCM_SetCallBack(void (*Local_ptr_toFunc));


ES_t PWM_Set_DutyCycle(u8 dutyCycle);



#endif /* MCAL_TIMER_TIMER_H_ */
