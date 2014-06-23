/*
 * Timer.h
 *
 * Created: 6/14/2014 8:27:48 PM
 *  Author: panasyuk
 */ 


#ifndef TIMER_H_
#define TIMER_H_

//! a set of hardware timer classes to use in SqrWave template. Each class constains hardware information how to program given timer
template<typename CounterType, volatile CounterType *pTCNT> struct Timer {
	/*! Ok, how do we select a prescaler? We need both micro and millisecond scales, so we need 16-bit timer/counter,
	 * because if it counts faster then microseconds it we roll over before millisecond comes. So, the only consideration is that
	 * it counts faster then microsecond.
	 * One consideration about this class is that it has to be fast, so we can not do divisions because AVR processor does
	 * not have division and it takes forever. We use only shift instead of divisions. So we can not get micro and millisecond PRECISLEY,
	 * instead we will have "tick" which is as close as possible but smaller then microsecond and "kibitick" = 2^10*"tick.  And
	 * we have "clock" which is what we really running timer at, because prescaler setting are not every power of 2, so "tick" may be = 2^?*"clock"
	 */
	static constexpr Log2Divider = avp::log2(F_CPU/1000000UL);

	/*! Looking for a maximum prescaler which is still less then divider,  because prescaler setting are not every power of 2*/  
	static constexpr uint8_t FindPrescalerI(uint8_t CurI=0) {
		return CurI == N_ELEMENTS(Timer::Prescaler) || Timer::Prescaler[CurI] > Log2Divider?CurI-1:FindPrescalerI(CurI+1);
	}
	static constexpr uint8_t Log2ClocksInTick = Timer::Prescaler[FindPrescalerI()] - Log2Divider;
	/*! Ok, so our tick is a microsecond or smaller, so what is it in nanoseconds */
	static constexpr uint16_t NanosecondsInTick = 1000000000UL/(F_CPU >> Timer::Prescaler[FindPrescalerI()]);
	static constexpr uint16_t TicksInKibitick = avp::RoundRatio(1000000UL,NanosecondsInTick); // almost always 2^10
	static constexpr uint16_t MicrosecondsInKibitick = uint32_t(TicksInKibitick)*NanosecondsInTick/1000UL;
	

	public:
	static constexpr uint8_t Prescaler
	static constexpr uint8_t Width = sizeof(CounterType) << 3; // width in bits
	typedef struct {
		CounterType CountTo; // divider = CountTo + 1
		uint8_t PrescalerInd; //! prescaler index, first active is 1
	} Params;
}; // Timer8bit

#endif /* TIMER_H_ */

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
