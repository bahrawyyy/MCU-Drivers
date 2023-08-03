/*
 * DC_Motor.c
 *
 *  Created on: Aug 3, 2023
 *      Author: Abdallah
 */


#include "DC_Motor.h"
#include "DC_MotorCfg.h"



ES_t DC_Motor_Init(DC_MOTOR_Index Motor)
{
	if(Motor == DC_MOTOR_0){
		DIO_ES_tSetPinDirection(DC_MOTOR_REG, IN1, OUTPUT);
		DIO_ES_tSetPinDirection(DC_MOTOR_REG, IN2, OUTPUT);
	}else if(Motor == DC_MOTOR_1){
		DIO_ES_tSetPinDirection(DC_MOTOR_REG, IN3, OUTPUT);
		DIO_ES_tSetPinDirection(DC_MOTOR_REG, IN4, OUTPUT);
	}else{
		return ES_NOT_OK;
	}


	DIO_ES_tSetPinDirection(DIO_U8_PORT_B, DIO_U8_PIN_3, OUTPUT);
	TIMER0_Config_t Timer0Cfg = {TIMER0_PHASE_CORRECT_MODE, TIMER0_PRESCALAR_8, OC0_NON_INVERTING, TOIE0_DISABLE, OCIE0_DISABLE};
	TIMER0_Init(&Timer0Cfg);




	return ES_OK;
}


ES_t DC_Motor_Stop(DC_MOTOR_Index Motor)
{
	if(Motor == DC_MOTOR_0){
		DIO_ES_tSetPinValue(DC_MOTOR_REG, IN1, LOW);
		DIO_ES_tSetPinValue(DC_MOTOR_REG, IN2, LOW);
	}else if(Motor == DC_MOTOR_1){
		DIO_ES_tSetPinValue(DC_MOTOR_REG, IN3, LOW);
		DIO_ES_tSetPinValue(DC_MOTOR_REG, IN4, LOW);
	}else{
		return ES_NOT_OK;
	}
	return ES_OK;
}


ES_t DC_Motor_Move(DC_MOTOR_Index Motor,DC_MOTOR_Dir MotorDir,u8 SpeedOF100)
{

	switch(MotorDir){
	case DC_MOTOR_CW:
		if(Motor == DC_MOTOR_0){
			DIO_ES_tSetPinValue(DC_MOTOR_REG, IN1, HIGH);
			DIO_ES_tSetPinValue(DC_MOTOR_REG, IN2, LOW);
		}else if(Motor == DC_MOTOR_1){
			DIO_ES_tSetPinValue(DC_MOTOR_REG, IN3, HIGH);
			DIO_ES_tSetPinValue(DC_MOTOR_REG, IN4, LOW);
		}
		break;
	case DC_MOTOR_CCW:
		if(Motor == DC_MOTOR_0){
			DIO_ES_tSetPinValue(DC_MOTOR_REG, IN1, LOW);
			DIO_ES_tSetPinValue(DC_MOTOR_REG, IN2, HIGH);
		}else if(Motor == DC_MOTOR_1){
			DIO_ES_tSetPinValue(DC_MOTOR_REG, IN3, LOW);
			DIO_ES_tSetPinValue(DC_MOTOR_REG, IN4, HIGH);
		}
		break;
	default:
		return ES_NOT_OK;
	}


	PWM_Set_DutyCycle(SpeedOF100);




	return ES_OK;
}



