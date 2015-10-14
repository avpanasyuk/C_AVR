/*
 * MCU_Defs.h
 *
 * Created: 10/14/2015 12:32:45 PM
 *  Author: panasyuk
 */


#ifndef MCU_DEFS_H_
#define MCU_DEFS_H_

#include "HW_Timer.h"

#if defined(__AVR_ATmega1284__)

#define ADC_PRR PRR0

TIMER_DEFS(0,8,0, {0,3,6,8,10})
TIMER_DEFS(1,16,0, {0,3,6,8,10})
TIMER_DEFS(2,8,0, {0,3,5,6,7,8,10})
TIMER_DEFS(3,16,1, {0,3,6,8,10})

#include "HW_UART.h"
UART_DEF(0,0,0)
UART_DEF(1,1,1)

#elif defined(__AVR_ATmega328P__)

TIMER_DEFS(0,8,, {0,3,6,8,10})
TIMER_DEFS(1,16,, {0,3,6,8,10})
TIMER_DEFS(2,8,, {0,3,5,6,7,8,10})

#include "HW_UART.h"
UART_DEF(0,,0) // UARTx::Init(bauds) should be called
#elif defined(__AVR_ATmega88A__)

TIMER_DEFS(0,8,, {0,3,6,8,10})
TIMER_DEFS(1,16,, {0,3,6,8,10})
TIMER_DEFS(2,8,, {0,3,5,6,7,8,10})

#include "HW_UART.h"
UART_DEF(0,,)

#else
#error Invalid for this processor! Got to add the MCU to MCU_Defs.h and MCU_defs.cpp
#endif

#ifndef ADC_PRR
#define ADC_PRR PRR
#endif

#endif /* MCU_DEFS_H_ */