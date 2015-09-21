#ifndef  MCP23017_H_
#define  MCP23017_H_

#include <stdint.h>
#include <AVP_LIBS/General/Macros.h>
#include <AVP_LIBS/AVR/GIOpins.h>
#include "TWI_I2C.h"

namespace avp {
  //! @tparam I2C - class providing I2C protocol, like TWI_I2C
  //! which provides Init(), send(uint8_t) and receive(uint8_t *ptr, uint16_t *size) functions
  //! @tparam AddressPins - address, set by A0-2 pins
  template<class I2C, uint8_t AddressPins>
  class MCP23017: public I2C_slave<I2C,AddressPins + 0x20> {
    static bool Inited;
  public:
    enum RegIndex_ {IODIR, IPOL, GPINTEN, DEFVAL, INTCON, IOCON, GPPU, INTF, INTCAP, GPIO, OLAT};
    enum {INTCC, INTPOL, ODR, SEQOP=5, MIRROR, BANK };

    static inline void SetRegister(uint8_t Index, uint8_t Value) {
      AVP_ASSERT(Inited);
      uint8_t ToSend[] = {Index,Value};
      I2C_slave<I2C,AddressPins + 0x20>::send(ToSend,2);
    } // SetRegister

    //static void ReadAllRegs() {
    //uint8_t _; uint16_t __;
    //for(uint8_t j=0; j<0x20; j+=0x10) { // runs twice (for ports A and B)
    //CommWrapper Auto;
    //I2C::send(uint8_t(0x40|1|(AddressPins << 1)) /* read */);
    //I2C::send(uint8_t(j)); // starting with zero register
    //for(uint8_t i=0; i<10; i++)  I2C::receive(&_,&__); // use login analyser to see
    //}
    //} // ReadAllRegs

    static void Init() {
      if(!Inited) {  // to init only once
        Inited = true;
        I2C::Init();
        // it is freaking complicated chip
        // Using Byte mode, so we are not automatically increment anything
        // Using segregated register map
        SetRegister(0x0B, 1<<BANK); // there is a problem with setting IOCON initially. The address depends
        // on BANK value. On power-up it is 0, so register map is not segregated and
        // IOCON is 0x0A and 0x0B (same register), but after CPU reset BANK=1
        // and IOCON is 0x05 and 0x15. So, let's do it twice
        SetRegister(0x15, 1<<BANK); // registers are segregated
      }
    } // Init
  }; // MCP23017

  template<class I2C, uint8_t AddressPins> bool MCP23017<I2C,AddressPins>::Inited = false;
} // avp

#endif /*  MCP23017_H_ */