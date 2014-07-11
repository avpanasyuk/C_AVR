/*
 * PWM.h
 *
 * Created: 7/4/2014 6:11:18 PM
 *  Author: panasyuk
 */


#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>
#include "HW_Timer.h"


//! Fast Mode 8-bit PWM generator. The signal apperas on OCxA pin of the corresponding timer. You should enable this pin for output
template<class Timer> struct PWM: public Timer {
  // OCxA port is used and should by set to output mode elsewhere
  static void Init() {
    Timer::Power(1);
    Timer::InitPWM();
    Timer::SetPrescaler(1); // highest frequency
  } // Init

  PWM() { Init(); }

  static void SetDuty(typename Timer::CounterType Value) { Timer::SetCountToValue(Value); }
}; // PWM

#endif /* PWM_H_ */