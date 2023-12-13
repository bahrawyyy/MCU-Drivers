/*
 * Stm32_F103C6_TIMER_driver.c
 *
 *  Created on: Sep 13, 2023
 *      Author: Abdallah
 */


#include "../inc/Stm32_F103C6_TIMER_driver.h"

/**================================================================
 * @Fn					- TIMER2_Init
 * @brief 				- This initialization mainly to provide delay using timer2
 * @param [in] 			- None
 * @retval 				- None
 * Note					- This isn't the full initialization for timer2
 *
 */


uint8_t PinStepper_flag = 0;


void TIMER2_Init(CLOCK_SPEED Clock_Speed_MHz)
{
	// Enable clock for timer2
	RCC_TIMER2_CLK_EN();

	// Choose a prescalar (8MHz/(7+1)=1MHz clock)
	//	TIMER2->PSC = 7;

	// If 36MHz clock used (36MHz/(35+1)=1MHz clock)
	//	TIMER2->PSC = 35;


	// Generic Prescalar (To get 1MHz clock speed for timer)
	TIMER2->PSC = (Clock_Speed_MHz - 1);


	// Choose a value for auto reload register (we didn't use the maximum value because we doesn't have long delays)
	// Timer 2 will generate an interrupt every 50 milliseconds
	// The auto-reload register (ARR) is used for the purpose of resetting the timer counter back to 0 when it reaches the value specified in ARR.
	// This process is often referred to as "wrapping" or "overflowing."
	TIMER2->ARR = 0xC350;  // 50000*1us = 0.05s

	// Enable Timer2
	TIMER2->CR1 |= (1<<0);


	//Wait until the update flag (UIF) in the status register (SR) of TIM2 becomes 1.
	// This indicates that the timer
	// has finished counting to its ARR value and generated an interrupt.
	while(!((TIMER2->SR) & (1<<0)));
}




void TIMER3_Init(CLOCK_SPEED Clock_Speed_MHz)
{
	// Enable clock for timer2
	RCC_TIMER3_CLK_EN();

	// Choose a prescalar (8MHz/(7+1)=1MHz clock)
	//	TIMER2->PSC = 7;

	// If 36MHz clock used (36MHz/(35+1)=1MHz clock)
	//	TIMER2->PSC = 35;


	// Generic Prescalar (To get 1MHz clock speed for timer)
	TIMER3->PSC = (Clock_Speed_MHz - 1);


	// Choose a value for auto reload register (we didn't use the maximum value because we doesn't have long delays)
	// Timer 2 will generate an interrupt every 50 milliseconds
	// The auto-reload register (ARR) is used for the purpose of resetting the timer counter back to 0 when it reaches the value specified in ARR.
	// This process is often referred to as "wrapping" or "overflowing."
	TIMER3->ARR = 0xC350;  // 50000*1us = 0.05s

	// Enable Timer2
	TIMER3->CR1 |= (1<<0);


	//Wait until the update flag (UIF) in the status register (SR) of TIM2 becomes 1.
	// This indicates that the timer
	// has finished counting to its ARR value and generated an interrupt.
	while(!((TIMER3->SR) & (1<<0)));
}


void Delay_Timer3_us(int us)
{
	TIMER3->CNT = 0;
	while((TIMER3->CNT) < us);
}


void Delay_Timer3_ms(int ms)
{
	int i;
	for(i=0;i<ms;i++)
	{
		Delay_Timer3_us(1000);
	}
}


/**================================================================
 * @Fn					- Delay_us
 * @brief 				- Provides delays in microseconds using timer2
 * @param [in] 			- us: Delay in microseconds
 * @retval 				- None
 * Note					- You must initialize timer2 before using this function
 *
 */

void Delay_us(int us)
{
	TIMER2->CNT = 0;
	while((TIMER2->CNT) < us);
}

/**================================================================
 * @Fn					- Delay_ms
 * @brief 				- Provides delays in milliseconds using timer2
 * @param [in] 			- ms: Delay in milliseconds
 * @retval 				- None
 * Note					- You must initialize timer2 before using this function
 *
 */

void Delay_ms(int ms)
{
	int i;
	for(i=0;i<ms;i++)
	{
		Delay_us(1000);
	}
}

// =============================================================================== //

