/*
 * PWM.h
 *
 * Created: 7/4/2014 6:11:18 PM
 *  Author: panasyuk
 */ 


#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>

//! Fast Mode 8-bit PWM generator
template<class TimerRegs> struct PWM: public HW_Timer<TimerRegs> {
  typedef HW_Timer<TimerRegs> R;
  // OCxA port is used and should by set to output mode elsewhere
  static void Init() {
    R::Power(1);
    R::InitPWM();
  } // Init
  
  PWM() { Init(); }
  
  static void SetDuty(uint8_t Value) { R::SetCountToValue(Value); }
}; // PWM

#endif /* PWM_H_ */