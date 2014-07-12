/*
 * ATmega88A.cpp
 *
 * Created: 7/12/2014 2:18:55 PM
 *  Author: panasyuk
 */ 

#ifdef __AVR_ATmega88A__
#include "ATmega88A.h"

UART_INIT(0,)

constexpr uint8_t Timer0Regs::Prescalers[];
constexpr uint8_t Timer1Regs::Prescalers[];
constexpr uint8_t Timer2Regs::Prescalers[];

#endif
