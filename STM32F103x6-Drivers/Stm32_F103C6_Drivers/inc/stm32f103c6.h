/*
 * stm32f103c6.h
 *
 *  Created on: Jul 9, 2023
 *      Author: Abdallah
 */

#ifndef INC_STM32F103C6_H_
#define INC_STM32F103C6_H_


//-----------------------------
//Includes
//-----------------------------

#include "stdlib.h"
#include <stdint.h>


//-----------------------------
//Base addresses for Memories
//-----------------------------


// UL {unsigned long} --> As it is constant value
#define FLASH_Memory_BASE						0x08000000UL
#define System_Memory_BASE						0x1FFFF000UL
#define SRAM_Memory_BASE						0x20000000UL


#define Peripherals_BASE						0x40000000UL

#define Cortex_M3_Internal_Peripherals_BASE		0xE0000000UL

#define NVIC_BASE								(0xE000E100UL)
#define NVIC_ISER0								*(volatile uint32_t*)(NVIC_BASE + 0x0)
#define NVIC_ISER1								*(volatile uint32_t*)(NVIC_BASE + 0x4)
#define NVIC_ISER2								*(volatile uint32_t*)(NVIC_BASE + 0x8)
#define NVIC_ICER0								*(volatile uint32_t*)(NVIC_BASE + 0x80)
#define NVIC_ICER1								*(volatile uint32_t*)(NVIC_BASE + 0x84)
#define NVIC_ICER2								*(volatile uint32_t*)(NVIC_BASE + 0x88)


//-----------------------------
//Base addresses for AHB Peripherals
//-----------------------------


// RCC
#define RCC_BASE								0x40021000UL



//-----------------------------
//Base addresses for ABP2 Peripherals
//-----------------------------



// GPIO
// A,B fully included in LQFP48 Package
#define GPIOA_BASE								0x40010800UL
#define GPIOB_BASE								0x40010C00UL




// C,D partially included in LQFP48 Package
#define GPIOC_BASE								0x40011000UL
#define GPIOD_BASE								0x40011400UL



// E is not included in LQFP48 Package
#define GPIOE_BASE								0x40011800UL




// EXTI (External interrupt)
#define EXTI_BASE								0x40010400UL



// AFIO (Alternative function input output)
#define AFIO_BASE								0x40010000UL




#define USART1_BASE								0x40013800UL


#define SPI1_BASE								0x40013000UL



//-----------------------------
//Base addresses for ABP1 Peripherals
//-----------------------------

#define USART2_BASE								0x40004400UL


#define USART3_BASE								0x40004800UL


#define SPI2_BASE								0x40003800UL


//===================================================================


//-*-*-*-*-*-*-*-*-*-*-*-
//Peripheral registers
//-*-*-*-*-*-*-*-*-*-*-*



//-*-*-*-*-*-*-*-*-*-*-*-
//Peripheral register: GPIO
//-*-*-*-*-*-*-*-*-*-*-*

typedef struct
{
	volatile uint32_t CRL;
	volatile uint32_t CRH;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t BRR;
	volatile uint32_t LCKR;
}GPIO_TypeDef;

//-*-*-*-*-*-*-*-*-*-*-*-
//Peripheral register: RCC
//-*-*-*-*-*-*-*-*-*-*-*

typedef struct
{
	volatile uint32_t CR;
	volatile uint32_t CFGR;
	volatile uint32_t CIR;
	volatile uint32_t APB2RSTR;
	volatile uint32_t APB1RSTR;
	volatile uint32_t AHBENR;
	volatile uint32_t APB2ENR;
	volatile uint32_t APB1ENR;
	volatile uint32_t BDCR;
	volatile uint32_t CSR;
}RCC_TypeDef;


//-*-*-*-*-*-*-*-*-*-*-*-
//Peripheral register: EXTI
//-*-*-*-*-*-*-*-*-*-*-*


typedef struct
{
	volatile uint32_t IMR;
	volatile uint32_t EMR;
	volatile uint32_t RTSR;
	volatile uint32_t FTSR;
	volatile uint32_t SWIER;
	volatile uint32_t PR;
}EXTI_Typedef;

