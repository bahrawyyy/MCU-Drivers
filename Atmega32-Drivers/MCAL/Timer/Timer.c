/*
 * Timer.c
 *
 *  Created on: May 6, 2023
 *      Author: Abdallah
 */


#include "Timer.h"

#define NULLPTR   ((void*)0)

static void (*Timer0_OVF_Fptr)(void) = NULLPTR;

void Timer0_Init(Timer0Mode_type Mode, Timer0_Prescalar Prescalar,OC0Mode_type Oc0_Mode)
{
	switch(Mode)
	{
	case TIMER0_NORMAL_MODE:
		CLR_BIT(TCCR0,WGM00);
		CLR_BIT(TCCR0,WGM01);
		break;
	case TIMER0_PHASE_CORRECT_MODE:
		SET_BIT(TCCR0,WGM00);
		CLR_BIT(TCCR0,WGM01);
		break;
	case TIMER0_CTC_MODE:
		CLR_BIT(TCCR0,WGM00);
		SET_BIT(TCCR0,WGM01);
		break;
	case TIMER0_FAST_PWM_MODE:
		SET_BIT(TCCR0,WGM00);
		SET_BIT(TCCR0,WGM01);
		break;
	}

	// To make sure the first three pins are zero
	// in case of old data written
	TCCR0 = TCCR0 & 0xf8;   // 11111000
	TCCR0 = TCCR0 | Prescalar;


	// Hardware pin from OCU (Output compare unit) to output something
	// when there is matching between OCR and TCNT

	switch(Oc0_Mode)
	{
	case OC0_DISCONNECTED:
		CLR_BIT(TCCR0,COM00);
		CLR_BIT(TCCR0,COM01);
		break;
	case OC0_TOGGLE:
		SET_BIT(TCCR0,COM00);
		CLR_BIT(TCCR0,COM01);
		break;
	case OC0_NON_INVERTING:
		CLR_BIT(TCCR0,COM00);
		SET_BIT(TCCR0,COM01);
		break;
	case OC0_INVERTING:
		SET_BIT(TCCR0,COM00);
		SET_BIT(TCCR0,COM01);
		break;
	}


}

// Separately, in case you want to run the timer
// without interrupt
void Timer0_OVInterrupt_Enable(void)
{
	SET_BIT(TIMSK,TOIE0);
}
void Timer0_OVInterrupt_Disable(void)
{
	CLR_BIT(TIMSK,TOIE0);
}

void Timer0_OVF_SetCallBack(void (*Local_ptr_toFunc))
{
	Timer0_OVF_Fptr = Local_ptr_toFunc;
}


// used in delay
ISR(TIMER0_OVF_vect)
{
	if(Timer0_OVF_Fptr != NULLPTR)
	{
		Timer0_OVF_Fptr();
	}
}






