/*
 * AVP_UART_base.h
 *
 * Created: 11/30/2013 9:51:39 AM
 *  Author: panasyuk
 */

uint32_t Init(uint32_t baud);

bool ptr_busy();
//! returns StatusRX and resets it. We are discarding all input after error occurs until reset
uint8_t GetStatusRX(); 
//! returns a pointer to a character, or NULL if none are available
bool ReadInto(uint8_t &d); 

// ALL write function return false is overrun and true if OK
////// two primary "write" functions
bool write(uint8_t d);

//! does not assume that Ptr remains valid afterwards,
//! so copies data from Ptr into buffer
bool write(const void *Ptr, size_t Size /*!< in bytes */); 

//! NOTE: !! stores pointer, so needs ptr and data to be valid until sent
bool write_ptr(const void *Ptr, size_t Size /*!< in bytes */); 
template<typename T> inline bool write(T const *p) { return write(p,sizeof(T)); }
inline bool write(char const *str) { return write(str, ::strlen(str)); } // no ending 0
template<typename T> inline bool write(T d) { return write(&d,sizeof(T)); }
inline bool write(int8_t d) { return write((uint8_t)d); }
  
uint8_t LeftToTX();

