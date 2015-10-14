/*
 * MCU_Defs.cpp
 *
 * Created: 10/14/2015 1:06:04 PM
 *  Author: panasyuk
 */

#include "MCU_Defs.h"

#if defined(__AVR_ATmega1284__)

UART_INIT(0,0)
UART_INIT(1,1)

constexpr uint8_t Timer0Regs::Prescalers[];
constexpr uint8_t Timer1Regs::Prescalers[];
constexpr uint8_t Timer2Regs::Prescalers[];
constexpr uint8_t Timer3Regs::Prescalers[];

#elif defined(__AVR_ATmega328P__)

UART_INIT(0,)

constexpr uint8_t Timer0Regs::Prescalers[];
constexpr uint8_t Timer1Regs::Prescalers[];
constexpr uint8_t Timer2Regs::Prescalers[];

#elif defined(__AVR_ATmega88A__)

UART_INIT(0,)

constexpr uint8_t Timer0Regs::Prescalers[];
constexpr uint8_t Timer1Regs::Prescalers[];
constexpr uint8_t Timer2Regs::Prescalers[];

#endif

