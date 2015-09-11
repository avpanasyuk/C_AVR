#ifndef  MCP23xxx_H_
#define  MCP23xxx_H_

#include <stdint.h>
#include <AVP_LIBS/General/Macros.h>
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
    enum {IODIR, IPOL, GPINTEN, DEFVAL, INTCON, IOCON, GPPU, INTF, INTCAP, GPIO, OLAT};
    enum {INTCC, INTPOL, ODR, SEQOP=5, MIRROR, BANK };

    IGNORE(-Wunused-variable)

    static inline void Set(uint8_t Register, uint8_t Value) {
      CommWrapper Auto;
      Comm::send(uint8_t(0x40) /* write */);
      Comm::send(Register);
      Comm::send(Value);
    } // Set

    static void ReadAllRegs() {
      uint8_t _; uint16_t __;
      for(uint8_t j=0; j<0x20; j+=0x10) { // runs twice (for ports A and B)
        CommWrapper Auto;
        Comm::send(uint8_t(0x40|1) /* read */);
        Comm::send(uint8_t(j)); // starting with zero register
        for(uint8_t i=0; i<10; i++)  Comm::receive(&_,&__); // use login analyser to see
      }
    } // ReadAllRegs

    STOP_IGNORING

  public:
    static void Init() {
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

      ReadAllRegs(); // use logic analyser to see that all values are correct

      // everything else is fine in default setting
    } // Init

    static inline void SetPort(bool IsA, uint8_t Bits) { Set(GPIO|(IsA?0:0x10), Bits); }
  }; // MCP23xxx

  template<class SPI, class nCS_pin> class MCP23S18 : public MCP23xxx<SPI, SPI_Wrapper<SPI,nCS_pin>> {
  }; // MCP23S18

  template<uint32_t ClkF, uint8_t SlaveAddress> class MCP23017 : public MCP23xxx<I2C_master<ClkF,SlaveAddress>> {
  }; // MCP23017

  //! @tparam MCP23_chip = subclass of MCP23xxx template
  //! @tparam IsA - MCP23xxx has A and B ports, which one is it
  template<class MCP23_chip, bool IsA> class MCP23_Port {
    static uint8_t Bits; // stores current port value, so we can change individual pins

  public:
    //! @param Mask uint8_t - specifies which pins to change (default all pins)
    static inline void Set(uint8_t Bits_, uint8_t Mask = 0xFF) {
      MCP23_chip::SetPort(IsA, Bits = (Bits | Mask) & (Bits_ | ~Mask));
    } // Set

    //! @param bit - bit number
    static inline void SetBit(uint8_t bit, bool value) {
      Set(value?(1 << bit):0,1 << bit);
    } // SetBit
  }; // MCP23_Port

  template<class MCP23_chip, bool IsA> uint8_t MCP23_Port<MCP23_chip,IsA>::Bits = 0;

  template<class MCP23_Port, uint8_t Bit> class MCP23_Bit {
  public:
    static void Set(bool value) { MCP23_Port::SetBit(Bit,value); }
  };
} // avp

#endif /*  MCP23xxx_H_ */