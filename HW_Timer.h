/*
 * HW_Timer.h
 *
 * Created: 7/3/2014 7:29:32 PM
 *  Author: panasyuk
 */ 


#ifndef HW_TIMER_H_
#define HW_TIMER_H_

#include <General.h>

#define TIMER_DEF(I,nbits,...) \
  struct _COMB(Timer,I,Regs) { \
    static constexpr uint8_t PRTIMx = _COMB2(PRTIM,I); \
    static constexpr volatile uint8_t *pTCCRxA = &_COMB(TCCR,I,A); \
    static constexpr uint8_t COMxA0 = _COMB(COM,I,A0); \
    static constexpr uint8_t WGMx1 = _COMB(WGM,I,1); \
    static constexpr volatile uint8_t *pTCCRxB = &_COMB(TCCR,I,B); \
    static constexpr uint8_t CSx0 = _COMB(CS,I,0); \
    static constexpr volatile _COMB(uint,nbits,_t) *pOCRxA = &_COMB(OCR,I,A); \
    static constexpr volatile uint8_t *pPRRx = &_COMB2(PRR,__VA_ARGS__); \
    static constexpr volatile _COMB(uint,nbits,_t) *pTCNTx = &_COMB2(TCNT,I); \
    static constexpr volatile uint8_t *pTIMSKx = &_COMB2(TIMSK,I); \
    static constexpr uint8_t OCIExA = _COMB(OCIE,I,A); \
 }; //TimerXRegs
  
template<typename CounterType> struct TimerBase {
  static constexpr uint8_t Width = sizeof(CounterType) << 3;
  typedef struct {
    CounterType CountTo; // divider = CountTo + 1
    uint8_t PrescalerInd; //! prescaler index, first active is 1
  } Params;
}; // Timer

template<class TimerRegs> struct Timer8bit:public TimerBase<uint8_t> {
	  static constexpr volatile uint8_t *pTCNT() { return TimerRegs::pTCNTx; } 
      
  	static void InitCTC() {
    	*TimerRegs::pPRRx &= ~(1<<TimerRegs::PRTIMx); // remove Timer0 bit from the power reduction register
    	*TimerRegs::pTCCRxA = (1<<TimerRegs::COMxA0)|(1<<TimerRegs::WGMx1);  // toggle OC0A on compare match, set CTC (count to OCR0A) mode,
  	}

  	static void SetupCTC(Params Codes) {
    	*TimerRegs::pTCCRxB = Codes.PrescalerInd<<TimerRegs::CSx0;
    	*TimerRegs::pOCRxA = Codes.CountTo;
  	}
}; // Timer8bit 
  
template<class TimerRegs> struct Timer16bit:public TimerBase<uint16_t> {
  static constexpr volatile uint16_t *pTCNT() { return TimerRegs::pTCNTx; }
  
  static void InitCTC() {
    *TimerRegs::pPRRx &= ~(1<<TimerRegs::PRTIMx); // remove Timer0 bit from the power reduction register
    *TimerRegs::pTCCRxA = (1<<TimerRegs::COMxA0);  // toggle OC0A on compare match, set CTC (count to OCR0A) mode,
  }

  static void SetupCTC(Params Codes) {
    *TimerRegs::pTCCRxB = Codes.PrescalerInd<<TimerRegs::CSx0|(1<<TimerRegs::WGMx2);
    *TimerRegs::pOCRxA = Codes.CountTo;
  }
  
  	static void InitTimeCounter(uint16_t ClocksInKibitick, uint8_t PrescalerI) {
    	// OK, we build time counter on 16-bits timer 3
    	*TimerRegs::pPRRx &= ~(1<<TimerRegs::PRTIMx); // turn on power on the counter
    	*TimerRegs::pOCRxA = ClocksInKibitick; // we do interrupt when we count to about a  millisecond
    	*TimerRegs::pTIMSKx |= (1<<TimerRegs::OCIExA); // enable compare interrupts
    	*TimerRegs::pTCCRxA = 0;
    	*TimerRegs::pTCCRxB = (PrescalerI+1)<<TimerRegs::CSx0;  // PrescalerI is 1-based, because value 0 turns it off
    	sei();
  	} // Init
}; // Timer16bit

#endif /* HW_TIMER_H_ */