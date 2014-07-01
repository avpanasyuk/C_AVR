/*
 * SoftUART.cpp
 *
 * Created: 7/29/2013 3:51:52 PM
 *  Author: panasyuk
 */

#include "AVP_UART.h"

namespace avp {
template<> volatile const uint8_t *UART::TX_Ptr; // unbuffered transfer, pointer and size of memory block to transfer
template<> volatile size_t UART::TX_Size;
template<> CircBuffer<uint8_t, Log2_TX_Buf_size> UART::BufferTX;
template<> CircBuffer<uint8_t, Log2_RX_Buf_Size> UART::BufferRX;
}; // avp