/**
 * PWM Implemenation
 */

/**================================================================
 * @Fn					- PWM
 * @brief 				- Generate PWM signals using timer1, 2, 3, and 4
 * @param [in] 			- TIMERx: Which timer to work with
 * @param [in] 			- ChannelX: Which channel in this timer to work with
 * @param [in] 			- dutyCycle: Duty cycle for your signal as a value(meaning 50% is passed as 50)
 * @param [in] 			- frequency: Which frequency you want for your signal
 * @param [in] 			- clock_speed: Which clock_speed is provided for SYSCLK
 * @retval 				- None
 * Note					- TIMER1 is an advance control timer
 *
 */




void PWM(TIMER_Typedef *TIMERx, Timer_Channel ChannelX, uint8_t dutyCycle, uint32_t frequency, CLOCK_SPEED Clock_Speed_MHz)
{
	// Disabling timer to make configurations



	// Choose appropriate values for ARR and PSC
	uint8_t loop_flag = 1;
	uint32_t top_Value = 0, Prescalar_Value = 1;    // fCK_PSC / (PSC[15:0] + 1)

	while(loop_flag == 1)
	{
		top_Value = ((Clock_Speed_MHz*1000000) / (frequency * Prescalar_Value));
		if(top_Value >= 32000){   // Just an arbitrary threshold that could fit the ARR register
			Prescalar_Value += 2;
		}else{
			loop_flag = 0;
		}
	}


	// ===================== TIMER1 ===============================
	if(TIMERx == TIMER1)
	{
		RCC_TIMER1_CLK_EN();
		RCC_GPIOA_CLK_EN();
		RCC_AFIO_CLK_EN();

		TIMERx->CR1 &= ~(1<<0);

		switch(ChannelX){
		case TIMER_CH1:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_8,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOA, &GPIO_PinCfg);
			TIMERx->CCMR1 |= (1<<3);  // Output compare 1 preload enable
			TIMERx->CCER |= (1<<0);  // Capture/Compare 1 output enable
			TIMERx->CCMR1 |= (0b110 << 4);  // PWM mode 1 - In upcounting, channel 1 is active
			TIMERx->CCR1 = (top_Value * dutyCycle / 100);
			TIMERx->BDTR |=(1<<15); // Main output enable
			//TIMERx->DIER |= (0b11 << 0);  // Update interrupt enable && Capture/Compare 1 interrupt enable
			break;
		}

		case TIMER_CH2:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_9,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOA, &GPIO_PinCfg);
			TIMERx->CCMR1 |= (1<<11);  // Output compare 2 preload enable
			TIMERx->CCER |= (1<<4);   // Capture/Compare 2 output enable
			TIMERx->CCMR1 |= (0b110 << 12);
			TIMERx->CCR2 = (top_Value * dutyCycle / 100);
			TIMERx->BDTR |=(1<<15); // Main output enable
			//TIMERx->DIER |= (0b101 << 0);
			break;
		}

		case TIMER_CH3:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_10,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOA, &GPIO_PinCfg);
			TIMERx->CCMR2 |= (1<<3);  // Output compare 3 preload enable
			TIMERx->CCER |= (1<<8);   // Capture/Compare 2 output enable
			TIMERx->CCMR2 |= (0b110 << 4);
			TIMERx->CCR3 = (top_Value * dutyCycle / 100);
			TIMERx->BDTR |=(1<<15); // Main output enable
			//TIMERx->DIER |= (0b1001 << 0);
			break;
		}

		case TIMER_CH4:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_11,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOA, &GPIO_PinCfg);
			TIMERx->CCMR2 |= (1<<11);  // Output compare 4 preload enable
			TIMERx->CCER |= (1<<12);   // Capture/Compare 2 output enable
			TIMERx->CCMR2 |= (0b110 << 12);
			TIMERx->CCR4 = (top_Value * dutyCycle / 100);
			TIMERx->BDTR |=(1<<15); // Main output enable
			//TIMERx->DIER |= (0b10001 << 0);
			break;
		}
		}
	}

	// ===================== TIMER2 ===============================
	else if(TIMERx == TIMER2)
	{
		RCC_TIMER2_CLK_EN();
		RCC_GPIOA_CLK_EN();
		RCC_AFIO_CLK_EN();


		TIMERx->CR1 &= ~(1<<0);

		switch(ChannelX){
		case TIMER_CH1:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_0,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOA, &GPIO_PinCfg);
			TIMERx->CCMR1 |= (1<<3);  // Output compare 1 preload enable
			TIMERx->CCER |= (1<<0);  // Capture/Compare 1 output enable
			TIMERx->CCMR1 |= (0b110 << 4);  // PWM mode 1 - In upcounting, channel 1 is active
			TIMERx->CCR1 = (top_Value * dutyCycle / 100);
			//TIMERx->DIER |= (0b11 << 0);
			break;
		}

		case TIMER_CH2:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_1,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOA, &GPIO_PinCfg);
			TIMERx->CCMR1 |= (1<<11);  // Output compare 2 preload enable
			TIMERx->CCER |= (1<<4);   // Capture/Compare 2 output enable
			TIMERx->CCMR1 |= (0b110 << 12);
			TIMERx->CCR2 = (top_Value * dutyCycle / 100);
			//TIMERx->DIER |= (0b101 << 0);
			break;
		}

		case TIMER_CH3:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_2,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOA, &GPIO_PinCfg);
			TIMERx->CCMR2 |= (1<<3);  // Output compare 3 preload enable
			TIMERx->CCER |= (1<<8);   // Capture/Compare 2 output enable
			TIMERx->CCMR2 |= (0b110 << 4);
			TIMERx->CCR3 = (top_Value * dutyCycle / 100);
			//TIMERx->DIER |= (0b1001 << 0);
			break;
		}

		case TIMER_CH4:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_3,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOA, &GPIO_PinCfg);
			TIMERx->CCMR2 |= (1<<11);  // Output compare 4 preload enable
			TIMERx->CCER |= (1<<12);   // Capture/Compare 2 output enable
			TIMERx->CCMR2 |= (0b110 << 12);
			TIMERx->CCR4 = (top_Value * dutyCycle / 100);
			//TIMERx->DIER |= (0b10001 << 0);
			break;
		}
		}

	}

	// ===================== TIMER3 ===============================
	else if(TIMERx == TIMER3)
	{
		RCC_TIMER3_CLK_EN();
		RCC_GPIOA_CLK_EN();
		RCC_GPIOB_CLK_EN();
		RCC_AFIO_CLK_EN();



		TIMERx->CR1 &= ~(1<<0);


		switch(ChannelX){
		case TIMER_CH1:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_6,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOA, &GPIO_PinCfg);
			TIMERx->CCMR1 |= (1<<3);  // Output compare 1 preload enable
			TIMERx->CCER |= (1<<0);  // Capture/Compare 1 output enable
			TIMERx->CCMR1 |= (0b110 << 4);  // PWM mode 1 - In upcounting, channel 1 is active
			TIMERx->CCR1 = (top_Value * dutyCycle / 100);
			//TIMERx->DIER |= (0b11 << 0);
			break;
		}

		case TIMER_CH2:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_7,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOA, &GPIO_PinCfg);
			TIMERx->CCMR1 |= (1<<11);  // Output compare 2 preload enable
			TIMERx->CCER |= (1<<4);   // Capture/Compare 2 output enable
			TIMERx->CCMR1 |= (0b110 << 12);
			TIMERx->CCR2 = (top_Value * dutyCycle / 100);
			//TIMERx->DIER |= (0b101 << 0);
			break;
		}

		case TIMER_CH3:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_0,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOB, &GPIO_PinCfg);
			TIMERx->CCMR2 |= (1<<3);  // Output compare 3 preload enable
			TIMERx->CCER |= (1<<8);   // Capture/Compare 2 output enable
			TIMERx->CCMR2 |= (0b110 << 4);
			TIMERx->CCR3 = (top_Value * dutyCycle / 100);
			//TIMERx->DIER |= (0b1001 << 0);
			break;
		}

		case TIMER_CH4:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_1,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOB, &GPIO_PinCfg);
			TIMERx->CCMR2 |= (1<<11);  // Output compare 4 preload enable
			TIMERx->CCER |= (1<<12);   // Capture/Compare 2 output enable
			TIMERx->CCMR2 |= (0b110 << 12);
			TIMERx->CCR4 = (top_Value * dutyCycle / 100);
			//TIMERx->DIER |= (0b10001 << 0);
			break;
		}
		}
	}

	// ===================== TIMER4 ===============================
	else if(TIMERx == TIMER4)
	{
		RCC_TIMER4_CLK_EN();
		RCC_GPIOB_CLK_EN();
		RCC_AFIO_CLK_EN();



		TIMERx->CR1 &= ~(1<<0);


		switch(ChannelX){
		case TIMER_CH1:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_6,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOB, &GPIO_PinCfg);
			TIMERx->CCMR1 |= (1<<3);  // Output compare 1 preload enable
			TIMERx->CCER |= (1<<0);  // Capture/Compare 1 output enable
			TIMERx->CCMR1 |= (0b110 << 4);  // PWM mode 1 - In upcounting, channel 1 is active
			TIMERx->CCR1 = (top_Value * dutyCycle / 100);
			//TIMERx->DIER |= (0b11 << 0);
			break;
		}

		case TIMER_CH2:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_7,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOB, &GPIO_PinCfg);
			TIMERx->CCMR1 |= (1<<11);  // Output compare 2 preload enable
			TIMERx->CCER |= (1<<4);   // Capture/Compare 2 output enable
			TIMERx->CCMR1 |= (0b110 << 12);
			TIMERx->CCR2 = (top_Value * dutyCycle / 100);
			//TIMERx->DIER |= (0b101 << 0);
			break;
		}

		case TIMER_CH3:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_8,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOB, &GPIO_PinCfg);
			TIMERx->CCMR2 |= (1<<3);  // Output compare 3 preload enable
			TIMERx->CCER |= (1<<8);   // Capture/Compare 2 output enable
			TIMERx->CCMR2 |= (0b110 << 4);
			TIMERx->CCR3 = (top_Value * dutyCycle / 100);
			//TIMERx->DIER |= (0b1001 << 0);
			break;
		}

		case TIMER_CH4:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_9,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOB, &GPIO_PinCfg);
			TIMERx->CCMR2 |= (1<<11);  // Output compare 4 preload enable
			TIMERx->CCER |= (1<<12);   // Capture/Compare 2 output enable
			TIMERx->CCMR2 |= (0b110 << 12);
			TIMERx->CCR4 = (top_Value * dutyCycle / 100);
			//TIMERx->DIER |= (0b10001 << 0);
			break;
		}
		}
	}



	// Common configurations between all timers and channels


	//	Bit 7 ARPE: Auto-reload preload enable
	//	0: TIMx_ARR register is not buffered
	//	1: TIMx_ARR register is buffered
	TIMERx->CR1 |= (1<<7);


	//	110: PWM mode 1 - In upcounting, channel 1 is active as long as TIMx_CNT<TIMx_CCR1


	// ARR register value (Frequency value)
	TIMERx->ARR = top_Value;

	// Prescaler Value
	TIMERx->PSC = (Prescalar_Value - 1);


	//  Re-initialize the counter and generates an update of the registers (UG)
	TIMERx->EGR |= (1<<0);

	// Enable the timer
	TIMERx->CR1 |= (1<<0);
}


