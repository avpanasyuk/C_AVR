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

void * operator new(size_t n) {
  void * const p = malloc(n);
  if(p == NULL) avp::major_fail(1);
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
};

