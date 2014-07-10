/*
 * GIOpins.h
 *
 * Created: 6/25/2014 2:38:27 PM
 *  Author: panasyuk
 */


#ifndef GIOPINS_H_
#define GIOPINS_H_

#include <avr/io.h>
#include <AVP_LIBS/General/General.h>

namespace avp {
  template<uint16_t DDRaddr, uint16_t PORTaddr, uint16_t PINaddr, uint8_t Number> struct Pin {
    /*! sets PULLUP when mode is INPUT */
    static void write(bool value) { setbit(*(uint8_t *)PORTaddr,Number,value );  }
    static void set_high() { avp::set_high(*(uint8_t *)PORTaddr,Number); }
    static void set_low() { avp::set_low(*(uint8_t *)PORTaddr,Number); }
    static bool read() { return getbit(*(uint8_t *)PINaddr,Number); }
    static void mode(bool IsOutput) { setbit(*(uint8_t *)DDRaddr,Number,IsOutput); }
    static void config(bool IsOutput, bool level) { mode(IsOutput); write(level); }
  }; // Pin
} // avp

// using
// AVP_PIN(B,5) LED_PIN;
// LED_PIN::config(1,0);
// .........
// LED_PIN::write(0);

#define AVP_PIN(Port,Number) \
  struct avp::Pin<uint16_t(&_COMB2(DDR,Port)),uint16_t(&_COMB2(PORT,Port)),uint16_t(&_COMB2(PIN,Port)),Number>

#endif /* GIOPINS_H_ */