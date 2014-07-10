/*
* ADC.h
*
* Created: 6/28/2014 10:06:08 PM
*  Author: panasyuk
*/


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include "service.h"

//! NOTE: ADC clock frequency is between 50 and 200 kHz, normal conversion takes 13 cycles
namespace avp_ADC {
  // Next two function are dealing with a sleep state, first conversion takes the same time
  inline void PowerOn() { avp::set_low(PRR,PRADC); }
  inline void PowerOff() { avp::set_high(PRR,PRADC); }
    
  // ********* ADMUX *************
  enum Reference { AREF, AVCC, INTERNAL1_1V /* later two need external cap on AREF pin */,  };
  inline void SelectReference( enum Reference Ref ) {  avp::setbits(ADMUX, REFS0, 2, Ref); }
  //! how 10 bits are adjusted in 16-bits output
  inline void SetLeftAdjust(bool Set=true) { avp::setbit(ADMUX,ADLAR,Set?1:0); }
  enum Channel { CH0 /* from 0 to 7 */,  TEMP=8, REF_1_1V=14, GND=15 };
  inline void SetChannel( enum Channel Ch ) { avp::setbits(ADMUX, MUX0, 4, Ch); }

  // ********* ADCSRA ************
  // This is major disable, first conversion after Enable takes longer
  inline void Enable() { avp::set_high(ADCSRA,ADEN); }
  inline void Disable() { avp::set_low(ADCSRA,ADEN); }
  inline void Start() { avp::set_high(ADCSRA,ADSC); }
  inline bool Ready() { return avp::getbit(ADCSRA,ADSC); }
  inline void AutoTrigger(bool Set=true) { avp::setbit(ADCSRA, ADATE, Set); }
  // do not forget global interrupt enable sei() 
  inline void EnableInterrupts() { avp::set_high(ADCSRA,ADIE); } 
  inline void DisableInterrupts() { avp::set_low(ADCSRA,ADIE); }
  inline void SetPrescaler() { avp::setbits(ADCSRA,ADPS0,3,avp::CeilLog2Ratio(F_CPU,200000UL)); }
    
  // ******** ADCSRB *************
  enum AutoTriggerSource { FREE, AN_COMPARATOR, EXT_INT_0, TIMER0_COMPA, TIMER0_OVR,  TIMER1_COMPB, TIMER1_OVR, TIMER1_CAPT };
  inline void SetAutoTriggerSource( enum AutoTriggerSource Src ) { avp::setbits( ADCSRB, ADTS0, 3, Src); }    
    
  // ******** SERVICE ************  
  inline void Init() { SetPrescaler(); }
} // avp_ADC


#endif /* ADC_H_ */