void Disable_Timer2()
{
	TIMER2->CR1 &= ~(1<<0);

}


uint32_t Get_TIMER2_Counter()
{
	return (TIMER2->CNT);
}

uint32_t Get_TIMER3_Counter()
{
	return (TIMER3->CNT);
}

void TIMER2_Stop()
{
	// Bit 0 CEN: Counter enable
	TIMER2->CR1 &= ~(1<<0);
}

void TIMER3_Stop()
{
	// Bit 0 CEN: Counter enable
	TIMER3->CR1 &= ~(1<<0);
}


void TIMER4_Stop()
{
	// Bit 0 CEN: Counter enable
	TIMER4->CR1 &= ~(1<<0);
}



uint8_t stepper_Flag = 0;
uint16_t stepper_Steps = 0;


/**================================================================
 * @Fn					- PWM_Move_Steps
 * @brief 				- Make the timer count no. of steps (mostly with stepper motors)
 * @param [in] 			- TIMERx: Which timer to work with
 * @param [in] 			- ChannelX: Which channel in this timer to work with
 * @param [in] 			- dutyCycle: Duty cycle for your signal as a value(meaning 50% is passed as 50)
 * @param [in] 			- frequency: Which frequency you want for your signal
 * @param [in] 			- clock_speed: Which clock_speed is provided for SYSCLK
 * @param [in] 			- direction: CW or CCW
 * @param [in] 			- steps: No. of steps to move with stepper motor
 * @retval 				- None
 * Note					- This function utilize the interrupts in the timer to count the number of steps to move using stepper
 * 						This function is mostly used with stepper motors
 *
 */




