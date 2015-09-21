/*
 * HW_Timer.h
 *
 * Created: 7/3/2014 7:29:32 PM
 *  Author: panasyuk
 */


#ifndef HW_TIMER_H_
#define HW_TIMER_H_

#include <avr/io.h>
#include <AVP_LIBS/General/BitBang.h>
#include <AVP_LIBS/General/Macros.h>
#include "General.h"

//! @brief class for functions which work identical for 8 bit and 16 bit timers
template<class TimerRegs> struct HW_Timer: public TimerRegs {
  typedef typename TimerRegs::CounterType CounterType;
  static constexpr volatile CounterType *pCounter() { return TimerRegs::pTCNTx; }
  static void Power(bool State) { avp::setbit(*TimerRegs::pPRRx,TimerRegs::PRTIMx,!State); }
  static void SetCountToValueA(CounterType Value) { *TimerRegs::pOCRxA = Value; }
  static void SetCountToValueB(CounterType Value) { *TimerRegs::pOCRxB = Value; }
  static void EnableCompareInterrupts() { avp::set_high(*TimerRegs::pTIMSKx, TimerRegs::OCIExA); }
  //! @param A is 2 bits of TCCRxA register - COMxA1, COMxA0
  //! @param B is 2 bits of TCCRxB register - COMxB1, COMxB0
  static void SetCompareOutputMode(uint8_t A, uint8_t B=0) { 
    avp::setbits(*TimerRegs::pTCCRxA, TimerRegs::COMxB0, 4, (A<<2) | B); 
  }
  //! @param PrescalerI is just a value from CSxx table, 0 stops clock. The prescaler set is equal to Prescalers[PrescalerI-1]
  static void SetPrescaler(uint8_t PrescalerI) { 
    avp::setbits(*TimerRegs::pTCCRxB,TimerRegs::CSx0,3,PrescalerI); 
  }
  //! @retval index, or -1 if Value is not among prescalers
  static constexpr int8_t GetPrescalerIndex(uint16_t Value, int8_t CurIndex = 0) {
    return (1U << TimerRegs::Prescalers[CurIndex]) == Value?CurIndex:
      (CurIndex == (N_ELEMENTS(TimerRegs::Prescalers)-1))?-1:GetPrescalerIndex(Value,CurIndex+1);
  }
}; // Timer

template<class TimerRegs> struct Timer8bits:public HW_Timer<TimerRegs> {
  typedef HW_Timer<TimerRegs> R;
  static void InitCTC() {
    R::SetCompareOutputMode(1); // Toggle OCxA on Compare Match.
    SetWaveformGenerationMode(2);
  }
  static void InitPWM(bool A=true, bool B=false) {
    // Fast PWM mode, Clear OC0A on Compare Match, set OC0A at BOTTOM
    // Do not forget to set Prescaler
    R::SetCompareOutputMode(A?0b10:0,B?0b10:0); // clear OCxx on Compare Match, set on 0
    SetWaveformGenerationMode(3);
  }
  static void SetWaveformGenerationMode(uint8_t Value) {
    avp::setbits(*R::pTCCRxA, R::WGMx0, 2, Value & 0x3);
    avp::setbit(*R::pTCCRxB, R::WGMx2, Value >> 2);
  }
}; // Timer8bits

template<class TimerRegs> struct Timer16bits:public HW_Timer<TimerRegs> {
  typedef HW_Timer<TimerRegs> R;
  static void InitCTC() {
    R::SetCompareOutputMode(1); // Toggle OCxA on Compare Match.
    SetWaveformGenerationMode(4); // CTC mode
  }
  //! @param NumBitIndex: 0- 8 bits (counts to 0xFF), 1 - 9 bits (counts to 0x1FF), 2 - 10 bits (counts to 0x3FF)
  static void InitPWM(bool A=true, bool B=false, uint8_t NumBitIndex = 0) {
    R::SetCompareOutputMode(A?0b10:0,B?0b10:0); // clear OCxx on Compare Match, set on 0
    SetWaveformGenerationMode(4|(NumBitIndex+1)); // CTC mode
  }
  static void SetWaveformGenerationMode(uint8_t Value) {
    avp::setbits(*R::pTCCRxA, R::WGMx0, 2, Value & 0x3);
    avp::setbits(*R::pTCCRxB, R::WGMx2, 2, Value >> 2);
  }
}; // Timer16bits

//! this timer definition should be used in processor specific header files only, where they define all timers for this processor
//! it defines Timer0, Timer1. etc static "Timer?bits" classes
#define TIMER_DEFS(I,nbits,PRRi,...) \
struct COMB3(Timer,I,Regs) { \
  typedef COMB3(uint,nbits,_t) CounterType ; \
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
  BIT_NUM_DEF(COM,I,B0) \
  BIT_NUM_DEF(WGM,I,2) \
  BIT_NUM_DEF(WGM,I,1) \
  BIT_NUM_DEF(WGM,I,0) \
  BIT_NUM_DEF(CS,I,0) \
  BIT_NUM_DEF(OCIE,I,A) \
  static constexpr uint8_t Prescalers[] = __VA_ARGS__; \
}; \
typedef COMB3(Timer,nbits,bits)<COMB3(Timer,I,Regs)> COMB2(Timer,I);

//TimerXRegs

#endif /* HW_TIMER_H_ */