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
    //! @param Divider -  clock divider, should not be 0
    static void Setup(void (*pInterruptCallback)() = nullptr, typename Timer::CounterType Divider = Timer::GetMaxDivider(),
                      uint8_t PrescalerI = Timer::GetLastPrescalerI()) {
      Timer::pInterruptCallback = pInterruptCallback;
      Timer::Power(1);
      Timer::SetCountToValueA(Divider-1); // ticks=clocks/(1+CountToValue)
      Timer::SetCompareOutputMode(0); // do not toggle output pin
      Timer::InitCTC();
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
  struct SystemTimer: public TimeCounter<Timer> {
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

    static constexpr uint32_t MinTickFreq = 1000UL; // TICKS is the frequency interrupt is triggered
    // we do not want to run interrupt with frequency too high. We want them to be faster but as close as possible to
    // MinTickFreq
    static constexpr uint32_t MinClockFreq = 1000000UL; // CLOCK is the frequency counter is counting
    // we want clocks to have better resolution than 1 microsecond
    static constexpr uint8_t PrescalerI = Timer::GetPrescalerIndex(MinClockFreq); // prescaler acts on clocks
    static constexpr uint8_t PrescalerLog2 = Timer::GetLog2Prescaler(PrescalerI);
    // Ok, the problem is that Prescaler values are not always consequent powers of 2, there may be gaps. So Clocks may be more then
    // twice faster then  MinClockFreq. But we still want to have interrupt Freq as close to  MinTickFreq as possible
    // so TickDividerLog2 is not necessarily 10
    static constexpr uint8_t TickDividerLog2 = avp::log2(BaseClock/MinTickFreq) - PrescalerLog2; // log2(clocks/ticks)
    static constexpr uint8_t MicrosToClockTimes256 = uint8_t((1000000UL << 8)/(BaseClock >> PrescalerLog2)); // 1MHz*256/clocks
    static constexpr uint8_t MillisToTickTimes256 = uint8_t((1000UL << (8 + TickDividerLog2))/(BaseClock >> PrescalerLog2)); // 1kHz*256/ticks
    static volatile uint32_t Ticks; // something close to a millisecond, but may be much smaller if proper prescaler is missing

    static void InterruptHandler();

    static inline uint32_t _clocks() {
      return (Ticks << TickDividerLog2) + *Timer::pCounter();
    }
    static uint32_t clocks() {
      Timer::SetPrescalerI(0); // stop timer to avoid interrupts or missed interrupts
      auto Clocks = _clocks();
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
    //! divide operation user here takes a lot of CPU cycles, so it is better to use them as constexpr
    static constexpr uint32_t MillisToTicks(uint32_t ms) { return (ms << 8)/MillisToTickTimes256; }
    static constexpr uint32_t MicrosToClocks(uint32_t us) { return (us << 8)/MicrosToClockTimes256; }
    //! @param delay - us
    static inline void delayMicros(uint16_t delay) {
      delayClocks((delay*(BaseClock >> 10)) >> (PrescalerLog2 + 10));
    } // delayClocks
    //! @param delay - ms
    static inline void delayMillis(uint16_t delay) {
      delayTicks((delay*(BaseClock >> 10)) >> (PrescalerLog2 + TickDividerLog2));
    } // delayClocks

    static void Init() { TimeCounter<Timer>::Setup(InterruptHandler, 1U << TickDividerLog2, PrescalerI); }

    SystemTimer() { Init(); } // so Init can be called outside of function before main
  }; // SystemTimer
}; // namespace avp

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
  uint32_t millis() { return (Time::ticks()*Time::MillisToTickTimes256) >> 8; } \
  uint32_t micros() { return (Time::clocks()*Time::MicrosToClockTimes256) >> 8; }


#endif /* TIME_COUNTER_H_ */