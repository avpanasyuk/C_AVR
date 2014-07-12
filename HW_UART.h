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

//! redefines register names and bit numbers in uniform fashion. Defines structure UARTxRegs with register
//! addresses and bit numbers


namespace avp {
  template<class UART_Regs>  class HW_UART: public UART_Regs {
    typedef UART_Regs R; // just to make it shorter
    typedef bool (*t_StoreFunc)(uint8_t b);
    typedef bool (*t_GetFunc)(volatile uint8_t *b);

    static volatile uint8_t StatusRX;
    static t_StoreFunc StoreReceivedByte;
    static t_GetFunc GetByteToSend;
    enum StatusBits { OVERRAN, UPE = R::UPEx, DOR, FE };
   public:
    static uint32_t Init(uint32_t baud, t_StoreFunc pS, t_GetFunc pG) {
      StoreReceivedByte = pS;
      GetByteToSend = pG;      *R::pPRRx &= ~(1<<R::PRUSARTx);
      *R::pUBRRx = avp::RoundRatio(F_CPU,baud<<4)-1;
      *R::pUCSRxA &= ~(1<<R::U2Xx); // not using special 2x mode
      // enable transmitter and receiver and receiver interrupts
      *R::pUCSRxB = (1<<R::RXENx) | (1<<R::TXENx) | (1<<R::RXCIEx);
      *R::pUCSRxC = (3<<R::UCSZx0); // Frame definition
      sei();
      return avp::RoundRatio(F_CPU,uint32_t(*R::pUBRRx+1)<<4);
    }

    static void RX_vect()
    __attribute__((always_inline)) { // checks whether serial protocol OK
      StatusRX |= (7<<R::UPEx) & *R::pUCSRxA;
      if(!StoreReceivedByte(*R::pUDRx)) StatusRX |= 1<<OVERRAN;
      // we got to read UDR, otherwise the interrupt
      // will be called indefinitely. We do not have flow control, so we can not just disable interrupt
      // - we will still be just loosing data
    }

    // we have top free buffer first and then pointer. Until pointer is freed we can not write any more
    static void UDRE_vect() __attribute__((always_inline)) {
      if(!GetByteToSend(R::pUDRx)) avp::set_low(*R::pUCSRxB,R::UDRIEx);
    }

    static void EnableTX_Interrupt() { avp::set_high(*R::pUCSRxB,R::UDRIEx); }
  }; //  HW_UARTx

  template<class UART_Regs> volatile uint8_t HW_UART<UART_Regs>::StatusRX;
  template<class UART_Regs> typename HW_UART<UART_Regs>::t_StoreFunc HW_UART<UART_Regs>::StoreReceivedByte; 
  template<class UART_Regs> typename HW_UART<UART_Regs>::t_GetFunc HW_UART<UART_Regs>::GetByteToSend; 
}; // namespace avp

//! this UART definition should be used in processor specific header files only, where they define all timers for this processor
#define UART_DEF(I,PRRi,USARTi) \
  struct __COMB(UART,I,Regs) { \
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
    BIT_NUM_DEF(UCSZ,I,0) \
    REG_PTR_DEF(UDR,I,) \
    BIT_NUM_DEF(UDRIE,I,) \
  }; /* struct UARTxRegs */\
  typedef class avp::HW_UART<__COMB(UART,I,Regs)> __COMB2(UART,I);
  
  // ! this interrupt handler initialization is used only once in processor definition CPP file
  #define UART_INIT(I,USARTi) \
  ISR(__COMB(USART,USARTi,_RX_vect)) { __COMB2(UART,I)::RX_vect(); } \
  ISR(__COMB(USART,USARTi,_UDRE_vect)) { __COMB2(UART,I)::UDRE_vect(); }

#endif