void PWM_Move_Steps(TIMER_Typedef *TIMERx, Timer_Channel ChannelX, uint16_t steps,uint8_t dutyCycle, uint32_t frequency,CLOCK_SPEED Clock_Speed_MHz)
{
	stepper_Flag = 1;
	stepper_Steps = steps;





	// Choose appropriate values for ARR and PSC
	uint8_t loop_flag = 1;
	uint32_t top_Value = 0, Prescalar_Value = 1;    // fCK_PSC / (PSC[15:0] + 1)

	while(loop_flag == 1)
	{
		top_Value = ((Clock_Speed_MHz*1000000) / (frequency * Prescalar_Value));
		if(top_Value >= 32000){   // Just an arbitrary threshold that could fit the ARR register
			Prescalar_Value += 2;
		}else{
			loop_flag = 0;
		}
	}


	// ===================== TIMER1 ===============================
	if(TIMERx == TIMER1)
	{

		RCC_TIMER1_CLK_EN();
		RCC_GPIOA_CLK_EN();
		RCC_AFIO_CLK_EN();


		NVIC_IRQ24_TIMER1_BRK_Enable;
		NVIC_IRQ25_TIMER1_UP_Enable;
		NVIC_IRQ26_TIMER1_TRG_COM_Enable;
		NVIC_IRQ27_TIMER1_CC_Enable;

		TIMERx->CR1 &= ~(1<<0);

		switch(ChannelX){
		case TIMER_CH1:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_8,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOA, &GPIO_PinCfg);
			TIMERx->CCMR1 |= (1<<3);  // Output compare 1 preload enable
			TIMERx->CCER |= (1<<0);  // Capture/Compare 1 output enable
			TIMERx->CCMR1 |= (0b110 << 4);  // PWM mode 1 - In upcounting, channel 1 is active
			TIMERx->CCR1 = (top_Value * dutyCycle / 100);
			TIMERx->BDTR |=(1<<15); // Main output enable
			TIMERx->DIER |= (0b11 << 0);  // Update interrupt enable && Capture/Compare 1 interrupt enable
			break;
		}

		case TIMER_CH2:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_9,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOA, &GPIO_PinCfg);
			TIMERx->CCMR1 |= (1<<11);  // Output compare 2 preload enable
			TIMERx->CCER |= (1<<4);   // Capture/Compare 2 output enable
			TIMERx->CCMR1 |= (0b110 << 12);
			TIMERx->CCR2 = (top_Value * dutyCycle / 100);
			TIMERx->BDTR |=(1<<15); // Main output enable
			TIMERx->DIER |= (0b101 << 0);
			break;
		}

		case TIMER_CH3:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_10,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOA, &GPIO_PinCfg);
			TIMERx->CCMR2 |= (1<<3);  // Output compare 3 preload enable
			TIMERx->CCER |= (1<<8);   // Capture/Compare 2 output enable
			TIMERx->CCMR2 |= (0b110 << 4);
			TIMERx->CCR3 = (top_Value * dutyCycle / 100);
			TIMERx->BDTR |=(1<<15); // Main output enable
			TIMERx->DIER |= (0b1001 << 0);
			break;
		}

		case TIMER_CH4:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_11,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOA, &GPIO_PinCfg);
			TIMERx->CCMR2 |= (1<<11);  // Output compare 4 preload enable
			TIMERx->CCER |= (1<<12);   // Capture/Compare 2 output enable
			TIMERx->CCMR2 |= (0b110 << 12);
			TIMERx->CCR4 = (top_Value * dutyCycle / 100);
			TIMERx->BDTR |=(1<<15); // Main output enable
			TIMERx->DIER |= (0b10001 << 0);
			break;
		}
		}
	}

	// ===================== TIMER2 ===============================
	else if(TIMERx == TIMER2)
	{


		RCC_TIMER2_CLK_EN();
		RCC_GPIOA_CLK_EN();
		RCC_AFIO_CLK_EN();


		NVIC_IRQ28_TIMER2_Enable;


		TIMERx->CR1 &= ~(1<<0);

		switch(ChannelX){
		case TIMER_CH1:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_0,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOA, &GPIO_PinCfg);
			TIMERx->CCMR1 |= (1<<3);  // Output compare 1 preload enable
			TIMERx->CCER |= (1<<0);  // Capture/Compare 1 output enable
			TIMERx->CCMR1 |= (0b110 << 4);  // PWM mode 1 - In upcounting, channel 1 is active
			TIMERx->CCR1 = (top_Value * dutyCycle / 100);
			TIMERx->DIER |= (0b11 << 0);
			break;
		}

		case TIMER_CH2:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_1,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOA, &GPIO_PinCfg);
			TIMERx->CCMR1 |= (1<<11);  // Output compare 2 preload enable
			TIMERx->CCER |= (1<<4);   // Capture/Compare 2 output enable
			TIMERx->CCMR1 |= (0b110 << 12);
			TIMERx->CCR2 = (top_Value * dutyCycle / 100);
			TIMERx->DIER |= (0b101 << 0);
			break;
		}

		case TIMER_CH3:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_2,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOA, &GPIO_PinCfg);
			TIMERx->CCMR2 |= (1<<3);  // Output compare 3 preload enable
			TIMERx->CCER |= (1<<8);   // Capture/Compare 2 output enable
			TIMERx->CCMR2 |= (0b110 << 4);
			TIMERx->CCR3 = (top_Value * dutyCycle / 100);
			TIMERx->DIER |= (0b1001 << 0);
			break;
		}

		case TIMER_CH4:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_3,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOA, &GPIO_PinCfg);
			TIMERx->CCMR2 |= (1<<11);  // Output compare 4 preload enable
			TIMERx->CCER |= (1<<12);   // Capture/Compare 2 output enable
			TIMERx->CCMR2 |= (0b110 << 12);
			TIMERx->CCR4 = (top_Value * dutyCycle / 100);
			TIMERx->DIER |= (0b10001 << 0);
			break;
		}
		}

	}

	// ===================== TIMER3 ===============================
	else if(TIMERx == TIMER3)
	{


		RCC_TIMER3_CLK_EN();
		RCC_GPIOA_CLK_EN();
		RCC_GPIOB_CLK_EN();
		RCC_AFIO_CLK_EN();

		NVIC_IRQ29_TIMER3_Enable;



		TIMERx->CR1 &= ~(1<<0);


		switch(ChannelX){
		case TIMER_CH1:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_6,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOA, &GPIO_PinCfg);
			TIMERx->CCMR1 |= (1<<3);  // Output compare 1 preload enable
			TIMERx->CCER |= (1<<0);  // Capture/Compare 1 output enable
			TIMERx->CCMR1 |= (0b110 << 4);  // PWM mode 1 - In upcounting, channel 1 is active
			TIMERx->CCR1 = (top_Value * dutyCycle / 100);
			TIMERx->DIER |= (0b11 << 0);
			break;
		}

		case TIMER_CH2:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_7,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOA, &GPIO_PinCfg);
			TIMERx->CCMR1 |= (1<<11);  // Output compare 2 preload enable
			TIMERx->CCER |= (1<<4);   // Capture/Compare 2 output enable
			TIMERx->CCMR1 |= (0b110 << 12);
			TIMERx->CCR2 = (top_Value * dutyCycle / 100);
			TIMERx->DIER |= (0b101 << 0);
			break;
		}

		case TIMER_CH3:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_0,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOB, &GPIO_PinCfg);
			TIMERx->CCMR2 |= (1<<3);  // Output compare 3 preload enable
			TIMERx->CCER |= (1<<8);   // Capture/Compare 2 output enable
			TIMERx->CCMR2 |= (0b110 << 4);
			TIMERx->CCR3 = (top_Value * dutyCycle / 100);
			TIMERx->DIER |= (0b1001 << 0);
			break;
		}

		case TIMER_CH4:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_1,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOB, &GPIO_PinCfg);
			TIMERx->CCMR2 |= (1<<11);  // Output compare 4 preload enable
			TIMERx->CCER |= (1<<12);   // Capture/Compare 2 output enable
			TIMERx->CCMR2 |= (0b110 << 12);
			TIMERx->CCR4 = (top_Value * dutyCycle / 100);
			TIMERx->DIER |= (0b10001 << 0);
			break;
		}
		}
	}

	// ===================== TIMER4 ===============================
	else if(TIMERx == TIMER4)
	{


		RCC_TIMER4_CLK_EN();
		RCC_GPIOB_CLK_EN();
		RCC_AFIO_CLK_EN();

		NVIC_IRQ30_TIMER4_Enable;



		TIMERx->CR1 &= ~(1<<0);


		switch(ChannelX){
		case TIMER_CH1:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_6,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOB, &GPIO_PinCfg);
			TIMERx->CCMR1 |= (1<<3);  // Output compare 1 preload enable
			TIMERx->CCER |= (1<<0);  // Capture/Compare 1 output enable
			TIMERx->CCMR1 |= (0b110 << 4);  // PWM mode 1 - In upcounting, channel 1 is active
			TIMERx->CCR1 = (top_Value * dutyCycle / 100);
			TIMERx->DIER |= (0b11 << 0);
			break;
		}

		case TIMER_CH2:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_7,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOB, &GPIO_PinCfg);
			TIMERx->CCMR1 |= (1<<11);  // Output compare 2 preload enable
			TIMERx->CCER |= (1<<4);   // Capture/Compare 2 output enable
			TIMERx->CCMR1 |= (0b110 << 12);
			TIMERx->CCR2 = (top_Value * dutyCycle / 100);
			TIMERx->DIER |= (0b101 << 0);
			break;
		}

		case TIMER_CH3:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_8,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOB, &GPIO_PinCfg);
			TIMERx->CCMR2 |= (1<<3);  // Output compare 3 preload enable
			TIMERx->CCER |= (1<<8);   // Capture/Compare 2 output enable
			TIMERx->CCMR2 |= (0b110 << 4);
			TIMERx->CCR3 = (top_Value * dutyCycle / 100);
			TIMERx->DIER |= (0b1001 << 0);
			break;
		}

		case TIMER_CH4:
		{
			GPIO_PinConfig_t GPIO_PinCfg = {GPIO_PIN_9,GPIO_MODE_AF_OUTPUT_PUSHPULL,GPIO_SPEED_10MHZ};
			MCAL_GPIO_Init(GPIOB, &GPIO_PinCfg);
			TIMERx->CCMR2 |= (1<<11);  // Output compare 4 preload enable
			TIMERx->CCER |= (1<<12);   // Capture/Compare 2 output enable
			TIMERx->CCMR2 |= (0b110 << 12);
			TIMERx->CCR4 = (top_Value * dutyCycle / 100);
			TIMERx->DIER |= (0b10001 << 0);
			break;
		}
		}
	}



	// Common configurations between all timers and channels


	//	Bit 7 ARPE: Auto-reload preload enable
	//	0: TIMx_ARR register is not buffered
	//	1: TIMx_ARR register is buffered
	TIMERx->CR1 |= (1<<7);


	//	110: PWM mode 1 - In upcounting, channel 1 is active as long as TIMx_CNT<TIMx_CCR1


	// ARR register value (Frequency value)
	TIMERx->ARR = top_Value;

	// Prescaler Value
	TIMERx->PSC = (Prescalar_Value - 1);


	TIMERx->DIER |=(0b1<<0);


	//  Re-initialize the counter and generates an update of the registers (UG)
	TIMERx->EGR |= (1<<0);

	// Enable the timer
	TIMERx->CR1 |= (1<<0);





}




void TIM2_IRQHandler()
{
	if(stepper_Flag == 1)
	{
		TIMER2->SR &= ~(1<<0);
		TIMER2->SR &= ~(1<<1);  // Capture/compare 1 interrupt flag

		if(stepper_Steps != 0){
			stepper_Steps--;
		}else{
			stepper_Flag = 0;
			TIMER2->CR1 &= ~(1<<0);   // Disable timer
			PinStepper_flag = 1;
		}
	}else{   // For further usage rather than stepper

	}
}








