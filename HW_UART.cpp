/*
 * HW_UART.cpp
 *
 * Created: 6/30/2014 3:11:41 PM
 *  Author: panasyuk
 */
#include <avr/interrupt.h>
#include <General.h>
#include "HW_UART.h"

#ifndef PRR0
#define PRR0 PRR
#endif

namespace avp {
  volatile uint8_t UART0::StatusRX; // bits meaning in StatusBits
  bool (*UART0::StoreReceivedByte)(uint8_t b);
  bool (*UART0::GetByteToSend)(volatile uint8_t *b);

  uint32_t UART0::Init(uint32_t baud) {
    PRR &= ~(1<<PRUSART0);

    UBRR0 = avp::RoundRatio(F_CPU,baud<<4)-1;

    UCSR0A &= ~(1<<U2X0); // no special 2x mode
    UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0); // enable transmitter and receiver and receiver interrupts
    UCSR0C = (3<<UCSZ00); // Frame definition
    sei();
    return avp::RoundRatio(F_CPU,uint32_t(UBRR0+1)<<4);
  } // Init

  inline void UART0::RX_vect() {
    StatusRX |= (7<<UPE0) & UCSR0A; // checks whether serial protocol OK
    if(!UART0::StoreReceivedByte(UDR0)) StatusRX |= 1<<OVERRAN;
  } // if buffer is overran it will produce

  // we have top free buffer first and then pointer. Until pointer is freed we can not write any more
  inline void UART0::UDRE_vect() {
    if(!UART0::GetByteToSend(&UDR0)) avp::set_low(UCSR0B,UDRIE0); // nothing to transmit for now, disable interrupt
  }
  
  void UART0::EnableTX_Interrupt() { avp::set_high(UCSR0B,UDRIE0); }

  ISR(USART_RX_vect) { UART0::RX_vect(); }
  ISR(USART_UDRE_vect) { UART0::UDRE_vect(); }

  #ifdef PRUSART1
  // we have to use receiver interrupts, because  otherwise if we stuck for too long we can loose some
  // bits. So we make RX buffer

  volatile uint8_t UART1::StatusRX; // bits meaning in StatusBits
  bool (*UART1::StoreReceivedByte)(uint8_t b);
  bool (*UART1::GetByteToSend)(volatile uint8_t *b);

  uint32_t UART1::Init(uint32_t baud) {
    PRR0 &= ~(1<<PRUSART1);

    UBRR1 = avp::RoundRatio(F_CPU,baud<<4)-1;

    UCSR1A &= ~(1<<U2X1); // no special 2x mode
    UCSR1B = (1<<RXEN1) | (1<<TXEN1) | (1<<RXCIE1); // enable transmitter and receiver and receiver interrupts
    UCSR1C = (3<<UCSZ10); // Frame definition
    sei();
    return avp::RoundRatio(F_CPU,uint32_t(UBRR1+1)<<4);
  }

  inline void UART1::RX_vect() {
    StatusRX |= (7<<UPE1) & UCSR1A; // checks whether serial protocol OK
    if(!UART1::StoreReceivedByte(UDR1)) StatusRX |= 1<<OVERRAN;
  } // if buffer is overran it will produce

  // we have top free buffer first and then pointer. Until pointer is freed we can not write any more
  inline void UART1::UDRE_vect() {
    if(!UART1::GetByteToSend(&UDR1)) avp::set_low(UCSR1B,UDRIE1); // nothing to transmit for now, disable interrupt
  }
    
  void UART1::EnableTX_Interrupt() { avp::set_high(UCSR1B,UDRIE1); }

  ISR(USART1_RX_vect) { UART1::RX_vect(); }
  ISR(USART1_UDRE_vect) { UART1::UDRE_vect(); }

  #endif

}; // avp

