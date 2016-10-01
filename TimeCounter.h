/*
* Time.h
*
* Created: 8/1/2013 7:22:28 PM
*  Author: panasyuk
* function dealing with time
* including file should include a header file with HW Timer classes first
* so, use it like this:
-------------- in header file ---------------
#include "ATmega88A_Timers.h"
#include "TimeCounter.h"

DEFINE_SYSTEM_TIMER(Timer?)
-------------- in source file ----------------
INIT_SYSTEM_TIMER
*/


#ifndef TIME_COUNTER_H_
#define TIME_COUNTER_H_

// #include <AVP_LIBS/General/Math.h>
#include "MCU_Defs.h"
// #include "service.h"

namespace avp {
  //! @tparam Timer -  Timer? defined in HW_Timer
  template<class Timer>
  struct TimeCounter: public Timer {
    //! setting up and starting timer, by default - in slowest configuration.
    //! Frequency of interrupt calling = BaseClock/Timer::GetPrescaler(PrescalerI)/Divider.
    //! @param PrescalerI -  index in Timer::Prescalers[PrescalerI - 1]. If PrescalerI == 0 timer stops
    //! @param Divider -  clock divider, if not 0 coujnter does to this value, genrerates interrupt and resets
    //!                   if 0 counter just rolls over
    static void Setup(void (*pInterruptCallback)() = nullptr, typename Timer::CounterType Divider = Timer::GetMaxDivider(),
                      uint8_t PrescalerI = Timer::GetLastPrescalerI()) {
      Timer::pInterruptCallback = pInterruptCallback;
      Timer::Power(1);
      if(Divider == 0) Timer::SetWaveformGenerationMode(0); else Timer::InitCTC();
      Timer::SetCountToValueA(Divider-1); // ticks=clocks/(1+CountToValue)
      Timer::SetCompareOutputMode(0); // do not toggle output pin
      Timer::SetPrescalerI(PrescalerI); // start timer
      Timer::EnableCompareInterrupts();
      sei();
    } // Setup

    //! the class is used mostly as a static class, this constructor just so we can call Setup
    //! function when static variables are initialized and not from a function
    //! @see Setup for parameter description
    TimeCounter(void (*pInterruptCallback)() = nullptr, typename Timer::CounterType Divider = Timer::GetMaxDivider(),
                uint8_t PrescalerI = Timer::GetLastPrescalerI()) {
      Setup(pInterruptCallback, Divider, PrescalerI);
    } // constructor
  }; // TimeCounter


// @ tparam Timer -  either Timer8bits or Timer16bits from HW_Timer.h
  template<class Timer>
  class SystemTimer: public TimeCounter<Timer> {
    /*! Ok, how do we select a prescaler? We need both micro and millisecond scales, so we need 16-bit timer/counter,
    * because if it counts faster then microseconds it we roll over before millisecond comes. So, the only consideration is that
    * it counts faster then microsecond.
    * The idea is that we run counter with higher then 1 MHz freq, and interrupts with higher then 1 KHz, and each interrupt resets timer.
    * This way we have access to precise submicrosec timer value, but not overload system with too frequent interrupt
    * One consideration about this class is that it has to be fast, so we can not do divisions because AVR processor does
    * not have division and it takes forever. We use only shift instead of divisions. So we can not get micro and millisecond PRECISELY,
    * instead we will have "clock" which is as close as possible but smaller then microsecond and "tick" which is as close as possible
    * but smaller then millisecond.
    * ALL DIVIDERS SHOULD BE POWER OF 2 to be fast in main function _tick() call, so we can use Log2 values everywhere
    * CLOCK is the frequency counter is counting
    * TICKS is the frequency interrupt is triggered
    */

    // @note clocks = BaseClock/prescaler !!!!
    // ticks=clocks/(1+CountToValue)

    static volatile uint32_t Ticks; // interrupt counter


