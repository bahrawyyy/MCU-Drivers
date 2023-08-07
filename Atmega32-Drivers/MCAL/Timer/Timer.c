/*
 * Timer.c
 *
 *  Created on: Aug 3, 2023
 *      Author: Abdallah
 */


#include "Timer.h"

#define F_CPU 16000000UL


#define NULLPTR   ((void*)0)

static void (*Timer0_OVF_Fptr)(void) = NULLPTR;
static void (*Timer0_OCM_Fptr)(void) = NULLPTR;



TIMER0_Config_t Global_Timer0_Cfg;

TIMER1_Config_t Global_Timer1_Cfg;


volatile u8 OVF_Number = 0;
volatile u16 OVF_Num_Delay;
volatile u8 Global_Delay_Status=0;



ES_t TIMER0_Init(TIMER0_Config_t *TIME0_Cfg)
{
	ES_t RetValue = ES_NOT_OK;
	if(TIME0_Cfg == NULLPTR){
		RetValue = ES_NOT_OK;
	}else
	{
		// Copy the configuration in a global structure in case of
		// usage inside othe functions
		Global_Timer0_Cfg = *TIME0_Cfg;


		// Timer0 mode Configurations
		if((TIME0_Cfg->mode >= TIMER0_NORMAL_MODE) && (TIME0_Cfg->mode <= TIMER0_FAST_PWM_MODE))
		{
			RetValue = ES_OK;
			switch(TIME0_Cfg->mode)
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
		}else{
			RetValue = ES_NOT_OK;
		}

		// Timer_0 Prescalar Configuration
		if((TIME0_Cfg->prescalar >= TIMER0_STOP) && (TIME0_Cfg->prescalar <= EXTERNAL_RISING))
		{
			TCCR0 |= TIME0_Cfg->prescalar;
		}else{
			RetValue = ES_NOT_OK;
		}


		// Timer_0 OC0 Pin
		TCCR0 |= (TIME0_Cfg->oc0_mode << COM00);


		// Timer_0 Interrupts
		TIMSK |= (TIME0_Cfg->OVF_Interrupt) | (TIME0_Cfg->OCM_Interrupt);



	}

	return RetValue;

}
ES_t TIMER0_Stop(void)
{
	// No clock source
	CLR_BIT(TCCR0,CS00);
	CLR_BIT(TCCR0,CS01);
	CLR_BIT(TCCR0,CS02);
	return ES_OK;
}

ES_t TIMER0_GetCompareValue(u8 *CM_Value)
{
	*CM_Value = OCR0;

	return ES_OK;

}
ES_t TIMER0_SetCompareValue(u8 CM_Value)
{
	OCR0 = CM_Value;

	return ES_OK;

}

ES_t TIMER0_GetCounterValue(u8 *TCNT_Value)
{

	*TCNT_Value = TCNT0;


	return ES_OK;
}


ES_t TIMER0_SetCounterValue(u8 TCNT_Value)
{
	TCNT0 = TCNT_Value;

	return ES_OK;

}

// In case of larger than 255
ES_t TIMER0_GetOVF_Flag(u8 *OVF_Value)
{

	*OVF_Value = OVF_Number;
	return ES_OK;

}



ES_t PWM_Set_DutyCycle(float dutyCycle)
{
	ES_t RetValue;

	if((Global_Timer0_Cfg.mode == TIMER0_FAST_PWM_MODE) || (Global_Timer0_Cfg.mode == TIMER0_PHASE_CORRECT_MODE))
	{
		if(Global_Timer0_Cfg.oc0_mode == OC0_NON_INVERTING)
		{
			OCR0 = (u8)((dutyCycle/100.0)*256) - 1;

		}else if(Global_Timer0_Cfg.oc0_mode == OC0_INVERTING)
		{
			OCR0 = (u8)(256-((256*dutyCycle)/100.0)) - 1;
		}
		RetValue = ES_OK;
	}else{
		RetValue = ES_NOT_OK;
	}
	return RetValue;
}