//-*-*-*-*-*-*-*-*-*-*-*-
//Peripheral register: AFIO
//-*-*-*-*-*-*-*-*-*-*-*


typedef struct
{
	volatile uint32_t EVCR;
	volatile uint32_t MAPR;
	volatile uint32_t EXTICR[4];
	uint32_t 		RESERVED0;
	volatile uint32_t MAPR2;
}AFIO_Typedef;



//-*-*-*-*-*-*-*-*-*-*-*-
//Peripheral register: USART
//-*-*-*-*-*-*-*-*-*-*-*

typedef struct
{
	volatile uint32_t SR;
	volatile uint32_t DR;
	volatile uint32_t BRR;
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t CR3;
	volatile uint32_t GTPR;
}USART_Typedef;



//-*-*-*-*-*-*-*-*-*-*-*-
//Peripheral register: SPI
//-*-*-*-*-*-*-*-*-*-*-*

typedef struct
{
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t SR;
	volatile uint32_t DR;
	volatile uint32_t CRCPR;
	volatile uint32_t RXCRCR;
	volatile uint32_t TXCRCR;
	volatile uint32_t I2SCFGR;
	volatile uint32_t I2SPR;
}SPI_Typedef;

//===================================================================

//-*-*-*-*-*-*-*-*-*-*-*-
//Peripheral Instants
//-*-*-*-*-*-*-*-*-*-*-*

#define GPIOA							((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB							((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC							((GPIO_TypeDef *)GPIOC_BASE)
#define GPIOD							((GPIO_TypeDef *)GPIOD_BASE)
#define GPIOE							((GPIO_TypeDef *)GPIOE_BASE)

#define RCC								((RCC_TypeDef *)RCC_BASE)

#define EXTI							((EXTI_Typedef *)EXTI_BASE)

#define AFIO							((AFIO_Typedef *)AFIO_BASE)


#define USART1							((USART_Typedef *)USART1_BASE)
#define USART2							((USART_Typedef *)USART2_BASE)
#define USART3							((USART_Typedef *)USART3_BASE)

#define SPI1							((SPI_Typedef *)SPI1_BASE)
#define SPI2							((SPI_Typedef *)SPI2_BASE)


//===================================================================


//-*-*-*-*-*-*-*-*-*-*-*-
//clock enable Macros:
//-*-*-*-*-*-*-*-*-*-*-*

#define RCC_GPIOA_CLK_EN()             (RCC->APB2ENR |= 1<<2)
#define RCC_GPIOB_CLK_EN()             (RCC->APB2ENR |= 1<<3)
#define RCC_GPIOC_CLK_EN()             (RCC->APB2ENR |= 1<<4)
#define RCC_GPIOD_CLK_EN()             (RCC->APB2ENR |= 1<<5)
#define RCC_GPIOE_CLK_EN()             (RCC->APB2ENR |= 1<<6)

#define RCC_AFIO_CLK_EN()              (RCC->APB2ENR |= 1<<0)

// Clock enable for USART
#define RCC_USART1_CLK_EN()              (RCC->APB2ENR |= 1<<14)
#define RCC_USART2_CLK_EN()              (RCC->APB1ENR |= 1<<17)
#define RCC_USART3_CLK_EN()              (RCC->APB1ENR |= 1<<18)

// Clock enable for SPI
#define RCC_SPI1_CLK_EN()              (RCC->APB2ENR |= 1<<12)
#define RCC_SPI2_CLK_EN()              (RCC->APB1ENR |= 1<<14)



// RCC Reset Mechanism for USART
#define RCC_USART1_Reset()              (RCC->APB2RSTR |= 1<<14)
#define RCC_USART2_Reset()              (RCC->APB1RSTR |= 1<<17)
#define RCC_USART3_Reset()              (RCC->APB1RSTR |= 1<<18)


