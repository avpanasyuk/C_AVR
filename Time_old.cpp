/*
 * time.cpp
 *
 * Created: 8/1/2013 7:24:35 PM
 *  Author: panasyuk
 */
#include "Time.h"

// Ok, the previuos implementation (with Clear On compare) did not work because TCNT was clearing,
// but Millis was updated via interrupt later, and so we have a step backward if interrupts
// are busy
// So, in the new implementation we do not do CTC, we counting forward, but we do have
// interrupt on compare. When it happens we increase Millis and MANUALLY reset TCNT backwards
// at the same time.

namespace Time {
  volatile uint32_t Kibiticks;

  void Init() {
    // OK, we build time counter on 16-bits timer 3
    PRR1 &= ~(1<<PRTIM3); // turn on power on the counter
    OCR3A = KIBITICK_IN_CLKS; // we do interrupt when we count to about a  millisecond
    TIMSK3 |= (1<<OCIE3A); // enable compare interrupts
    TCCR3A = 0;
    TCCR3B = (1<<CS31); // Normal mode, we count top 0xFFFF, prescaler by 8
    sei();
  } // Init

  ISR(TIMER3_COMPA_vect) {
    while(TCNT3 > KIBITICK_IN_CLKS ) {
      TCNT3 -= KIBITICK_IN_CLKS;
      Kibiticks++;
    }
  } // ISR

  void delayTicks(uint32_t delay) { // delay < UINT32_MAX/2
    uint32_t Till = ticks() + delay;
    while(ticks()-Till > UINT32_MAX/2);
  } // delayTicks
  void delayKibiticks(uint32_t delay) { // delay < UINT32_MAX/2
    uint32_t Till = kibiticks() + delay;
    while(kibiticks()-Till > UINT32_MAX/2);
  } // delayTicks

}  // namespace Time