void Timer0_Set_Delay_ms(u32 delay_ms)
{
	u32 CountsNum = (delay_ms * F_CPU) / (1000UL * Global_Timer0_Cfg.prescalar);
	OVF_Num_Delay = CountsNum / 256;
	u8 RestCounts = CountsNum % 256;

	TIMER0_SetCounterValue(256 - RestCounts);

	Global_Delay_Status = 0; // Reset the delay status
	while (!Global_Delay_Status);
}


void Timer0_OVF_SetCallBack(void (*Local_ptr_toFunc))
{
	Timer0_OVF_Fptr = Local_ptr_toFunc;


}
void Timer0_OCM_SetCallBack(void (*Local_ptr_toFunc))
{
	Timer0_OCM_Fptr = Local_ptr_toFunc;
}


// ===========================================================


ES_t TIMER1_Init(TIMER1_Config_t *TIME1_Cfg)
{
	ES_t RetValue = ES_NOT_OK;
	if(TIME1_Cfg == NULLPTR){
		RetValue = ES_NOT_OK;
	}else
	{
		// Copy the configuration in a global structure in case of
		// usage inside othe functions
		Global_Timer1_Cfg = *TIME1_Cfg;


		// Timer0 mode Configurations
		if((TIME1_Cfg->mode >= TIMER1_NORMAL_MODE) && (TIME1_Cfg->mode <= TIMER1_FAST_PWM_OCR1A_TOP))
		{
			RetValue = ES_OK;
			switch(TIME1_Cfg->mode)
			{
			case TIMER1_NORMAL_MODE:
				CLR_BIT(TCCR1A,WGM10);
				CLR_BIT(TCCR1A,WGM11);
				CLR_BIT(TCCR1B,WGM12);
				CLR_BIT(TCCR1B,WGM13);
				break;
			case TIMER1_FAST_PWM_ICR1_TOP:
				CLR_BIT(TCCR1A,WGM10);
				SET_BIT(TCCR1A,WGM11);
				SET_BIT(TCCR1B,WGM12);
				SET_BIT(TCCR1B,WGM13);
				break;
			case TIMER1_FAST_PWM_OCR1A_TOP:
				SET_BIT(TCCR1A,WGM10);
				SET_BIT(TCCR1A,WGM11);
				SET_BIT(TCCR1B,WGM12);
				SET_BIT(TCCR1B,WGM13);
				break;
			}
		}else{
			RetValue = ES_NOT_OK;
		}

		// Timer_0 Prescalar Configuration
		if((TIME1_Cfg->prescalar >= TIMER1_STOP) && (TIME1_Cfg->prescalar <= EXTERNAL1_RISING))
		{
			TCCR1B |= TIME1_Cfg->prescalar;
		}else{
			RetValue = ES_NOT_OK;
		}


		// Timer_1 OCA Pin
		TCCR1A |= (TIME1_Cfg->OC1A_mode << COM1A0);

		// Timer_1 OCA Pin
		TCCR1A |= (TIME1_Cfg->OC1B_mode << COM1B0);


		// Timer_1 Interrupts
		TIMSK |= (TIME1_Cfg->OVF_Interrupt) | (TIME1_Cfg->ICU_Interrupt) | (TIME1_Cfg->OCA_Interrupt) | (TIME1_Cfg->OCB_Interrupt);



	}

	return RetValue;
}


ES_t TIMER1_Stop(void)
{
	// No clock source
	CLR_BIT(TCCR1B,CS10);
	CLR_BIT(TCCR1B,CS11);
	CLR_BIT(TCCR1B,CS12);
	return ES_OK;
}





ISR(TIMER0_OVF_vect)
{
	if (Timer0_OVF_Fptr != NULLPTR)
	{
		OVF_Number++;
		Timer0_OVF_Fptr();
	}
}


ISR(TIMER0_COMP_vect)
{
	if(Timer0_OCM_Fptr != NULLPTR)
	{
		Timer0_OCM_Fptr();
	}
}
