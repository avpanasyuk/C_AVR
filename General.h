/*!
 * @file AVP_LIBS/AVR/AVP_LIBS/General/General.h
 *
 * Created: 7/10/2014 12:28:04 PM
 *  Author: panasyuk
 */ 


#ifndef AVR_GENERAL_H_
#define AVR_GENERAL_H_

#include <AVP_LIBS/General/General.h>

//! in avr files hardware register the register names are defined as macros with numbered names. It is a major pain, because
//! e.g. code for UART0 has to be rewritten for UART1 just replacing indexes in macros. We are trying to deal with it using
//! class definition generating macros

//! @brief used by to define unified pointers to numbered MCU registers. defines constexpr pointer to defined register with 
//! name PreXPost, where X is index. The Name of pointer will  be PrexPost. For example  REG_PTR_DEF(TCCR,0,A) 
//! defines pTCCRxA to point to  TCCR0A 
#define REG_PTR_DEF(Pre,I,Post) \
static constexpr decltype(&__COMB(Pre,I,Post)) \
p##Pre##x##Post = &__COMB(Pre,I,Post);

//! defines constexpr uint8_t number equal to corresponding bit number. For example  REG_PTR_DEF(COM,0,A0)
//! defines COMxA0 to be equal to COM0A0
#define BIT_NUM_DEF(Pre,I,Post) \
static constexpr uint8_t __COMB(Pre,x,Post) = _COMB(Pre,I,Post);

namespace avp {
  template<typename type, typename value_type=type> 
  inline void setbits(volatile type &var, uint8_t lowest_bit, uint8_t numbits, value_type value) {
    var = (var & ~make_mask<type>(lowest_bit,numbits)) | (type(value) << lowest_bit);
  }
}// avp






#endif /* AVR_GENERAL_H_ */