/*!
* @file SoftUART.h
*
* Created: 7/29/2013 2:37:48 PM
*  Author: panasyuk
*  @note To use this class first we have to define HW_UART class using UART_CLASS_DECL(I,PRRi,VectI) macro
*/


#ifndef AVP_UART_H_
#define AVP_UART_H_

#include <stdint.h>
#include <string.h>
#include <AVP_LIBS/General/CircBuffer.h>
#include "HW_UART.h"

// the AVP_UART has two ways to transfer - buffered and unbuffered. We try to fill the buffer first. If message does not fit into buffer
// we STORE a pointer to it and hope it will be valid until sent. After this we can not write any more unless we free buffer first and
// then pointer.

namespace avp {
//! @tparam tSize - type of CircBuffer counter, should be big enough to fit both buffer sizes.
  template<class HW_UART_, uint8_t Log2_TX_Buf_size=5, uint8_t Log2_RX_Buf_Size=5, typename tSize=uint8_t>
  class Serial {
    // there is too ways to setup transmission. Either via circular buffer, or by giving a data
    // pointer and size to transmit
    // NOTE: we use circular buffer as much as we can, when we run out of space we use the block one,
    // when latter is in use write fails
    static volatile const uint8_t *TX_Ptr; // unbuffered transfer, pointer and size of memory block to transfer
    static volatile size_t TX_Size;
    static volatile CircBuffer<uint8_t, Log2_TX_Buf_size, tSize> BufferTX;
    static volatile CircBuffer<uint8_t, Log2_RX_Buf_Size, tSize> BufferRX;

    static bool StoreReceivedByte(uint8_t b) {
      if(!BufferRX.LeftToWrite()) return false;
      BufferRX.Write(b);
      return true;
    } // StoreReceivedByte

    static bool GetByteToSend(volatile uint8_t *b) {
      if(BufferTX.LeftToRead()) *b = BufferTX.Read();
      else if(TX_Size) { // circular buffer is empty and now block pointer
        *b = *(TX_Ptr++);
        TX_Size--;
      } else return false;
      return true;
    } //  GetByteToSend

  public:
    static uint32_t Init(uint32_t baud) {
      return HW_UART_::Init(baud,StoreReceivedByte,GetByteToSend);
    } //  Init

    Serial(uint32_t baud) { Init(baud); }

    static tSize InTransmitBuffer() { return BufferTX.LeftToRead(); }
    static tSize InReadBuffer() { return BufferRX.LeftToRead(); }
    static bool ptr_busy() { return  TX_Size != 0; }
    static bool IsTXdone() { return HW_UART_::IsTXdone(); }

    //! stores character along pd, returns true if there was a character
    static bool ReadInto(uint8_t *pd) {
      if(BufferRX.LeftToRead()) {
        *pd = BufferRX.Read();
        return true;
      } else return false;
    } // ReadInto

    // ALL write function return false is overrun and true if OK

    //! does not assume that Ptr remains valid afterwards, so it buffers data
    //! @param Size is in bytes
    static bool write(const void *Ptr, size_t Size) {
      if(ptr_busy()) return false; // if block is yet to be written we can not write to buffer, as buffer is read first
      if(Size <= BufferTX.LeftToWrite()) {
        const uint8_t *p = (const uint8_t *)Ptr;
        while(Size--) BufferTX.Write(*(p++));
        HW_UART_::EnableTX_Interrupt(); // got something to transmit, reenable interrupt
        return true;
      } else return false;
    }  // write

    //! NOTE: !! stores pointer, so needs ptr and data to be valid until sent
    static bool write_ptr(const void *Ptr, size_t Size) { // stores ptr. Pointed data should not be destroyed until sent
      if(ptr_busy()) return false;
      TX_Ptr = (const uint8_t *)Ptr; TX_Size = Size;
      HW_UART_::EnableTX_Interrupt(); // got something to transmit, reenable interrupt
      return true;
    } // write

    static bool write(uint8_t d) {
      if(ptr_busy()) return false;
      if(!BufferTX.LeftToWrite()) return false;
      BufferTX.Write(d);
      HW_UART_::EnableTX_Interrupt(); // got something to transmit, reenable interrupt
      return true;
    } // write

    static uint8_t GetStatusRX() { return HW_UART_::GetStatusRX(); }

    template<typename T> static bool write(T const *p) { return write(p,sizeof(T)); }
    static bool write(char const *str) { return write(str, ::strlen(str)); } // no ending 0
    template<typename T> static bool write(T d) { return write(&d,sizeof(T)); }
    static bool write(int8_t d) { return write((uint8_t)d); }
  }; // UART
  template<class HW_UART_, uint8_t Log2_TX_Buf_size, uint8_t Log2_RX_Buf_Size, typename tSize>
  volatile const uint8_t *Serial<HW_UART_,Log2_TX_Buf_size,Log2_RX_Buf_Size, tSize>::TX_Ptr; // unbuffered transfer, pointer and size of memory block to transfer
  template<class HW_UART_, uint8_t Log2_TX_Buf_size, uint8_t Log2_RX_Buf_Size, typename tSize>
  volatile size_t Serial<HW_UART_,Log2_TX_Buf_size,Log2_RX_Buf_Size, tSize>::TX_Size = 0;
  template<class HW_UART_, uint8_t Log2_TX_Buf_size, uint8_t Log2_RX_Buf_Size, typename tSize>
  volatile CircBuffer<uint8_t, Log2_TX_Buf_size, tSize> Serial<HW_UART_,Log2_TX_Buf_size,Log2_RX_Buf_Size, tSize>::BufferTX;
  template<class HW_UART_, uint8_t Log2_TX_Buf_size, uint8_t Log2_RX_Buf_Size, typename tSize>
  volatile CircBuffer<uint8_t, Log2_RX_Buf_Size, tSize> Serial<HW_UART_,Log2_TX_Buf_size,Log2_RX_Buf_Size, tSize>::BufferRX;
}; // avp


#endif /* AVP_UART_H_ */

