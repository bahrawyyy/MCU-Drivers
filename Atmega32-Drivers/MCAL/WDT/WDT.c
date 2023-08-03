/*
 * WDT.c
 *
 *  Created on: Aug 3, 2023
 *      Author: Abdallah
 */

#include "WDT.h"


ES_t WDT_START(u8 timing)
{
	// The WDP2, WDP1, and WDP0 bits determine the Watchdog Timer prescaling when the
	// Watchdog Timer is enabled
	WDTCR |= (1<<WDE) | timing;
	return ES_OK;
}



ES_t WDT_Stop(void)
{

//	- In the same operation, write a logic one to WDTOE and WDE. A logic one must
//	be written to WDE even though it is set to one before the disable operation starts.
//	- Within the next four clock cycles, write a logic 0 to WDE. This disables the
//	Watchdog.

	WDTCR |= (1<<WDE) | (1<<WDTOE);
	WDTCR = 0x00;

	return ES_OK;

}
