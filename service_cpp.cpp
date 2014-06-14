/*
* service_cpp.cpp
*
* Created: 8/5/2013 10:34:14 PM
*  Author: panasyuk
*/

#include <stdlib.h>
#include <General.h>

Fail::function malloc_failed_func = Fail::default_function;

void * operator new(size_t n) {
  void * const p = malloc(n);
  if(p == NULL) (*malloc_failed_func)();
  return p;
}

void operator delete(void * p) {// or delete(void *, std::size_t)
  free(p);
}