  public:
    static constexpr uint32_t MinClockFreq = 1000000UL; // CLOCK is the frequency counter is counting
    // we want clocks to have better resolution than 1 microsecond
    static constexpr uint8_t PrescalerI = Timer::GetPrescalerIndex(MinClockFreq); // prescaler acts on clocks
    static constexpr uint8_t PrescalerLog2 = Timer::GetLog2Prescaler(PrescalerI);
    // Ok, the problem is that Prescaler values are not always consequent powers of 2, there may be gaps. So Clocks may be more then
    // twice faster then  MinClockFreq.
    // TICKS is the frequency interrupt is triggered. Which is every timer 16 bit counter rolls over
    static constexpr uint8_t ClocksInTickLog2 = 16; // log2(clocks/ticks).
    // we are trying to avoid division operation in millis and micros. So instead of division we do shift and multiplication
    // micros = clocks*x>>16;
    static constexpr uint16_t MicrosToClockShift16 = uint16_t((1000000UL << 12)/(BaseClock >> (PrescalerLog2+4))); // 1MHz*65536/clocks
    static constexpr uint16_t MillisToClockShift16 = uint16_t((1000UL << 16)/(BaseClock >> PrescalerLog2)); // 1kHz*65536/clocks
    static constexpr uint16_t ClocksInMillis = (BaseClock >> PrescalerLog2)/1000U;

    static void InterruptHandler();


    static uint64_t _clocks() {
      Timer::SetPrescalerI(0); // stop timer to avoid interrupts or missed interrupts
      uint64_t Clocks = (uint64_t(Ticks) << ClocksInTickLog2) + *Timer::pCounter();
      Timer::SetPrescalerI(PrescalerI); // restart timer
      return Clocks;
    } // _clocks
    static uint32_t clocks() {
      Timer::SetPrescalerI(0); // stop timer to avoid interrupts or missed interrupts
      auto Clocks = (Ticks << ClocksInTickLog2) + *Timer::pCounter();
      Timer::SetPrescalerI(PrescalerI); // restart timer
      return Clocks;
    } // clocks
    static uint32_t ticks() { return Ticks; }
    static void delayClocks(uint32_t delay) { // delay < UINT32_MAX/2
      uint32_t Till = clocks() + delay;
      while(clocks()-Till > UINT32_MAX/2);
    } // delayClocks
    static void delayTicks(uint32_t delay) { // delay < UINT32_MAX/2
      uint32_t Till = ticks() + delay;
      while(ticks()-Till > UINT32_MAX/2);
    } // delayClocks
    //! @param delay - us
    static inline void delayMicros(uint16_t delay) {
      delayClocks(delay*(BaseClock >> PrescalerLog2)/1000000UL);
    } // delayClocks
    //! @param delay - ms
    static inline void delayMillis(uint16_t delay) {
      delayTicks(uint32_t(delay)*ClocksInMillis);
    } // delayClocks

    static void Init() { TimeCounter<Timer>::Setup(InterruptHandler, 0, PrescalerI); }

    SystemTimer() { Init(); } // so Init can be called outside of function before main
  }; // SystemTimer
}; // namespace avp

// template<class Timer> constexpr uint16_t SystemTimer<Timer>::ClocksInMillis = (1000UL << 8)/MicrosToClockShift16;
template<class Timer> volatile uint32_t SystemTimer<Timer>::Ticks;
//! timer is set to reset *Timer::pCounter() to 0 when it happens
template<class Timer> void SystemTimer<Timer>::InterruptHandler() { Ticks++; }

//! should be called in cpp file for each timer
#define INIT_TIMER_ISR(Timer) \
  ISR(COMB3(TIMER,TimerI,_COMPA_vect)) { Timer::InterruptHandler(); } \


//! following defile aliases "Time" class to the specified timer. This class maintains "system"
//! clock. "standard" functions millis() and micros() are defined as well, returning corresponding time marks
//! @tparam Timer is Timer? defined in HW_Timer.h
#define DEFINE_SYSTEM_TIMER(Timer) \
  typedef avp::SystemTimer<Timer> Time; \
  extern uint32_t millis(); \
  extern uint32_t micros();

//! initiates static Time class, should be called once in CPP file
//! @tparam TimerI - index of timer
#define INIT_SYSTEM_TIMER \
  Time __Time_Init__; \
  uint32_t millis() { return (Time::_clocks()*Time::MillisToClockShift16) >> 16;} \
  uint32_t micros() { return (Time::_clocks()*Time::MicrosToClockShift16) >> 16; }


#endif /* TIME_COUNTER_H_ */