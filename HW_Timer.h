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
#include <AVP_LIBS/General/Error.h>
#include "General.h"

namespace avp {
  static constexpr uint32_t BaseClock = F_CPU/2;
//! class for functions which work identical for 8 bit and 16 bit timers. OBJECTS SHOULD NOT BE CREATED:
//! USE ascendants Timer8buts or Timer16bits
//! @tparam TimerRegs - class created by using TIMER_DEFS #define in the MCU_Defs class.
//!       it looks like Timer0Regs, Timer1Regs etc
  template<class TimerRegs> struct HW_Timer: public TimerRegs {
    typedef typename TimerRegs::CounterType CounterType;
    static constexpr volatile CounterType *pCounter() { return TimerRegs::pTCNTx; }
    static void Power(bool State) { avp::setbit(*TimerRegs::pPRRx,TimerRegs::PRTIMx,!State); }
    static void SetCountToValueA(CounterType Value) { *TimerRegs::pOCRxA = Value; }
    static void SetCountToValueB(CounterType Value) { *TimerRegs::pOCRxB = Value; }
    static void EnableCompareInterrupts() { avp::set_high(*TimerRegs::pTIMSKx, TimerRegs::OCIExA); }
    //! determine how output to OCxA/OCxB pins behave
    //! @param A is 2 bits of TCCRxA register - COMxA1, COMxA0
    //! @param B is 2 bits of TCCRxB register - COMxB1, COMxB0
    static void SetCompareOutputMode(uint8_t A, uint8_t B=0) {
      avp::setbits(*TimerRegs::pTCCRxA, TimerRegs::COMxB0, 4, (A<<2) | B);
    }
    //! @param PrescalerI is just a value from CSxx table, 0 stops clock. Use GetPrescaler() to get value
    static void SetPrescalerI(uint8_t PrescalerI) {
      avp::setbits(*TimerRegs::pTCCRxB,TimerRegs::CSx0,3,PrescalerI);
    }
    static constexpr uint8_t GetLastPrescalerI() { return N_ELEMENTS(TimerRegs::Prescalers); }

    //! @param PrescalerI is just a value from CSxx table, 0 stops clock. The prescaler set is equal to Prescalers[PrescalerI-1].
    //!           by default uses maximum prescaler
    static constexpr uint8_t GetLog2Prescaler(uint8_t PrescalerI = GetLastPrescalerI()) {
      return TimerRegs::Prescalers[PrescalerI-1];
    }
    static constexpr CounterType GetMaxDivider() { return CounterType(-1); }

    //! returns clock value before divider
    //! @param PrescalerI is just a value from CSxx table, 0 stops clock. Use GetPrescaler() to get value
    static constexpr uint32_t GetClock(uint8_t PrescalerI = GetLastPrescalerI()) {
      return BaseClock >> GetLog2Prescaler(PrescalerI);
    } // GetClock

    //! returns clock value after divider
    //! @param PrescalerI is just a value from CSxx table, 0 stops clock. Use GetPrescaler() to get value
    static constexpr float GetFreq(CounterType Divider = GetMaxDivider(),uint8_t PrescalerI = GetLastPrescalerI()) {
      return float(GetClock(PrescalerI))/Divider;
    } // GetClock


    //! Get biggest prescaler index at which Frequency is still higher then Freq
    //! @param Freq - minimum freq
    //! @param CurPrescalerI - starting index in looking for PrescaledI, default is minimun PrescalerI
    //! @return PrescalerI of maximim prescaler which still produces frequency higher than Freq, or minimum PrescalerI
    static constexpr int8_t GetPrescalerIndex(uint32_t Freq, int8_t CurPrescalerI = 1) {
      return GetFreq(CurPrescalerI) <= Freq?CurPrescalerI:GetPrescalerIndex(Freq,CurPrescalerI+1);
    }
    static void InterruptHandler();
  }; // Timer

  template<class TimerRegs>
  __weak void HW_Timer<TimerRegs>::InterruptHandler() {
    hang_cpu();
  };

  template<class TimerRegs> struct Timer8bits:public HW_Timer<TimerRegs> {
    typedef HW_Timer<TimerRegs> R;
    static void InitCTC() { SetWaveformGenerationMode(2); } // Clear counter on compare
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
    static void InitCTC() { SetWaveformGenerationMode(4); } // Clear counter on compare
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
}

//! this timer definition should be used in MCU_Defs.h, where they define all timers for this processor
//! it defines Timer0, Timer1. etc static "Timer?bits" classes
#define TIMER_DEFS(I,nbits,PRRi,...) \
using namespace avp; \
struct COMB3(Timer,I,Regs) { \
  typedef COMB3(uint,nbits,_t) CounterType ; \
  static constexpr uint8_t Width = nbits; \
  static constexpr uint8_t TimerIndex = I; \
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
typedef COMB3(Timer,nbits,bits)<COMB3(Timer,I,Regs)> COMB2(Timer,I); // defines Timer? as Timer?bits<Timer?Regs>

//! this macro should be called in MCU_Defs.cpp file where they sets interrupt handler
#define TIMER_INIT(I) \
    ISR(COMB3(TIMER,I,_COMPA_vect)) { COMB2(Timer,I)::InterruptHandler(); } \
    constexpr uint8_t COMB3(Timer,I,Regs)::Prescalers[];


//TimerXRegs

#endif /* HW_TIMER_H_ */