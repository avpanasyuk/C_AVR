/*
 * SoftUART.cpp
 *
 * Created: 7/29/2013 3:51:52 PM
 *  Author: panasyuk
 */

#ifndef HW_UART_H
#define HW_UART_H 1

#include <avr/io.h>

namespace avp {

#define UART_CLASS \
protected: \
  enum StatusBits { OVERRAN, UPE = UPE0, DOR, FE }; \
  static volatile uint8_t StatusRX; \
  static bool (*StoreReceivedByte)(uint8_t b); \
  static bool (*GetByteToSend)(volatile uint8_t *b); \
public: \
  static void SetCallBacks(bool (*pStoreReceivedByte)(uint8_t), \
                           bool (*pGetByteToSend)(volatile uint8_t *)) { \
    StoreReceivedByte = pStoreReceivedByte; \
    GetByteToSend = pGetByteToSend; \
  } \
  static uint8_t GetStatusRX() { uint8_t t = StatusRX; StatusRX = 0; return t; } \
  static uint32_t Init(uint32_t baud); \
  static void RX_vect(); \
  static void UDRE_vect(); \
  static void EnableTX_Interrupt();
   
  struct UART0 {
    UART_CLASS
  }; //UART0

  #ifdef PRUSART1
  struct UART1 {
    UART_CLASS
  }; // UART1
  #endif
}; //avp
#endif