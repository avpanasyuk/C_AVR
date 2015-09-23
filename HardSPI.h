/*
 * HardSPI.h
 * Hardware SPI from AVR. Currently only for byte by byte Transfer, no Receive
 *
 * Created: 8/1/2013 1:17:11 PM
 *  Author: panasyuk
 */


#ifndef HARDSPI_H_
#define HARDSPI_H_

#include <stdint.h>
#include "service.h"
#include "GIOpins.h"

#ifndef PRR0
#define PRR0 PRR
#endif

namespace avp {
  /// @tparam nSS - AVP_PIN class, this pin is doing nothing except selecting MASTER mode when configured as OUTPUT
  /// @tparam CLK - AVP_PIN class
  /// @tparam MOSI - AVP_PIN class
  /// @tparam MISO - AVP_PIN class
  /// @tparam Log2_Prescaler, from 1 to 7,  determines SPI frequency = F_CPU/2^Log2_Prescaler
  template<class nSS, class CLK, class MOSI, class MISO, uint8_t Log2_Prescaler=2>
  class tSPImaster {
  public:
    static void SetMode(uint8_t Mode) { SPCR &= ~(3<<CPHA); SPCR |= Mode<<CPHA; }
    static void SetFDIV(uint8_t FDIV) { SPCR &= ~3; SPCR |= FDIV; }
    static void BitOrder(bool MSBfirst) { SPCR &= ~(1<<DORD); SPCR |= MSBfirst?0:(1<<DORD); }
    static void Init() {
      nSS::mode(OUTPUT); // and should be kept OUTPUT, otherwise switches to SLAVE  mode
      CLK::mode(OUTPUT);
      MOSI::mode(OUTPUT);
      MISO::mode(INPUT);

      PRR0 &= ~(1<<PRSPI); // remove SPI bit from the power reduction register
      // Ok, prescaler setup is really wierd
      static constexpr uint8_t SCKbits[7] = {4,0,5,1,6,2,3};
      SPCR = (1<<MSTR)|(1<<SPE)|(SCKbits[Log2_Prescaler-1] & ((1<<SPR0)|(1<<SPR1))); // enable hardware SPI and set as MASTER, interrupts disabled
      SPSR = (SCKbits[Log2_Prescaler-1] >> 2) & (1 << SPI2X); // setting SPI2X bit of prescaler
    } // Init
    static uint8_t TransferByte(uint8_t d = 0) {
      SPDR = d;
      while (!(SPSR & (1<<SPIF)));
      return SPDR;
    } // Transfer
    static uint16_t TransferWord(uint16_t d) {
      return (uint16_t(TransferByte(uint8_t(d >> 8))) << 8) + TransferByte(uint8_t(d & 0xFF));
    } // Transfer
  }; // class SPImaster

  /*!
   * @brief Following class is used to automatically set mode and CS pin in a
   * function which does SPI transfer. Just call Init once and create an object of this class
   * in the beginning of each function doing SPI transfer
   * @tparam nCS_pin AVP_PIN class - processor pin number this chip CS bus in connected to
   * @tparam Mode SPI mode to activate
   * @tparam Master - tSPImaster class
   * @tparam FDIV
   * @tparam MSBfirst
   */
  template<class Master, class nCS_pin, uint8_t Mode = 0, uint8_t FDIV = 0, bool MSBfirst = true>
  class SPI_Wrapper {
    static uint8_t Nested;
  public:
    SPI_Wrapper() {
      if(!Nested++) {
        Master::SetMode(Mode);
        Master::SetFDIV(FDIV);
        Master::BitOrder(MSBfirst);
        NOP; NOP; NOP;
        nCS_pin::config(OUTPUT,LOW);
      }
    } // constructor
    ~SPI_Wrapper() { if(!--Nested) nCS_pin::set_high(); }
  }; // SPI_Wrapper

  template<class Master, class nCS_pin, uint8_t Mode, uint8_t FDIV, bool MSBfirst>
  uint8_t SPI_Wrapper<Master, nCS_pin, Mode, FDIV, MSBfirst>::Nested = 0;
} // namespace avp

#endif /* HARDSPI_H_ */