/*
* ATMEGA88A_Timers.h
*
* Created: 6/14/2014 8:24:01 PM
*  Author: panasyuk
*/


#ifndef ATMEGA88A_TIMERS_H_
#define ATMEGA88A_TIMERS_H_

#ifndef __AVR_ATmega88A__
#error Invalid for this processor!
#endif

#include "HW_Timer.h"

TIMER_DEFS(0,8,,{0,3,6,8,10})
TIMER_DEFS(1,16,,{0,3,6,8,10})
TIMER_DEFS(2,8,,{0,3,5,6,7,8,10})

#endif /* ATMEGA88A_TIMERS_H_ */