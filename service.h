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
  
namespace avp {
  // ***** REGISTERS BIT HANDLING FUNCTIONS
  inline void set_high(volatile uint8_t &reg, uint8_t bitI) { reg |= 1 << bitI; }
  inline void set_low(volatile uint8_t &reg, uint8_t bitI) { reg &= ~(1 << bitI); }
  inline void setbit(volatile uint8_t &reg, uint8_t bitI, bool value) {
    value?set_high(reg,bitI):set_low(reg,bitI);
  } // setbit
  inline bool getbit(volatile uint8_t &reg,uint8_t bitI) {
    return (reg >> bitI) & 1;
  }
  inline void setbits(volatile uint8_t &reg, uint8_t lowest_bit, uint8_t numbits, uint8_t value) {
    uint8_t Mask = ((1 << numbits) - 1) << lowest_bit;
    reg = (reg | Mask) & (value << lowest_bit);
  }
} // avp  

#endif /* SERVICE_H_ */