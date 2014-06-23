/*
* Time.h
*
* Created: 8/1/2013 7:22:28 PM
*  Author: panasyuk
* function dealing with time
*/


#ifndef TIME_H_
#define TIME_H_

#include <avr/interrupt.h>
#include <stdint.h>
#include <General.h>
#include "service.h"

// Instead of millis and micros we use ticks and kibiticks
// we use ticks, with kibitick ~ KibitickInUSecs usec, ticks =  KibitickInUSecs/(1<<KIBI_PWR2) usec

// So, what we are counting to? to a millisecond. The smallest step we care about is microsecond,
// so we can put prescaler to 8, and our CLK is F_CPU/8. so, in a milli
#define PRESCALER 8U
#define PWR2 LOG2(F_CPU/PRESCALER/1000U) // 11
#define KIBI_PWR2 10U // there are 2^10 = 1024 ticks in kibitick
#define KIBITICK_IN_CLKS (1U << PWR2)

namespace Time {
	static constexpr uint16_t KibitickInUSecs = KIBITICK_IN_CLKS*PRESCALER*1000UL/(F_CPU/1000U);
	// to avoid divisions it is really about 819 us instead.
	extern volatile uint32_t Kibiticks; // we have to put it here so we can inline "micros"
	// to avoid multiplication and division be decimal number we have to implement stuff
	// by shifts, so kibiticks are not quite milliseconds, but MicrosInKibitick of microseconds;
	extern void Init();

	static inline uint32_t _ticks() { return (Kibiticks << KIBI_PWR2) + (TCNT3 >> (PWR2-KIBI_PWR2)); }
	static inline uint32_t ticks() { ISR_Blocker Auto; return _ticks(); }
	static inline uint32_t kibiticks() { return Kibiticks; }
	void delayTicks(uint32_t delay); // delay < UINT32_MAX/2
	void delayKibiticks(uint32_t delay); // delay < UINT32_MAX/2

	//! @brief class for timeouts. Create, e.g., as automatic variable and check bool value
	//! to see if triggered
	//!
	//! @tparam T should be unsigned!
	template<class T=uint32_t> class Out {
		const T Expires;
		public:
		Out(T Timeout /*!< time in ticks */):Expires(ticks() + Timeout) {}
		operator bool() { return (T(ticks()) - Expires) < ((~T(0))/2); }
	}; // Out

	template<class T=uint32_t> class KibiOut {
		const T Expires;
		public:
		KibiOut(T Timeout):Expires(kibiticks() + Timeout) {}
		operator bool() { return (T(kibiticks()) - Expires) < ((~T(0))/2); }
	}; // KibiOut
} // namespace Time

template<class Timer> struct TimeFunctions {
	/*! Ok, how do we select a prescaler? We need both micro and millisecond information, so we need 16-bit timer/counter,
	 * because if it counts faster then microseconds it we roll over before millisecond comes. So, the only consideration is that
	 * it counts faster then microsecond.
	 */
	static constexpr Divider = avp::RoundLog2Ratio(F_CPU,1000000UL);
	/*! One consideration about this class is that it has to be fast, so we can not do divisions because AVR processor does
	 * not have division and it takes forever. We use only shift instead of divisions. So lets find a power of 2 (i) we have to divide 
	 * F_CPU on to get period closest to a microsecond. It means that i is closest to log2(F_CPU/1000000) 
	 */
	
	/*! Looking for a maximum prescaler which is still less then divider */  
	static constexpr uint8_t FindPrescalerI(uint8_t CurI=0) {
		return CurI == N_ELEMENTS(Timer::Prescaler) || Timer::Prescaler[CurI] > Divider?CurI-1:FindPrescalerI(CurI+1);
	}
	/*! Because prescaler setting are not every power of 2  
	static constexpr uint8_t PWR2 = avp::log2(avp::RoundRatio(F_CPU,(1000UL << FindPrescalerI())));
	static constexpr uint8_t KIBI_PWR2 = 10U; // there are 2^10 = 1024 ticks in kibitick
	static constexpr uint8_t KIBITICK_IN_CLKS = 1U << PWR2;
	
	void Init() {
		Timer
	}
	
	private:
	static volatile uint32_t Kibiticks;
	
	
	
	
};

#endif /* TIME_H_ */