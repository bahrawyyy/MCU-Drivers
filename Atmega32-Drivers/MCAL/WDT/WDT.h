/*
 * WDT.h
 *
 *  Created on: Aug 3, 2023
 *      Author: Abdallah
 */

#ifndef MCAL_WDT_WDT_H_
#define MCAL_WDT_WDT_H_

#include "../Registers.h"
#include "../../LIB/Bit_Math.h"
#include "../../LIB/STD_Types.h"
#include "../../LIB/ErrorState.h"

// @ref WDT_Timing_Define
#define WDT_TIME_16_3	0
#define WDT_TIME_32_5	1
#define WDT_TIME_65		2
#define WDT_TIME_0_13	3
#define WDT_TIME_0_26	4
#define WDT_TIME_0_52	5
#define WDT_TIME_1		6
#define WDT_TIME_2_1	7


// the passed parameter must be
// set based on @ref WDT_Timing_Define

ES_t WDT_START(u8 timing);
ES_t WDT_Stop(void);

#endif /* MCAL_WDT_WDT_H_ */
