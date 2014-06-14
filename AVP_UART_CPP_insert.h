/*
 * AVP_UART_CPP_insert.h
 *
 * Created: 11/30/2013 9:57:16 AM
 *  Author: panasyuk
 */

static SimpleCircBuffer<uint8_t> BufferTX; // 256 byte buffers
static CircBuffer<uint8_t, 10, uint16_t> BufferRX; //1024 size buffer
static volatile uint8_t StatusRX; // bits meaning in StatusBits

// there is too ways to setup transmission. Either via circular buffer, ot by giving a data
// pointer and size to transmit
// NOTE: we use circular buffer as much as we can, when we run out of space we use the block one,
// when latter is in use write fails
static volatile const uint8_t *TX_Ptr; // unbuffered transfer, pointer and size of memory block to transfer
static volatile size_t TX_Size;

bool ptr_busy() { return  TX_Size; }
  
uint8_t GetStatusRX() { uint8_t t = StatusRX; StatusRX = 0; return t; }

uint8_t LeftToTX() { return BufferTX.LeftToRead(); }

bool ReadInto(uint8_t &d) {
  if(BufferRX.LeftToRead()) {
    d = BufferRX.Read();
    return true;
  } else return false;
} // ReadInto

////// three primary "write" functions
bool write(const void *Ptr, size_t Size) { // does not assume that Ptr remains valid afterwards, so stores into buffer only
  if(ptr_busy()) return false; // if block is yet to be written we can not write to buffer, as buffer is read first
  if(Size <= BufferTX.LeftToWrite()) {
    const uint8_t *p = (const uint8_t *)Ptr;
    while(Size--) BufferTX.Write(*(p++));
    EnableTX_Interrupt(); // got something to transmit, reenable interrupt
    return true;
  } else return false;
}  // write

bool write_ptr(const void *Ptr, size_t Size) { // stores ptr. Pointed data should not be destroyed until sent 
  if(ptr_busy()) return false;
  TX_Ptr = (const uint8_t *)Ptr; TX_Size = Size;
  EnableTX_Interrupt(); // got something to transmit, reenable interrupt
  return true;
} // write

bool write(uint8_t d) {
  if(ptr_busy()) return false;
  if(!BufferTX.LeftToWrite()) return false;
  BufferTX.Write(d);
  EnableTX_Interrupt(); // got something to transmit, reenable interrupt
  return true;
} // write


