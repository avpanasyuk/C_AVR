/*
 * service.c
 *
 * Created: 7/11/2013 9:41:51 PM
 *  Author: panasyuk
 */

#include <stdlib.h>
#include <AVP_LIBS/General/Macros.h>
#include <AVP_LIBS/General/Error.h>
#include <AVP_LIBS/General/General.h>
#include <AVP_LIBS/AVR/service.h>
// #include <AVP_LIBS/AVR/Time.h>

void * operator new(size_t n) {
  void * const p = malloc(n);
  if(p == NULL) major_fail(1);
  return p;
}

void operator delete(void * p) {// or delete(void *, std::size_t)
  free(p);
}

IGNORE(-Wreturn-local-addr)

extern "C" {
  int freeRam () {
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
  }

  void __cxa_pure_virtual() {
  }
  void _pure_virtual() {
  }
  void __pure_virtual() {
  }

  int atexit( void (*func)(void)) {return -1;}
  int __cxa_atexit() {return -1;}

  // we use watchdog for reboot and have to disable watchdog on boot
  // void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));
  // void wdt_init(void) { MCUSR = 0; wdt_disable(); }
};

static struct DoOnStartup {
  DoOnStartup() {
    MCUSR = 0; wdt_disable();
  }
} _ ;

void avp::hang_cpu() { cli(); volatile uint8_t stop=1; while(stop); }
void avp::soft_reset() { wdt_enable(WDTO_120MS);  hang_cpu(); } // wdt_disable() should be called immediately after restart. It is done in servuce.cpp



