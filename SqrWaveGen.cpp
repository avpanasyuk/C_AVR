/*
 * SqrWaveGen.cpp
 *
 * Created: 10/26/2013 7:56:54 PM
 *  Author: panasyuk
 */
#include <avr/io.h>
#include <General.h>
#include "service.h" // this is the header file

// NOTE: We can not set timer to toggle port on OCRB, only OCRA
#ifndef __AVR_ATmega1284__
#error Invalid for this processor!
#endif

static uint8_t FindPrescaler(uint32_t Ratio, const uint8_t *Prescalers, uint8_t N_Prescalers, 
														 uint8_t TimerSize=8) {
  int8_t pI; // this is what is written to the register, smallest is 1
  for(pI=N_Prescalers+1; pI >=2 ;) {
    if(Ratio >> (Prescalers[pI-2] + TimerSize)) break;
    pI--;
  }
  return pI;
} // FindPrescaler

int32_t SetRectWave(uint8_t TimerI, uint32_t Freq) {
  // using Count To Clock (CLC) mode
  switch(TimerI) {
    case 0: {
        const uint8_t Prescalers[] = {0,3,5,8,10}; // powers of 2
        PRR0 &= ~(1<<PRTIM0); // remove Timer0 bit from the power reduction register

        // we have to determine whether we need a prescaler
        uint32_t Ratio = avp::RoundRatio<uint32_t>(F_CPU>>1, Freq)-1;
        uint8_t pI = FindPrescaler(Ratio, Prescalers, sizeof(Prescalers));
        if(pI == sizeof(Prescalers)) return -1;
        TCCR0B = pI<<CS00;
        uint8_t RatioAfterPresc = Ratio>>Prescalers[pI-1];
        OCR0A = RatioAfterPresc;

        TCCR0A = (1<<COM0A0)|(1<<WGM01);  // toggle OC0A on compare match, set CTC (count to OCR0A) mode,
        return avp::RoundRatio<uint32_t>(F_CPU>>(Prescalers[pI-1]+1), RatioAfterPresc+1);
      }
    case 1: {
        const uint8_t Prescalers[] = {0,3,5,8,10}; // powers of 2
        PRR0 &= ~(1<<PRTIM1); // remove Timer bit from the power reduction register

        // we have to determine whether we need a prescaler
        uint32_t Ratio = avp::RoundRatio<uint32_t>(F_CPU>>1, Freq)-1;
        uint8_t pI = FindPrescaler(Ratio, Prescalers, sizeof(Prescalers),16);
        if(pI == sizeof(Prescalers)) return -1;
        TCCR1B = (pI<<CS10)|(1<<WGM12);
        uint16_t RatioAfterPresc = Ratio>>Prescalers[pI-1];
        OCR1A = RatioAfterPresc;

        TCCR1A = (1<<COM1A0);  // toggle OC0A on compare match, set CTC (count to OCR0A) mode,
        return avp::RoundRatio<uint32_t>(F_CPU>>(Prescalers[pI-1]+1), RatioAfterPresc+1);
      }
    case 2: {
        const uint8_t Prescalers[] = {0,3,4,5,7,8,10}; // powers of 2
        PRR0 &= ~(1<<PRTIM2); // remove Timer2 bit from the power reduction register

        // we have to determine whether we need a prescaler
        uint32_t Ratio = avp::RoundRatio<uint32_t>(F_CPU>>1, Freq)-1;
        uint8_t pI = FindPrescaler(Ratio, Prescalers, sizeof(Prescalers));
        if(pI == sizeof(Prescalers)) return -1;
        TCCR2B = pI<<CS20;
        uint8_t RatioAfterPresc = Ratio>>Prescalers[pI-1];
        OCR2A = RatioAfterPresc;

        TCCR2A = (1<<COM2A0)|(1<<WGM21);  // toggle OC0A on compare match, set CTC (count to OCR0A) mode,
        return avp::RoundRatio<uint32_t>(F_CPU>>(Prescalers[pI-1]+1), RatioAfterPresc+1);
      }
    case 3: {
        const uint8_t Prescalers[] = {0,3,5,8,10}; // powers of 2
        PRR1 &= ~(1<<PRTIM3); // remove Timer bit from the power reduction register

        // we have to determine whether we need a prescaler
        uint32_t Ratio = avp::RoundRatio<uint32_t>(F_CPU>>1, Freq)-1;
        uint8_t pI = FindPrescaler(Ratio, Prescalers, sizeof(Prescalers),16);
        if(pI == sizeof(Prescalers)) return -1;
        TCCR3B = (pI<<CS30)|(1<<WGM32);
        uint16_t RatioAfterPresc = Ratio>>Prescalers[pI-1];
        OCR3A = RatioAfterPresc;

        TCCR3A = (1<<COM3A0);  // toggle OC0A on compare match, set CTC (count to OCR0A) mode,
        return avp::RoundRatio<uint32_t>(F_CPU>>(Prescalers[pI-1]+1), RatioAfterPresc+1);
      }
  }
  return -1;
} // SetRectWave
