/*
* ATmega1284P_Timers.h
*
* Created: 6/14/2014 8:24:01 PM
*  Author: panasyuk
*/


#ifndef ATMEGA1284P_TIMERS_H_
#define ATMEGA1284P_TIMERS_H_

#ifndef __AVR_ATmega1284__
#error Invalid for this processor!
#endif

#include <avr/io.h>
#include "Timer.h"

class Timer0: public Timer<uint8_t> {
	static constexpr uint8_t Prescalers[] = {0,3,6,8,10}; // powers of 2

	static inline void Init() {
		PRR0 &= ~(1<<PRTIM0); // remove Timer0 bit from the power reduction register
		TCCR0A = (1<<COM0A0)|(1<<WGM01);  // toggle OC0A on compare match, set CTC (count to OCR0A) mode,
	}

	static inline void Setup(Params Codes) {
		TCCR0B = Codes.PrescalerInd<<CS00;
		OCR0A = Codes.CountTo;
	}
}; // Timer0

struct Timer1: public Timer<uint16_t> {
	static constexpr uint8_t Prescalers[] = {0,3,6,8,10}; // powers of 2

	static inline void Init() {
		PRR0 &= ~(1<<PRTIM1); // remove Timer1 bit from the power reduction register
		TCCR1A = (1<<COM1A0);  // toggle OC0A on compare match, set CTC (count to OCR0A) mode,
	}

	static inline void Setup(Params Codes) {
		TCCR1B = (Codes.PrescalerInd<<CS10)|(1<<WGM12);
		OCR1A = Codes.CountTo;
	}
}; // Timer1

struct Timer2: public Timer<uint8_t> {
	static constexpr uint8_t Prescalers[] = {0,3,5,6,7,8,10}; // powers of 2

	static inline void Init() {
		PRR0 &= ~(1<<PRTIM2); // remove Timer2 bit from the power reduction register
		TCCR2A = (1<<COM2A0)|(1<<WGM21);  // toggle OC0A on compare match, set CTC (count to OCR0A) mode,
	}

	static inline void Setup(Params Codes) {
		TCCR2B = Codes.PrescalerInd<<CS20;
		OCR2A = Codes.CountTo;
	}
}; // Timer2


struct Timer3: public Timer<uint16_t> {
	static constexpr uint8_t Prescalers[] = {0,3,6,8,10}; // powers of 2

	static inline void Init() {
		PRR1 &= ~(1<<PRTIM3); // remove Timer bit from the power reduction register
		TCCR3A = (1<<COM3A0);  // toggle OC0A on compare match, set CTC (count to OCR0A) mode,
	}

	static inline void Setup(Params Codes) {
		TCCR3B = (Codes.PrescalerInd<<CS30)|(1<<WGM32);
		OCR3A = Codes.CountTo;
	}
}; // Timer3	PRR0 &= ~(1<<PRTIM1); // remove Timer bit from the power reduction register



#endif /* ATMEGA1284P_TIMERS_H_ */