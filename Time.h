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


#endif /* TIME_H_ */