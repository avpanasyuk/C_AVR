/*
 * HW_Timer.h
 *
 * Created: 7/3/2014 7:29:32 PM
 *  Author: panasyuk
 */


#ifndef HW_TIMER_H_
#define HW_TIMER_H_

#include <avr/io.h>
#include <AVP_LIBS/General/General.h>
#include "General.h"

// WGM - Waveform Generation Mode
// COM - Compare Output Mode
#define TIMER_REGS_DEFS(I,nbits,PRRi,...) \
  struct __COMB(Timer,I,Regs) { \
    typedef __COMB(uint,nbits,_t) CounterType ; \
    static constexpr uint8_t Width = nbits; \
    REG_PTR_DEF(TCCR,I,A) \
    REG_PTR_DEF(TCCR,I,B) \
    REG_PTR_DEF(OCR,I,A) \
    REG_PTR_DEF(OCR,I,B) \
    REG_PTR_DEF(PRR,PRRi,) \
    REG_PTR_DEF(TCNT,I,) \
    REG_PTR_DEF(TIMSK,I,) \
    BIT_NUM_DEF(PRTIM,I,) \
    BIT_NUM_DEF(COM,I,A0) \
    BIT_NUM_DEF(WGM,I,2) \
    BIT_NUM_DEF(WGM,I,1) \
    BIT_NUM_DEF(WGM,I,0) \
    BIT_NUM_DEF(CS,I,0) \
    BIT_NUM_DEF(OCIE,I,A) \
    static constexpr uint8_t Prescalers[] = __VA_ARGS__; \
  }; //TimerXRegs

template<class TimerRegs> struct HW_Timer: public TimerRegs {
  typedef typename TimerRegs::CounterType CounterType;
  static constexpr volatile CounterType *pCounter() { return TimerRegs::pTCNTx; }
  static void Power(bool State) { avp::setbit(*TimerRegs::pPRRx,TimerRegs::PRTIMx,!State); }
  static void SetCountToValue(CounterType Value) { *TimerRegs::pOCRxA = Value; }
  //! @param PrescalerI is just a value from CSxx table, 0 stops clock
  static void SetPrescaler(uint8_t PrescalerI) { 
    avp::setbits(*TimerRegs::pTCCRxB,TimerRegs::CSx0,3,PrescalerI); 
  }
}; // Timer

template<class TimerRegs> struct Timer8bits:public HW_Timer<TimerRegs> {
  typedef HW_Timer<TimerRegs> R;
  static void InitCTC() {
    avp::setbits(*R::pTCCRxA,R::COMxA0,2,1); // Toggle OCxA on Compare Match.
    avp::setbits(*R::pTCCRxA,R::WGMx0,2,2);  // CTC mode
    avp::set_low(*R::pTCCRxB,R::WGMx2);
  }
  static void InitPWM() {
    // Fast PWM mode, Clear OC0A on Compare Match, set OC0A at BOTTOM
    // Do not forget to set Prescaler
    *R::pTCCRxA = (2 << R::COMxA0)|(3 << R::WGMx0);
    avp::set_low(*R::pTCCRxB,R::WGMx2);
  }
}; // Timer8bits

template<class TimerRegs> struct Timer16bits:public HW_Timer<TimerRegs> {
  typedef HW_Timer<TimerRegs> R;
  static void InitCTC() {
    avp::setbits(*R::pTCCRxA,R::COMxA0,2,1); // Toggle OCxA on Compare Match.
    avp::setbits(*R::pTCCRxA,R::WGMx0,2,0);  // CTC mode
    avp::setbits(*R::pTCCRxB,R::WGMx2,2,1);
  }
  //! @param NumBitIndex: 0- 8 bits (counts to 0xFF), 1 - 9 bits (counts to 0x1FF), 2 - 10 bits (counts to 0x3FF)
  static void InitPWM(uint8_t NumBitIndex = 0) {
    *R::pTCCRxA = (2 << R::COMxA0)|((NumBitIndex+1) << R::WGMx0);
    avp::setbits(*R::pTCCRxB,R::WGMx2,2,1);
  }
}; // Timer16bits

#endif /* HW_TIMER_H_ */