/*!
* @file AVP_LIBS/AVR/ADC.h
*
* Created: 6/28/2014 10:06:08 PM
*  Author: panasyuk
*  In default mode we are running ADC full speed oversampling and averaging
*  ADC_::Init();
*  ADC_::SelectReference(ADC_::AVCC);
*  ADC_::SelectChannel(ADC_::CH0);
*  ADC_::StartFreeRunning(); which start averaging accumulated samples
*  then do ADC_::ReadFreeRun(); // return result and resets accumulator
*/


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include "General.h"
#include "TimeCounter.h"

//! NOTE: ADC_ clock frequency is between 50 and 200 kHz, normal conversion takes 13 cycles
namespace avp {
  namespace ADC_ { // we can not use name "ADC" because it is #defined as a name of register
    // Next two function are dealing with a sleep state, first conversion takes the same time
    inline void PowerOn() { avp::set_low(PRR,PRADC); }
    inline void PowerOff() { avp::set_high(PRR,PRADC); }

    // ********* ADMUX *************
    enum Reference { AREF, AVCC, INTERNAL1_1V /* later two need external cap on AREF pin */,  };
    inline void SelectReference( enum Reference Ref ) {  avp::setbits(ADMUX, REFS0, 2, Ref); }

    //! how 10 bits are adjusted in 16-bits output
    inline void SetLeftAdjust(bool Set=true) { avp::setbit(ADMUX,ADLAR,Set?1:0); }
    enum Channel { CH0 /* from 0 to 7 */,  TEMP=8, REF_1_1V=14, GND=15 };
    inline void SelectChannel( uint8_t /* enum Channel */ Ch ) { avp::setbits(ADMUX, MUX0, 4, Ch); }

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
    inline void Init() {
      PowerOn();
      SetPrescaler();
      Enable();
    } // Init
    inline uint16_t ReadOut() { return ADCW; }

    template<class time_out, uint32_t Interval> uint16_t Read() {
      time_out Out(Interval);
      Start();
      while(!Ready()) if(Out) return -1;
      return ReadOut();
    }
    void ConvertionComplete_Interrupt();

    // Free Running - oversampling at max
    void StartFreeRunning();
    uint16_t ReadFreeRun();
    uint16_t GetCurrentNumSamples();
  } // ADC_
} // avp


#endif /* ADC_H_ */