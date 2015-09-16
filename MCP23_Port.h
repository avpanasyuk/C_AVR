/*
 * MCP23_Port.h
 *
 * Created: 9/15/2015 7:43:26 PM
 *  Author: panasyuk
 */


#ifndef MCP23_PORT_H_
#define MCP23_PORT_H_

#include "MCP23017.h"
#include "MCP23S18.h"

namespace avp {
  //! @tparam MCP23_chip = subclass of MCP23017 or MCP23S18 template
  //! @tparam IsA - MCP23xxx has A and B ports, which one is it
  template<class MCP23_chip, bool IsB>
  class MCP23_Port {
    static uint8_t Bits; // stores current port value, so we can change individual pins
    static constexpr uint8_t Offset = IsB?0x10:0x0; // register offset

  public:
    static void SetDir(uint8_t InputMap) { MCP23_chip::SetRegister(MCP23_chip::IODIR|Offset, InputMap); }
    static void SetPullUp(uint8_t PullUpMap) {  MCP23_chip::SetRegister(MCP23_chip::GPPU|Offset, PullUpMap); }

    //! @param Mask uint8_t - specifies which pins to change (default all pins)
    static void Set(uint8_t Bits_, uint8_t Mask = 0xFF) {
      MCP23_chip::SetRegister(MCP23_chip::GPIO|Offset, Bits = (Bits | Mask) & (Bits_ | ~Mask));
    } // Set

    //! @param bit - bit number
    static void SetBit(uint8_t bit, bool value) {
      Set(value?(1 << bit):0,1 << bit);
    } // SetBit
  }; // MCP23_Port


  template<class MCP23_Port, uint8_t Bit>
  struct MCP23_Bit { static void Set(bool value) { MCP23_Port::SetBit(Bit,value); }};

  template<class MCP23_chip, bool IsB> uint8_t MCP23_Port<MCP23_chip,IsB>::Bits = 0;
  // template<class MCP23_chip, bool IsA> constexpr uint8_t MCP23_Port<MCP23_chip, IsA>::Offset;
} // avp


#endif /* MCP23_PORT_H_ */