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
#include <AVP_LIBS/General/Error.h>

namespace EEPROM {
  static inline void write_byte(uint16_t address, uint8_t value) __attribute__((optimize("O3")));
  // there is critical timing inside this function

  static inline void write_byte(uint16_t address, uint8_t value) {
    /* Wait for completion of previous write */
    // debug_printf("%hu<-%hhu.",address,value);

    while(EECR & ((1<<EEPE) | (1<<EEMPE)));
    // while(EECR & (1<<EEPE));
    // while(SPMCSR & (1<<SPMEN));
    EEAR = address;
    EEDR = value;

    ISR_Blocker Auto;
    EECR |= (1<<EEMPE); // there should be at most 4 clocks between these two commands!
    EECR |= (1<<EEPE);
  } // write

  static inline uint8_t read_byte(uint16_t address) {
    /* Wait for completion of previous write */
    while(EECR & (1<<EEPE));

    ISR_Blocker Auto;
    EEAR = address;

    EECR |= (1<<EERE);
    // debug_printf("%hu->%hhu.",address,EEDR);
    return EEDR;
  } // read

  /// @tparam T. If T is array the size is the whole array, n should be 1
  template<typename T> void write_object(uint16_t address, T const &from, size_t n=1) {
    uint8_t const *p_ = (uint8_t const *)&from, *AfterEnd = p_ + sizeof(T)*n;

    while(p_ < AfterEnd) write_byte(address++,*(p_++));
  }

  /// @tparam T. If T is array the size is the whole array, n should be 1
  template<typename T> void read_object(uint16_t address, T &to, size_t n=1) {
    uint8_t *p_ = (uint8_t *)&to, *AfterEnd = p_ + sizeof(T)*n;

    while(p_ < AfterEnd) *(p_++) = read_byte(address++);
  }
} // namespace EEPROM


#endif /* EEPROM_H_ */