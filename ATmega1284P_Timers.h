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

#include <avr/io.h>
#include "Timer.h"
#include "HW_Timer.h"

TIMER_DEF(0,8,0)
TIMER_DEF(1,16,0)
TIMER_DEF(2,8,0)
TIMER_DEF(3,16,1)

struct Timer0: public Timer8bit<Timer0Regs> {
  static constexpr uint8_t Prescalers[] = {0,3,6,8,10}; // powers of 2
};

struct Timer1: public Timer16bit<Timer1Regs> {
  static constexpr uint8_t Prescalers[] = {0,3,6,8,10}; // powers of 2
};

struct Timer2: public Timer8bit<Timer2Regs> {
  static constexpr uint8_t Prescalers[] = {0,3,5,6,7,8,10}; // powers of 2
};

struct Timer3: public Timer16bit<Timer1Regs> {
  static constexpr uint8_t Prescalers[] = {0,3,6,8,10}; // powers of 2
};

#endif /* ATMEGA1284P_TIMERS_H_ */