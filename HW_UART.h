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
#include <AVP_LIBS/General/Math.h>
#include <AVP_LIBS/General/BitBang.h>
#include "General.h"

//! redefines register names and bit numbers in uniform fashion. Defines structure UARTxRegs with register
//! addresses and bit numbers

namespace avp {
  template<class UART_Regs>
  class HW_UART: public UART_Regs {
    typedef UART_Regs R; // just to make it shorter
    typedef bool (*t_StoreFunc)(uint8_t b);
    static t_StoreFunc StoreReceivedByte;

    //! WRITING TO *b immediately sends byte, so GetByteToSend should do it ONLY ONCE !
    typedef bool (*t_GetFunc)(volatile uint8_t *b);
    static t_GetFunc GetByteToSend;

    static volatile uint8_t StatusRX;
    enum StatusBits { OVERRAN, UPE = R::UPEx, DOR, FE };
  public:
    static uint32_t Init(uint32_t baud) {
      *R::pPRRx &= ~(1<<R::PRUSARTx);
      *R::pUBRRx = avp::RoundRatio(F_CPU,baud<<4)-1;
      *R::pUCSRxA &= ~(1<<R::U2Xx); // not using special 2x mode
      // enable transmitter and receiver and receiver interrupts
      *R::pUCSRxB = (1<<R::RXENx) | (1<<R::TXENx) | (1<<R::RXCIEx);
      *R::pUCSRxC = (3<<R::UCSZx0); // Frame definition
      sei();
      return avp::RoundRatio(F_CPU,uint32_t(*R::pUBRRx+1)<<4);
    }

    static void SetCallBacks(t_StoreFunc pS, t_GetFunc pG) {
      StoreReceivedByte = pS;
      GetByteToSend = pG;
    } //  SetCallBacks

    static void RX_vect() __attribute__((always_inline)) { // checks whether serial protocol OK
      StatusRX |= (7<<R::UPEx) & *R::pUCSRxA;
      if(!StoreReceivedByte(*R::pUDRx)) StatusRX |= 1<<OVERRAN;
      // we got to read UDR, otherwise the interrupt
      // will be called indefinitely. We do not have flow control, so we can not just disable interrupt
      // - we will still be just loosing data
    }

    // we have top free buffer first and then pointer. Until pointer is freed we can not write any more
    static void UDRE_vect() __attribute__((always_inline)) {
      if(!GetByteToSend(R::pUDRx)) avp::set_low(*R::pUCSRxB,R::UDRIEx); // disable interrupt
    }

    static void EnableTX_Interrupt() {
      avp::set_high(*R::pUCSRxB,R::UDRIEx);
    }
    static inline void TryToSend() {
      EnableTX_Interrupt();
    }

    static uint8_t GetStatusRX() {
      uint8_t temp = StatusRX;
      StatusRX = 0;
      return temp;
    }

    static bool IsOverrun() { return GetStatusRX() & (OVERRAN | DOR); }

    IGNORE(-Wunused-but-set-variable)
    static void FlushRX() {
      volatile uint8_t dummy;
      while ( *R::pUCSRxA & (1<<R::RXCx) ) dummy = *R::pUDRx;
    }
    STOP_IGNORING
  }; //  HW_UARTx

// following defines are for conveniense only, do not use elsewhere
# define T1 template<class UART_Regs>
# define T2 HW_UART<UART_Regs>

  T1 volatile uint8_t T2::StatusRX;
  T1 typename T2::t_StoreFunc T2::StoreReceivedByte;
  T1 typename T2::t_GetFunc T2::GetByteToSend;
}; // namespace avp

//! this UART definition should be used in processor specific header files only, where they define all timers for this processor
#define UART_DEF(I,PRRi,USARTi) \
  struct COMB3(UART,I,Regs) { \
    REG_PTR_DEF(PRR,PRRi,) \
    BIT_NUM_DEF(PRUSART,I,) \
    REG_PTR_DEF(UBRR,I,) \
    REG_PTR_DEF(UCSR,I,A) \
    REG_PTR_DEF(UCSR,I,B) \
    REG_PTR_DEF(UCSR,I,C) \
    BIT_NUM_DEF(U2X,I,) \
    BIT_NUM_DEF(RXEN,I,) \
    BIT_NUM_DEF(RXC,I,) \
    BIT_NUM_DEF(TXEN,I,) \
    BIT_NUM_DEF(RXCIE,I,) \
    BIT_NUM_DEF(UPE,I,) \
    BIT_NUM_DEF(UCSZ,I,0) \
    REG_PTR_DEF(UDR,I,) \
    BIT_NUM_DEF(UDRIE,I,) \
  }; /* struct UARTxRegs */\
  typedef class avp::HW_UART<COMB3(UART,I,Regs)> COMB2(UART,I);  //! creates UART0, UART1 etc. aliases to use

//! this interrupt handler initialization is used only once in processor definition CPP file,
//! DO NOT USE IN YOUR CODE
#define UART_INIT(I,USARTi) \
  ISR(COMB3(USART,USARTi,_RX_vect)) { COMB2(UART,I)::RX_vect(); } \
  ISR(COMB3(USART,USARTi,_UDRE_vect)) { COMB2(UART,I)::UDRE_vect(); }

#endif

