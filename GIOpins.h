/*!
 * @file AVP_LIBS/AVR/GIOpins.h
 *
 * Created: 6/25/2014 2:38:27 PM
 *  Author: panasyuk
 */


#ifndef GIOPINS_H_
#define GIOPINS_H_

#include <avr/io.h>
#include <AVP_LIBS/General/BitBang.h>
#include <AVP_LIBS/General/Macros.h>

// the difference between peading PORT address and PIN address is that PORT address is
// reading port register which reflected in pins only when configured as output, PIN address reads pins

namespace avp {
  enum PinModes {INPUT, OUTPUT};
  enum PinLevels {LOW, HIGH};

  template<uint16_t DDRaddr, uint16_t PORTaddr, uint16_t PINaddr, char Name_> struct IO_Port {
    static volatile uint8_t &port_byte() { return *(volatile uint8_t *)PORTaddr; }
    static volatile uint8_t &pin_byte() { return *(volatile uint8_t *)PINaddr; }
    static volatile uint8_t &mode_byte() { return *(volatile uint8_t *)DDRaddr; }
    static void config(uint8_t IsOutput, uint8_t level) { mode_byte() = IsOutput; port_byte() = level; }
    static void setbits(uint8_t values, uint8_t mask) { avp::setbits(port_byte(),mask,values); }
    static void setbits(uint8_t values, uint8_t numbits, uint8_t lowest_bit = 0) {
      avp::setbits(port_byte(),values,numbits,lowest_bit);
    }
    static constexpr char Name() { return Name_; }
  };

  /// @tparam Port is of class IO_Port
  /// @tparam Number is 0-based pin number
  template<class Port, uint8_t Number> struct Pin {
    static constexpr char Name[] = {Port::Name,'0'+Number,0};
    /*! sets PULLUP when mode is INPUT */
    static void set_high() { avp::set_high(Port::port_byte(),Number); }
    static void set_low() { avp::set_low(Port::port_byte(),Number); }
    static void write(bool value) { avp::setbit(Port::mode_byte(),Number,value); }
    static bool read() { return avp::getbit(Port::read(), Number); }
    static void mode(bool IsOutput) { avp::setbit(Port::mode_byte(),Number,IsOutput); }
    static void config(bool IsOutput, bool level) { mode(IsOutput); write(level); }
    static void toggle() { avp::set_high(Port::pin_byte(),Number); /* just works this way, according to spec */   }
  }; // Pin

  template<class Port, uint8_t Number> constexpr char Pin<Port,Number>::Name[];
}; // avp

#define AVP_PORT(PortChar) avp::IO_Port<uint16_t(&COMB2(DDR,PortChar)),uint16_t(&COMB2(PORT,PortChar)),uint16_t(&COMB2(PIN,PortChar)),(#PortChar)[0]>
#define AVP_PIN(Port,Number) avp::Pin<AVP_PORT(Port),Number>

#endif /* GIOPINS_H_ */