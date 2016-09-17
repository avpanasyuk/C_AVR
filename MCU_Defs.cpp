/*
 * MCU_Defs.cpp
 *
 * Created: 10/14/2015 1:06:04 PM
 *  Author: panasyuk
 */

#include "MCU_Defs.h"

TIMER_INIT(0)
TIMER_INIT(1)
TIMER_INIT(2)

#if defined(__AVR_ATmega1284__)

UART_INIT(0,0)
UART_INIT(1,1)
TIMER_INIT(3)

#elif defined(__AVR_ATmega328P__)

UART_INIT(0,)

#elif defined(__AVR_ATmega88A__)

UART_INIT(0,)

#endif

