/*!
* @file AVP_LIBS/AVR/ATmega328.h
*
* Created: 6/14/2014 8:24:01 PM
*  Author: panasyuk
* Defines TimerX static class for all timers present in the processor.
*/


#ifndef ATMEGA328P_TIMERS_H_
#define ATMEGA328P_TIMERS_H_

#ifndef __AVR_ATmega328P__
#error Invalid for this processor!
#endif

#include "HW_Timer.h"

TIMER_DEFS(0,8,, {0,3,6,8,10})
TIMER_DEFS(1,16,, {0,3,6,8,10})
TIMER_DEFS(2,8,, {0,3,5,6,7,8,10})

#include "HW_UART.h"
UART_DEF(0,,0)


#endif /* ATMEGA328P_TIMERS_H_ */