/*
 * HW_Timer.h
 *
 * Created: 7/3/2014 7:29:32 PM
 *  Author: panasyuk
 */


#ifndef HW_TIMER_H_
#define HW_TIMER_H_

#include <avr/io.h>
#include <General.h>

// WGM - Waveform Generation Mode
// COM - Compare Output Mode
#define TIMER_REGS_DEF(I,nbits,PRRi,...) \
  struct _COMB(Timer,I,Regs) { \
    typedef _COMB(uint,nbits,_t) CounterType ; \
    static constexpr uint8_t Width = nbits; \
    static constexpr uint8_t PRTIMx = _COMB2(PRTIM,I); \
    static constexpr volatile uint8_t *pTCCRxA = &_COMB(TCCR,I,A); \
    static constexpr uint8_t COMxA0 = _COMB(COM,I,A0); \
    static constexpr uint8_t WGMx1 = _COMB(WGM,I,1); \
    static constexpr volatile uint8_t *pTCCRxB = &_COMB(TCCR,I,B); \
    static constexpr uint8_t CSx0 = _COMB(CS,I,0); \
    static constexpr volatile CounterType *pOCRxA = &_COMB(OCR,I,A); \
    static constexpr volatile CounterType *pOCRxB = &_COMB(OCR,I,B); \
    static constexpr volatile uint8_t *pPRRx = &_COMB2(PRR,PRRi); \
    static constexpr volatile CounterType *pTCNTx = &_COMB2(TCNT,I); \
    static constexpr volatile uint8_t *pTIMSKx = &_COMB2(TIMSK,I); \
    static constexpr uint8_t OCIExA = _COMB(OCIE,I,A); \
    static constexpr uint8_t Prescalers[] = __VA_ARGS__; \
  }; //TimerXRegs

template<class TimerRegs> struct HW_Timer: public TimerRegs {
  typedef typename TimerRegs::CounterType CounterType;
  static constexpr volatile CounterType *pCounter() { return TimerRegs::pTCNTx; }
  static void Power(bool State) { avp::setbit(*TimerRegs::pPRRx,TimerRegs::PRTIMx,!State); }
  static void SetCountToValue(CounterType Value) { *TimerRegs::pOCRxA = Value; }
  //! @param PrescalerI is just a value from CSxx table, 0 stops clock  
  static void SetPrescaler(uint8_t PrescalerI) { avp::setbits(*TimerRegs::pTCCRxB,TimerRegs::CSx0,3,PrescalerI); }  
}; // Timer

template<class TimerRegs> struct Timer8bits:public HW_Timer<TimerRegs> {
  typedef HW_Timer<TimerRegs> R;
  static void InitCTC() {
    avp::setbits(*R::pTCCRxA,R::COMxA0,2,1); // Toggle OCxA on Compare Match.
    avp::setbits(*R::pTCCRxA,R::WGMx0,2,2);  // CTC mode
    avp::set_low(*R::pTCCRxB,R::WGMx2);
   }
  static void InitPWM() {
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