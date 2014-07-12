/*!
 * @file AVP_LIBS/AVR/GIOpins.h
 *
 * Created: 6/25/2014 2:38:27 PM
 *  Author: panasyuk
 */


#ifndef GIOPINS_H_
#define GIOPINS_H_

#include <avr/io.h>
#include <AVP_LIBS/AVR/General.h>

namespace avp {
  enum PinModes {INPUT, OUTPUT};
  enum PinLevels {LOW, HIGH};

  template<uint16_t DDRaddr, uint16_t PORTaddr, uint16_t PINaddr, uint8_t Number, char Port> struct Pin {
    static constexpr char Name[] = {Port,'0'+Number,0};
    /*! sets PULLUP when mode is INPUT */
    static void write(bool value) { avp::setbit(*(volatile uint8_t *)PORTaddr,Number,value );  }
    static void set_high() { avp::set_high(*(volatile uint8_t *)PORTaddr,Number); }
    static void set_low() { avp::set_low(*(volatile uint8_t *)PORTaddr,Number); }
    static bool read() { return avp::getbit(*(volatile uint8_t *)PINaddr,Number); }
    static void mode(bool IsOutput) { avp::setbit(*(volatile uint8_t *)DDRaddr,Number,IsOutput); }
    static void config(bool IsOutput, bool level) { mode(IsOutput); write(level); }
    static void toggle() { avp::toggle(*(volatile uint8_t *)PORTaddr,Number);  }
  }; // Pin

  template<uint16_t DDRaddr, uint16_t PORTaddr, uint16_t PINaddr, uint8_t Number, char Port>
  constexpr char Pin<DDRaddr,PORTaddr,PINaddr,Number,Port>::Name[];
}; // avp

#define AVP_PIN(Port,Number) \
  avp::Pin<uint16_t(&_COMB2(DDR,Port)),uint16_t(&_COMB2(PORT,Port)),uint16_t(&_COMB2(PIN,Port)),Number,(#Port)[0]>

#endif /* GIOPINS_H_ */