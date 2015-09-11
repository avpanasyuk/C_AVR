/*
 * I2C.h
 *
 * Created: 9/9/2015 9:49:31 AM
 *  Author: panasyuk
 *
 * @brief I2C communication using TWI module of AVR MCU.
 * @date 9/9/15 Original rough version just for interruptless writing
 *
 * @note both SCL and SDA lines should have pull-up resistors 1 kOhm < R < 300ns/Cload, internal may be used
 * @note when I implement full capabilities class I should use Port.h
 */


#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>
#include <AVP_LIBS/General/Error.h>
#include <AVP_LIBS/General/BitBang.h>
#include "GIOpins.h"

namespace avp {
  /*!
   * @tparam ClkF, clock frequency, Hz, F_CPU/16 > ClkF_kHz >= F_CPU/528.
   * If we recode it with prescaler we are not using prescaler
   * @tparam Address Slave address
   */

  template<uint32_t ClkF, uint8_t SlaveAddress> class I2C_master {
    static_assert(F_CPU < ClkF*528, "ClkF is too low, recode with prescaler!");
    static_assert(F_CPU > ClkF*16, "ClkF is too high!");
  protected:
    enum Status_ {START=0x08, RESTART_=0x10, WRT_ACK=0x18, WRT_NOACK=0x20, DAT_ACK=0x28,
                  DAT_NOACK=0x30, BUSY=0x38, RD_ACK = 0x40, RD_NOACK=0x48, RDD_ACK=0x50,
                  RDD_NOACK=0x58
                 };
    static enum Status_ get_status() {
      while((TWCR & (1 << TWINT)) == 0);
      return Status_(TWSR & (0xFF - 0x3));
    };
  public:
    static void Init() {
      set_low(PRR,PRTWI); // enable power
      // set clk frequency
      setbits(TWSR,0,2,0); // prescaler == 1
      TWBR = (F_CPU/ClkF-16)/2;
      // setup
      TWCR = 1 << TWEN | 1 << TWEA; // enable module, no interrupts, send ACK on receive
    } // Init

    static void send(uint8_t const *pb, uint16_t Size) { // Master transmitter
      TWCR |= 1 << TWSTA | 1 << TWINT;
      AVP_ASSERT(get_status() == START);
      TWDR = SlaveAddress << 1; // SLA+W
      TWCR |= 1 << TWINT;
      switch(get_status()) {
        case BUSY: AVP_ERROR("I2C_master::send:BUSY!");
        case WRT_NOACK: AVP_ERROR("I2C_master::send:No WRT_ACK!");
        default: AVP_ERROR("I2C_master::send:Wrong Status SLA+W!");
        case WRT_ACK:;
      }
      while(Size--) {
        TWDR = *(pb++); // data
        TWCR |= 1 << TWINT;
        switch(get_status()) {
          case BUSY: AVP_ERROR("I2C_master::send:BUSY!");
          case DAT_NOACK: AVP_ERROR("I2C_master::send:No DAT_ACK!");
          default: AVP_ERROR("I2C_master::send:Wrong Status DAT!");
          case DAT_ACK:;
        }
      }
      TWCR |= 1 << TWSTO | 1 << TWINT;
    } // send

    static inline void send(uint8_t b) { send(&b,1); }

    static void receive(uint8_t *pb, uint16_t *pSize)  {
      TWCR |= 1 << TWSTA | 1 << TWINT;
      AVP_ASSERT(get_status() == START);
      TWDR = (SlaveAddress << 1) | 1; // SLA+R
      TWCR |= 1 << TWINT;
      switch(get_status()) {
        case BUSY: AVP_ERROR("I2C_master::receive:BUSY!");
        case RD_NOACK: AVP_ERROR("I2C_master::receive:No RD_ACK!");
        default: AVP_ERROR("I2C_master::receive:Wrong Status SLA+R!");
        case RD_ACK:;
      }
      *pSize = 0;
      bool AllRead = false;
      while(!AllRead) {
        *(pb++) = TWDR; // data
        (*pSize)++;
        TWCR |= 1 << TWINT;
        switch(get_status()) {
          case BUSY: AVP_ERROR("I2C_master::send:BUSY!");
          default: AVP_ERROR("I2C_master::send:Wrong Status DAT!");
          case DAT_NOACK: AllRead = true;
          case DAT_ACK:;
        }
      }
      TWCR |= 1 << TWSTO | 1 << TWINT;
    } // receive
  }; // I2C_master
} // avp



#endif /* I2C_H_ */