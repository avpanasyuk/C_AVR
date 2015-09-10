#ifndef  MCP23xxx_H_
#define  MCP23xxx_H_

#include <stdint.h>
#include <AVP_LIBS/AVR/GIOpins.h>
#include "HardSPI.h"
#include "I2C.h"

namespace avp {
  class NullClass {};
  //! @tparam Comm - communication class, like SPI from HardSPI or I2C_master or HW_UART
  //! which provides transfer functions
  //! @tparam CommWrapper - class which has in constructor actions for transfer preparation, in
  //!         destructor - transfer cancellation
  template<class Comm, class CommWrapper = NullClass> class MCP23xxx {
    static uint8_t PortA, PortB; // stores current port values, so we can change individual pins

    enum {IODIR, IPOL, GPINTEN, DEFVAL, INTCON, IOCON, GPPU, INTF, INTCAP, GPIO, OLAT};
    enum {INTCC, INTPOL, ODR, SEQOP=5, MIRROR, BANK };

    static inline void Set(uint8_t Register, uint8_t Value) {
      CommWrapper Auto;
      Comm::send(uint8_t(0x40) /* write */);
      Comm::send(Register);
      Comm::send(Value);
    } // Set

    static void ReadAllRegs() {
      for(uint8_t j=0; j<0x20; j+=0x10) { // runs twice (for ports A and B)
        CommWrapper Auto;
        Comm::send(uint8_t(0x40|1) /* read */);
        Comm::send(uint8_t(j)); // starting with zero register
        for(uint8_t i=0; i<10; i++)  Comm::receive(); // use login analyser to see
      }
    } // ReadAllRegs

    void Init() {
      // it is freaking complicated chip
      // Using Byte mode, so we are not automatically increment anything
      CommWrapper::Init();
      Set(0x0B, 1<<BANK); // there is a problem with setting IOCON initially. The address depends
      // on BANK value. On power-up it is 0, so IOCON is 0x0A and 0x0B (same register), but after CPU reset BANK=1
      // and IOCON is 0x05 and 0x15. So, let's do it twice
      Set(0x15, 1<<BANK); // registers are segregated, with can R/W registers in sequence,

      Set(IODIR, (1<<7)|(1<<6)); // All PortA pins are output, except of bits 6 & 7
      Set(IODIR|0x10, 0); // All PortB pins are output
      Set(GPPU, 0xFF);
      Set(GPPU|0x10, 0xFF);
      // reset ports to known state
      SetPortA(0xFF,0x0);
      SetPortB(0xFF,0x0);

      ReadAllRegs(); // use logic analyser to see that all values are correct

      // everything else is fine in default setting
    } // Init

    // Mask specifies which pins to change
    void SetPortA(uint8_t Mask, uint8_t State) {
      Set(GPIO, PortA = (PortA | Mask) & (State | ~Mask));
    } // SetPortBA

    void SetPortB(uint8_t Mask, uint8_t State) {
      Set(GPIO|0x10, PortB = (PortB | Mask) & (State | ~Mask));
    } // SetPortB
  } // MCP23xxx

  template<class SPI, class nCS_pin> class MCP23S18 : public MCP23xxx<SPI,SPI::tWrapper<nCS_pin,0> {
  }; // MCP23S18
  
  template<uint32_t ClkF, uint8_t SlaveAddress> class MCP23017 : public MCP23xxx<I2C_master<ClkF,SlaveAddress> > {
  }; // MCP23017
} // avp


#endif /*  MCP23xxx_H_ */