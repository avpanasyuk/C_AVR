/*
 * SoftUART.cpp
 *
 * Created: 7/29/2013 3:51:52 PM
 *  Author: panasyuk
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <General.h>
#include <CircBuffer.h>
#include "service.h"
#include "AVP_UART.h"

namespace AVP_UART0 {
  enum StatusBits { OVERRAN, UPE = UPE0, DOR, FE };
  static inline void EnableTX_Interrupt() { UCSR0B |= (1<<UDRIE0); }

#include "AVP_UART_CPP_insert.h"

  uint32_t Init(uint32_t baud) {
    PRR &= ~(1<<PRUSART0);

    UBRR0 = avp::RoundRatio(F_CPU,baud<<4)-1;

    UCSR0A &= ~(1<<U2X0); // no special 2x mode
    UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0); // enable transmitter and receiver and receiver interrupts
    UCSR0C = (3<<UCSZ00); // Frame definition
    sei();
    return avp::RoundRatio(F_CPU,uint32_t(UBRR0+1)<<4);
  }

  ISR(USART_RX_vect) {
    if(BufferRX.LeftToWrite()) {
      StatusRX |= (7<<UPE0) & UCSR0A; // checks whether serial protocol OK
      BufferRX.Write(UDR0); 
    } else {
      StatusRX |= 1<<OVERRAN;
      volatile uint8_t Discard __attribute__ ((unused)) = UDR0; // we got to read UDR, otherwise the interrupt
    // will be called indefinitely. We do not have flow control, so we can not just disable interrupt
    // - we will still be just loosing data
    }
  } // if buffer is overran it will produce

  // we have top free buffer first and then pointer. Until pointer is freed we can not write any more 
  ISR(USART_UDRE_vect) {
    if(BufferTX.LeftToRead()) UDR0 = BufferTX.Read();
    else if(TX_Size) { // circular buffer is empty and now block pointer
      UDR0 = *(TX_Ptr++);
      TX_Size--;
    } else UCSR0B &= ~(1<<UDRIE0); // nothing to transmit for now, disable interrupt
  }
} // namespace AVP_UART0

#ifdef PRUSART1
namespace AVP_UART1 {
  enum StatusBits { OVERRAN, UPE = UPE1, DOR, FE };
  static inline void EnableTX_Interrupt() { UCSR1B |= (1<<UDRIE1); }
#include "AVP_UART_CPP_insert.h"
  // we have to use receiver interrupts, because  otherwise if we stuck for too long we can loose some
  // bits. So we make RX buffer

  uint32_t Init(uint32_t baud) {
    PRR0 &= ~(1<<PRUSART1);

    UBRR1 = avp::RoundRatio(F_CPU,baud<<4)-1;

    UCSR1A &= ~(1<<U2X1); // no special 2x mode
    UCSR1B = (1<<RXEN1) | (1<<TXEN1) | (1<<RXCIE1); // enable transmitter and receiver and receiver interrupts
    UCSR1C = (3<<UCSZ10); // Frame definition
    sei();
    return avp::RoundRatio(F_CPU,uint32_t(UBRR1+1)<<4);
  }

  ISR(USART1_RX_vect) {
    if(BufferRX.LeftToWrite()) {
      StatusRX |= (7<<UPE1) & UCSR1A; // checks whether serial protocol OK
      BufferRX.Write(UDR1); 
    } else {
      StatusRX |= 1<<OVERRAN;
      volatile uint8_t Discard __attribute__ ((unused)) = UDR1; // we got to read UDR, otherwise the interrupt will be called indefinitely
    }      
  } // if buffer is overran it will produce


  ISR(USART1_UDRE_vect) {
    if(BufferTX.LeftToRead()) UDR1 = BufferTX.Read();
    else if(TX_Size != 0) { // circular buffer is empty and now block pointer
      UDR1 = *(TX_Ptr++);
      TX_Size--;
    } else UCSR1B &= ~(1<<UDRIE1); // nothing to transmit for now, disable interrupt
  }

} // namespace AVP_UART1
#endif
