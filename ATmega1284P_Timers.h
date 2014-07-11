/*
* ATmega1284P_Timers.h
*
* Created: 6/14/2014 8:24:01 PM
*  Author: panasyuk
*/


#ifndef ATMEGA1284P_TIMERS_H_
#define ATMEGA1284P_TIMERS_H_

#ifndef __AVR_ATmega1284__
#error Invalid for this processor!
#endif

#include "HW_Timer.h"

TIMER_REGS_DEFS(0,8,0,{0,3,6,8,10})
TIMER_REGS_DEFS(1,16,0,{0,3,6,8,10})
TIMER_REGS_DEFS(2,8,0,{0,3,5,6,7,8,10})
TIMER_REGS_DEFS(3,16,1,{0,3,6,8,10})

#endif /* ATMEGA1284P_TIMERS_H_ */