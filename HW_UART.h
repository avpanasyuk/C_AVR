/*!
 * @file AVP_LIBS/AVR/HW_UART.h
 *
 * Created: 7/29/2013 3:51:52 PM
 *  Author: panasyuk
 * 
 * @brief class definition generating macros for UART classes. UART0 and UART1 differs only by numbers 
 * in register name macros,  and to avoid wring two identical classes we use this approach 
 */

#ifndef HW_UART_H
#define HW_UART_H 1

#include <avr/io.h>
#include <avr/interrupt.h>
#include "General.h"

//! redefines register names and bit numbers in uniform fasion
#define UART_DEFS(I,PRRi) \
  REG_PTR_DEF(PRR,PRRi,) \
  BIT_NUM_DEF(PRUSART,I,) \
  REG_PTR_DEF(UBRR,I,) \
  REG_PTR_DEF(UCSR,I,A) \
  REG_PTR_DEF(UCSR,I,B) \
  REG_PTR_DEF(UCSR,I,C) \
  BIT_NUM_DEF(U2X,I,) \
  BIT_NUM_DEF(RXEN,I,) \
  BIT_NUM_DEF(TXEN,I,) \
  BIT_NUM_DEF(RXCIE,I,) \
  BIT_NUM_DEF(UPE,I,) \
  BIT_NUM_DEF(UCSR,I,A) \
  REG_PTR_DEF(UDR,I,) \
  BIT_NUM_DEF(UDRIE,I,)

namespace avp {
  struct UART_callback {
    typedef bool (*t_StoreFunc)(uint8_t b);
    typedef bool (*t_GetFunc)(volatile uint8_t *b);
  };

  template<uint8_t I> class HW_UART_Base: public UART_callback {
    static volatile uint8_t StatusRX;
    static t_StoreFunc StoreReceivedByte;
    static t_GetFunc GetByteToSend;
  };

  template<uint8_t I> volatile uint8_t HW_UART_Base<I>::StatusRX;
  template<uint8_t I> UART_callback::t_StoreFunc HW_UART_Base<I>::StoreReceivedByte;
  template<uint8_t I> UART_callback::t_GetFunc HW_UART_Base<I>::GetByteToSend;
};

//! HW_UART? class definition generation macro 
#define UART_CLASS_DECL(I,PRRi,VectI) \
  class __COMB2(HW_UART,I): public HW_UART_Base<I> { \
    UART_DEFS(I,PRRi) \
    enum StatusBits { OVERRAN, UPE = UPEx, DOR, FE }; \
   public: \
    static uint32_t Init(uint32_t baud,  \
                         UART_callback::t_StoreFunc pS, UART_callback::t_GetFunc pG) { \
      HW_UART_Base<I>::StoreReceivedByte = pS; \
      HW_UART_Base<I>::GetByteToSend = pG; \
      *pPRRx &= ~(1<<PRUSARTx); \
      *pUBRRx = avp::RoundRatio(F_CPU,baud<<4)-1; \
      *pUCSRxA &= ~(1<<U2Xx); \
      *pUCSRxB = (1<<RXENx) | (1<<TXENx) | (1<<RXCIEx); \
      *pUCSRxC = (3<<UCSZx0); \
      sei(); \
      return avp::RoundRatio(F_CPU,uint32_t(*pUBRRx+1)<<4); \
    } \
    ISR(__COMB(USART,VectI,_RX_vect)) { \
      StatusRX |= (7<<UPEx) & UCSRxA; \
      if(!HW_UART_Base<I>::StoreReceivedByte(*pUDRx)) StatusRX |= 1<<OVERRAN; \
    } \
    ISR(__COMB(USART,VectI,_UDRE_vect) { \
      if(!HW_UART_Base<I>::GetByteToSend(pUDRx)) avp::set_low(*pUCSRxB,UDRIEx); \
    } \
    static void EnableTX_Interrupt() { avp::set_high(*pUCSRxB,UDRIEx); } \
  }; //  HW_UARTx

#endif