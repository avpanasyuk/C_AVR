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
#include <AVP_LIBS/General/General.h>

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

class ISR_Blocker {
  uint8_t oldSREG;
public:
  ISR_Blocker(): oldSREG(SREG) { cli(); }
  ~ISR_Blocker() { SREG = oldSREG; }
}; // ISR_Blocker

static inline void hang_cpu() { cli(); volatile uint8_t stop=1; while(stop); }

#endif /* SERVICE_H_ */