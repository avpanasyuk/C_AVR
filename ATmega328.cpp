/*
 * ATmega328.cpp
 *
 * Created: 7/12/2014 1:37:44 PM
 *  Author: panasyuk
 */

#ifdef __AVR_ATmega328P__
#include "ATmega328.h"

UART_INIT(0,)

constexpr uint8_t Timer0Regs::Prescalers[];
constexpr uint8_t Timer1Regs::Prescalers[];
constexpr uint8_t Timer2Regs::Prescalers[];

#endif