// RCC Reset Mechanism for SPI
#define RCC_SPI1_Reset()              (RCC->APB2RSTR |= 1<<12)
#define RCC_SPI2_Reset()              (RCC->APB1RSTR |= 1<<14)


// ==================================================================

//-*-*-*-*-*-*-*-*-*-*-*-
//IVT (Interrupt vector table)
//-*-*-*-*-*-*-*-*-*-*-*


#define EXTI0_IRQ			6
#define EXTI1_IRQ			7
#define EXTI2_IRQ			8
#define EXTI3_IRQ			9
#define EXTI4_IRQ			10
#define EXTI5_IRQ			23
#define EXTI6_IRQ			23
#define EXTI7_IRQ			23
#define EXTI8_IRQ			23
#define EXTI9_IRQ			23
#define EXTI10_IRQ			40
#define EXTI11_IRQ			40
#define EXTI12_IRQ			40
#define EXTI13_IRQ			40
#define EXTI14_IRQ			40
#define EXTI15_IRQ			40

#define USART1_IRQ			37
#define USART2_IRQ			38
#define USART3_IRQ			39

#define SPI1_IRQ			35
#define SPI2_IRQ			36


//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
// NVIC: IRQ enable/disable macros
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#define NVIC_IRQ6_EXTI0_Enable			(NVIC_ISER0 |= (1<<6))
#define NVIC_IRQ7_EXTI1_Enable			(NVIC_ISER0 |= (1<<7))
#define NVIC_IRQ8_EXTI2_Enable			(NVIC_ISER0 |= (1<<8))
#define NVIC_IRQ9_EXTI3_Enable			(NVIC_ISER0 |= (1<<9))
#define NVIC_IRQ10_EXTI4_Enable			(NVIC_ISER0 |= (1<<10))
#define NVIC_IRQ23_EXTI5_9_Enable		(NVIC_ISER0 |= (1<<23))
#define NVIC_IRQ40_EXTI10_15_Enable		(NVIC_ISER1 |= (1<<8))    // 40-32

// USART
#define NVIC_IRQ37_USART1_Enable		(NVIC_ISER1 |= (1<<5))    // 37-32
#define NVIC_IRQ38_USART2_Enable		(NVIC_ISER1 |= (1<<6))    // 38-32
#define NVIC_IRQ39_USART3_Enable		(NVIC_ISER1 |= (1<<7))    // 39-32


// SPI
#define NVIC_IRQ35_SPI1_Enable			(NVIC_ISER1 |= (1<<3))    // 35-32
#define NVIC_IRQ36_SPI2_Enable			(NVIC_ISER1 |= (1<<4))    // 36-32


#define NVIC_IRQ6_EXTI0_Disable			(NVIC_ICER0 |= (1<<6))
#define NVIC_IRQ7_EXTI1_Disable			(NVIC_ICER0 |= (1<<7))
#define NVIC_IRQ8_EXTI2_Disable			(NVIC_ICER0 |= (1<<8))
#define NVIC_IRQ9_EXTI3_Disable			(NVIC_ICER0 |= (1<<9))
#define NVIC_IRQ10_EXTI4_Disable		(NVIC_ICER0 |= (1<<10))
#define NVIC_IRQ23_EXTI5_9_Disable		(NVIC_ICER0 |= (1<<23))
#define NVIC_IRQ40_EXTI10_15_Disable	(NVIC_ICER1 |= (1<<8))    // 40-32



// USART
#define NVIC_IRQ37_USART1_Disable		(NVIC_ICER1 |= (1<<5))    // 37-32
#define NVIC_IRQ38_USART2_Disable		(NVIC_ICER1 |= (1<<6))    // 38-32
#define NVIC_IRQ39_USART3_Disable		(NVIC_ICER1 |= (1<<7))    // 39-32

// SPI
#define NVIC_IRQ35_SPI1_Disable			(NVIC_ICER1 |= (1<<3))    // 35-32
#define NVIC_IRQ36_SPI2_Disable			(NVIC_ICER1 |= (1<<4))    // 36-32


#endif /* INC_STM32F103C6_H_ */
