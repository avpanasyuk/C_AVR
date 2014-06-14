/*
 * EEPROM.h
 *
 * Created: 12/30/2013 1:02:45 PM
 *  Author: panasyuk
 */


#ifndef EEPROM_H_
#define EEPROM_H_

#include <stddef.h>
#include <avr/io.h>
#include "service.h"

namespace EEPROM {
  static inline void write(uint16_t address, uint8_t value) {
    /* Wait for completion of previous write */
    while(EECR & (1<<EEPE));
    /* Set up address and Data Registers */
    EEAR = address;
    EEDR = value;

    ISR_Blocker Auto;
    /* Write logical one to EEMPE */
    EECR |= (1<<EEMPE);
    /* Start eeprom write by setting EEPE */
    EECR |= (1<<EEPE);
  } // write

  static inline uint8_t read(uint16_t address) {
    /* Wait for completion of previous write */
    while(EECR & (1<<EEPE));
    /* Set up address register */
    EEAR = address;
    /* Start eeprom read by writing EERE */
    EECR |= (1<<EERE);
    /* Return data from Data Register */
    return EEDR;
  } // read

  template<typename T> void write(uint16_t address, T const *from, size_t n) {
    uint8_t const *p_ = (uint8_t const *)from, *AfterEnd = p_ + sizeof(T)*n;

    while(p_ < AfterEnd) write(address++,*(p_++));
  }

  template<typename T> void read(uint16_t address, T *to, size_t n) {
    uint8_t *p_ = (uint8_t *)to, *AfterEnd = p_ + sizeof(T)*n;

    while(p_ < AfterEnd) *(p_++) = read(address++);
  }
} // namespace EEPROM


#endif /* EEPROM_H_ */