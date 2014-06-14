/*
 * SqrWaveGen.cpp
 *
 * Created: 10/26/2013 7:56:54 PM
 *  Author: panasyuk
 */

#include "SqrWave.h"

constexpr uint8_t Timer0::Prescalers[];
constexpr uint8_t Timer1::Prescalers[];
constexpr uint8_t Timer2::Prescalers[];
constexpr uint8_t Timer3::Prescalers[];
template<> constexpr uint8_t Timer<uint8_t>::Width;
template<> constexpr uint8_t Timer<uint16_t>::Width;
