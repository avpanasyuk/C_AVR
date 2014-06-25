/*
* Time.h
*
* Created: 8/1/2013 7:22:28 PM
*  Author: panasyuk
* function dealing with time
* including file should include a header file with HW Timer classes first
* so, use ia like this:
-------------- in header file ---------------
#include "ATmega88A_Timers.h"
#include "Time.h"

DEFINE_TIME(1) 
-------------- in source file ----------------
INIT_TIME(1)
*/


#ifndef TIME_H_
#define TIME_H_

#include <General.h>
#include "service.h"

template<class _Timer> struct TimeCounter {
	/*! Ok, how do we select a prescaler? We need both micro and millisecond scales, so we need 16-bit timer/counter,
	* because if it counts faster then microseconds it we roll over before millisecond comes. So, the only consideration is that
	* it counts faster then microsecond.
	* One consideration about this class is that it has to be fast, so we can not do divisions because AVR processor does
	* not have division and it takes forever. We use only shift instead of divisions. So we can not get micro and millisecond PRECISLEY,
	* instead we will have "tick" which is as close as possible but smaller then microsecond and "kibitick" = 2^10*"tick.  And
	* we have "clock" which is what we really running timer at, because prescaler setting are not every power of 2, so "tick" may be = 2^?*"clock"
	*/
	static constexpr uint8_t Log2Divider = avp::log2(F_CPU/1000000UL);

	/*! Looking for a maximum prescaler which is still less then divider,  because prescaler setting are not every power of 2*/
	static constexpr uint8_t FindPrescalerI(uint8_t CurI=0) {
		return CurI == N_ELEMENTS(_Timer::Prescalers) || _Timer::Prescalers[CurI] > Log2Divider?CurI-1:FindPrescalerI(CurI+1);
	}
	static constexpr uint8_t Log2ClocksInTick = _Timer::Prescalers[FindPrescalerI()] - Log2Divider;
	/*! Ok, so our tick is a microsecond or smaller, so what is it in nanoseconds */
	static constexpr uint16_t NanosecondsInTick = 1000000000UL/(F_CPU >> _Timer::Prescalers[FindPrescalerI()]);
	static constexpr uint16_t Log2TicksInKibitick = avp::RoundLog2Ratio(1000000UL,NanosecondsInTick); // almost always 10
	static constexpr uint16_t MicrosecondsInKibitick = (NanosecondsInTick << Log2TicksInKibitick)/1000UL;
	static constexpr uint16_t ClocksInKibitick = 1U << (Log2ClocksInTick+Log2TicksInKibitick);
	static volatile uint32_t Kibiticks;  // something close to a millisecond, may be up to 40% off
	
	static void InterruptHandler() {
		while(*_Timer::pTCNT() > ClocksInKibitick) {
			*_Timer::pTCNT() -= ClocksInKibitick;
			Kibiticks++;
		}
	} //  InterruptHandler
	
	static void Init() { _Timer::InitTimeCounter(ClocksInKibitick, FindPrescalerI()); }

	static uint32_t _ticks() { return (Kibiticks << Log2TicksInKibitick) + (*_Timer::pTCNT() >> Log2ClocksInTick); }
	static uint32_t ticks() { ISR_Blocker Auto; return _ticks(); }
	static uint32_t kibiticks() { return Kibiticks; }
	void delayTicks(uint32_t delay) { // delay < UINT32_MAX/2
		uint32_t Till = ticks() + delay;
		while(ticks()-Till > UINT32_MAX/2);
	} // delayTicks
	void delayKibiticks(uint32_t delay) { // delay < UINT32_MAX/2
		uint32_t Till = kibiticks() + delay;
		while(kibiticks()-Till > UINT32_MAX/2);
	} // delayTicks	
}; // TimeCounter

#define DEFINE_TIME(TimerI) typedef TimeCounter<_COMB2(Timer,TimerI)> Time; \
	static inline uint32_t millis() { return Time::kibiticks(); } \
	static inline uint32_t micros() { return Time::ticks(); }

 // initiates static Time class, should be called once in CPP file
#define INIT_TIME(TimerI) \
ISR(_COMB(TIMER,TimerI,_COMPA_vect)) { Time::InterruptHandler(); } \
template<> volatile uint32_t Time::Kibiticks = 0; 	

//! @tparam T should be unsigned!
template<uint32_t (*TickFunction)(), typename T=uint32_t> class TimeOut {
	const T Expires;
	public:
	TimeOut(T Timeout):Expires(TickFunction() + Timeout) {}
	operator bool() { return (T(TickFunction()) - Expires) < ((~T(0))/2); }
}; // Out


#endif /* TIME_H_ */