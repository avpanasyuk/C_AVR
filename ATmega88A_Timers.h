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

#include <avr/io.h>
#include <avr/interrupt.h>
#include "HW_Timer.h"

TIMER_DEF(0,8)
TIMER_DEF(1,16)
TIMER_DEF(2,8)

struct Timer0: public Timer8bit<Timer0Regs> {
	static constexpr uint8_t Prescalers[] = {0,3,6,8,10}; // powers of 2  
};

struct Timer1: public Timer16bit<Timer1Regs> {
	static constexpr uint8_t Prescalers[] = {0,3,6,8,10}; // powers of 2
};

struct Timer2: public Timer8bit<Timer2Regs> {
  static constexpr uint8_t Prescalers[] = {0}; // powers of 2
};

#endif /* ATMEGA88A_TIMERS_H_ */