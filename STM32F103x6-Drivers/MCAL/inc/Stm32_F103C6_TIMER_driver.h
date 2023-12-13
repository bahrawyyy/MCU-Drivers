/*
 * Stm32_F103C6_TIMER_driver.h
 *
 *  Created on: Sep 13, 2023
 *      Author: Abdallah
 */

#ifndef STM32_F103C6_DRIVERS_INC_STM32_F103C6_TIMER_DRIVER_H_
#define STM32_F103C6_DRIVERS_INC_STM32_F103C6_TIMER_DRIVER_H_

#include "stm32f103c6.h"
#include "Stm32_F103C6_gpio_driver.h"
#include "Stm32_F103C6_RCC_driver.h"


typedef enum{
	TIMER_CH1,
	TIMER_CH2,
	TIMER_CH3,
	TIMER_CH4
}Timer_Channel;



extern uint8_t PinStepper_flag;


//#define Stepper_Port	GPIOA
//
//#define Step_Pin	GPIO_PIN_8
//#define Dir_Pin		GPIO_PIN_9




//	Timer1 CH1 --> A8  @@ Timer2 CH1 --> A0 @@ Timer3 CH1 --> A6 @@ Timer4 CH1 --> B6
//	Timer1 CH2 --> A9  @@ Timer2 CH2 --> A1 @@ Timer3 CH2 --> A7 @@ Timer4 CH2 --> B7
//	Timer1 CH3 --> A10 @@ Timer2 CH3 --> A2 @@ Timer3 CH3 --> B0 @@ Timer4 CH3 --> B8
//	Timer1 CH4 --> A11 @@ Timer2 CH4 --> A3 @@ Timer3 CH4 --> B1 @@ Timer4 CH4 --> B9

void TIMER3_Init(CLOCK_SPEED Clock_Speed_MHz);


void Delay_Timer3_ms(int ms);

void Delay_Timer3_us(int us);



void TIMER2_Init(CLOCK_SPEED Clock_Speed_MHz);

uint32_t Get_TIMER2_Counter(void);
void TIMER2_Stop();

void Delay_us(int us);

void Delay_ms(int ms);

void PWM(TIMER_Typedef *TIMERx, Timer_Channel ChannelX, uint8_t dutyCycle, uint32_t frequency, CLOCK_SPEED Clock_Speed_MHz);

void PWM_Move_Steps(TIMER_Typedef *TIMERx, Timer_Channel ChannelX, uint16_t steps,uint8_t dutyCycle, uint32_t frequency,CLOCK_SPEED Clock_Speed_MHz);

uint32_t Get_TIMER3_Counter();

void Disable_Timer2();

void TIMER3_Stop(void);

void TIMER4_Stop(void);



#endif /* STM32_F103C6_DRIVERS_INC_STM32_F103C6_TIMER_DRIVER_H_ */
