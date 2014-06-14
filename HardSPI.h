/*
 * HardSPI.h
 * Hardware SPI from AVR
 *
 * Created: 8/1/2013 1:17:11 PM
 *  Author: panasyuk
 */


#ifndef HARDSPI_H_
#define HARDSPI_H_

#include <stdint.h>
#include "service.h"
#include "DigitalPin.h"

class SPI_Wrapper { static uint8_t Nested; };

template<uint8_t nSS, uint8_t CLK, uint8_t MOSI, uint8_t MISO> class tSPI {
public:
  static void SetMode(uint8_t Mode) { SPCR &= ~(3<<CPHA); SPCR |= Mode<<CPHA; }
  static void SetFDIV(uint8_t FDIV) { SPCR &= ~3; SPCR |= FDIV; }
  static void BitOrder(bool MSBfirst) { SPCR &= ~(1<<DORD); SPCR |= MSBfirst?0:(1<<DORD); }
  static void Init(uint8_t Mode = 0, uint8_t FDIV = 0, bool MSBfirst = true) {
    PRR0 &= ~(1<<PRSPI); // remove SPI bit from the power reduction register
    DigitalPin<nSS>::mode(OUTPUT); // and should be kept OUTPUT, otherwise switches to SLAVE  mode
    SPCR = (1<<MSTR)|(1<<SPE); // enable hardware SPI and set as MASTER, interrupts disabled
    SPSR &= ~(1<<SPI2X); // no 2x clock
    DigitalPin<CLK>::mode(OUTPUT);
    DigitalPin<MOSI>::mode(OUTPUT);
    DigitalPin<MISO>::mode(INPUT);
    SetMode(Mode);
    SetFDIV(FDIV);
    BitOrder(MSBfirst);
  } // Init
  static uint8_t Transfer(uint8_t d = 0) {
    SPDR = d;
    while (!(SPSR & (1<<SPIF)));
    return SPDR;
  } // Transfer
  static uint16_t Transfer(uint16_t d) {
    return (uint16_t(Transfer(uint8_t(d >> 8))) << 8) + Transfer(uint8_t(d & 0xFF));
  } // Transfer

  /*! 
   * @brief Following class is used to automatically set mode and CS pin in a
   * function which does SPI transfer. Just call Init once and create an object of this class
   * in the beginning of each function doing SPI transfer
   * @tparam nCS_pin processor pin number this chip CS bus in connected to
   * @tparam SPI_MODE SPI mode to activate
   */ 

  template<uint8_t nCS_pin, uint8_t SPI_MODE> struct tWrapper: public SPI_Wrapper {
//     static struct Initer_ {
//       Initer_(int) { DigitalPin<nCS_pin>::config(OUTPUT,HIGH); }
//     } Initer;
    tWrapper() {
      if(!Nested++) {
        SetMode(SPI_MODE);
        NOP;  NOP;  NOP; // FIXME do we need it?
        DigitalPin<nCS_pin>::low();
      }        
    } // constructor
    ~tWrapper() { if(!--Nested) DigitalPin<nCS_pin>::high(); }
    static void Init() { DigitalPin<nCS_pin>::config(OUTPUT,HIGH); }
  };
};
#endif /* HARDSPI_H_ */