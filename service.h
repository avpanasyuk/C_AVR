/*
 * service.h
 *
 * Created: 7/11/2013 9:59:01 PM
 *  Author: panasyuk
 */


#ifndef SERVICE_H_
#define SERVICE_H_

#include <stdlib.h>
#include <stdint.h>
#include <avr/interrupt.h.>
#include <General.h>

extern "C"  int freeRam ();
extern void * operator new(size_t n);
extern void operator delete(void * p);
extern Fail::function malloc_failed_func;

extern "C" void __cxa_pure_virtual();
extern "C" void _pure_virtual(void);
extern "C" void __pure_virtual(void);
extern "C" int atexit( void (*func)(void));
extern "C" int __cxa_atexit();

/** nop for timing */
#define NOP asm volatile ("nop\n\t")

extern "C" int32_t SetRectWave(uint8_t TimerI, uint32_t Freq); // from SqrWaveGen
// returns -1 if frequency can not be generated, or true frequency otherwise

class ISR_Blocker {
  uint8_t oldSREG;
public:
  ISR_Blocker(): oldSREG(SREG) { cli(); }
  ~ISR_Blocker() { SREG = oldSREG; }
}; // ISR_Blocker

static inline void hang_cpu() { cli(); volatile uint8_t stop=1; while(stop); }

template<typename out_type, typename in_type> out_type Sqrt(in_type y) {
  in_type x = 1, old_x, y_=y;
  while(y_>>=2) x <<= 1; // rough estimate
  do {
    old_x = x;
    x = (old_x+y/old_x)>>1;
  } while (x != old_x && x + 1 != old_x);
  return x;
} //Sqrt



#endif /* SERVICE_H_ */