/*
 * SoftUART.cpp
 *
 * Created: 7/29/2013 3:51:52 PM
 *  Author: panasyuk
 */

#ifndef HW_UART
#define HW_UART 1

#include <avr/io.h>
#include <CircBuffer.h>

namespace avp {
  class BASE_UART {
    enum StatusBits { OVERRAN, UPE = UPE0, DOR, FE };
      
    static bool (*StoreReceivedByte)(uint8_t b);
    static bool (*GetByteToSend)(volatile uint8_t *b);
    static void SetCallBacks(bool (*pStoreReceivedByte)(uint8_t),
                             bool (*pGetByteToSend)(volatile uint8_t *)) {
      StoreReceivedByte = pStoreReceivedByte;
      GetByteToSend = pGetByteToSend;
    }
  }; // BASE_UART
  
  struct UART0: public BASE_UART {
    static volatile uint8_t StatusRX; // bits meaning in StatusBits
    static void EnableTX_Interrupt() { UCSR0B |= (1<<UDRIE0); }
    static uint32_t Init(uint32_t baud);
    static void RX_vect();
    static void UDRE_vect();
    //! returns StatusRX and resets it. We are discarding all input after error occurs until reset
    static uint8_t GetStatusRX() { uint8_t t = StatusRX; StatusRX = 0; return t; }
  }; //UART0

  #ifdef PRUSART1
  struct UART1: public BASE_UART {
    static volatile uint8_t StatusRX; // bits meaning in StatusBits
    static void EnableTX_Interrupt() { UCSR1B |= (1<<UDRIE1); }
    static uint32_t Init(uint32_t baud);
    static void RX_vect();
    static void UDRE_vect();
    //! returns StatusRX and resets it. We are discarding all input after error occurs until reset
    static uint8_t GetStatusRX() { uint8_t t = StatusRX; StatusRX = 0; return t; }
  }; // UART1
    #endif
}; //avp
#endif