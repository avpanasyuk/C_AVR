/*
 * ATmega1284.cpp
 *
 * Created: 7/12/2014 1:37:44 PM
 *  Author: panasyuk
 */ 

#ifdef __AVR_ATmega1284__
#include "ATmega1284.h"

UART_INIT(0,0)
UART_INIT(1,1)

constexpr uint8_t Timer0Regs::Prescalers[];
constexpr uint8_t Timer1Regs::Prescalers[];
constexpr uint8_t Timer2Regs::Prescalers[];
constexpr uint8_t Timer3Regs::Prescalers[];

#endif

