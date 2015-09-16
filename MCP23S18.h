#ifndef  MCP23S18_H_
#define  MCP23S18_H_

#include <stdint.h>
#include <AVP_LIBS/General/Macros.h>
#include <AVP_LIBS/AVR/GIOpins.h>
#include "HardSPI.h"

namespace avp {
  class NullClass {};
  //! @tparam SPI - communication class, like SPI from HardSPI or I2C_master or HW_UART
  //! which provides Init(), send(uint8_t) and receive(uint8_t *ptr, uint16_t *size) functions
  //! @tparam nCS_pin - AVP_PIN nCS line is connected to
  template<class SPI, class nCS_pin> class MCP23S18 {
    enum {IODIR, IPOL, GPINTEN, DEFVAL, INTCON, IOCON, GPPU, INTF, INTCAP, GPIO, OLAT};
    enum {INTCC, INTPOL, ODR, SEQOP=5, MIRROR, BANK };

    IGNORE(-Wunused-variable)

    static inline void Set(uint8_t Register, uint8_t Value) {
      SPI_Wrapper<SPI,nCS_pin> Auto;
      SPI::send(uint8_t(0x40) /* write */);
      SPI::send(Register);
      SPI::send(Value);
    } // Set

    static void ReadAllRegs() {
      uint8_t _; uint16_t __;
      for(uint8_t j=0; j<0x20; j+=0x10) { // runs twice (for ports A and B)
        SPI_Wrapper<SPI,nCS_pin> Auto;
        SPI::send(uint8_t(0x40|1) /* read */);
        SPI::send(uint8_t(j)); // starting with zero register
        for(uint8_t i=0; i<10; i++)  SPI::receive(&_,&__); // use login analyser to see
      }
    } // ReadAllRegs

    STOP_IGNORING

  public:
    static void Init() {
      SPI::Init();
      // it is freaking complicated chip
      // Using Byte mode, so we are not automatically increment anything
      Set(0x0B, 1<<BANK); // there is a problem with setting IOCON initially. The address depends
      // on BANK value. On power-up it is 0, so IOCON is 0x0A and 0x0B (same register), but after CPU reset BANK=1
      // and IOCON is 0x05 and 0x15. So, let's do it twice
      Set(0x15, 1<<BANK); // registers are segregated, with can R/W registers in sequence,

      Set(IODIR, (1<<7)|(1<<6)); // All PortA pins are output, except of bits 6 & 7
      Set(IODIR|0x10, 0); // All PortB pins are output
      Set(GPPU, 0xFF);
      Set(GPPU|0x10, 0xFF);
      // reset ports to known state, so MCP23_Port::Bits on boot are correct
      SetPort(0,0x0);
      SetPort(1,0x0);

      // ReadAllRegs(); // use logic analyser to see that all values are correct

      // everything else is fine in default setting
    } // Init

    static inline void SetPort(bool IsA, uint8_t Bits) { Set(GPIO|(IsA?0:0x10), Bits); }
  }; // MCP23S18
} // avp

#endif /*  MCP23S18_H_ */