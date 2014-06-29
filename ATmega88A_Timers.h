/*
* ATMEGA88A_Timers.h
*
* Created: 6/14/2014 8:24:01 PM
*  Author: panasyuk
*/


#ifndef ATMEGA88A_TIMERS_H_
#define ATMEGA88A_TIMERS_H_

#ifndef __AVR_ATmega88A__
#error Invalid for this processor!
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Timer.h"

// NOTE!!! There is only one prescaler for both Timer 0 and 1 and none for timer 2 .
// NOGTE Prescaler index is 1-based, because value 0 turns it off 

struct Timer0: public Timer<uint8_t> {
	static constexpr uint8_t Prescalers[] = {0,3,6,8,10}; // powers of 2

	static void InitCTC() {
		PRR &= ~(1<<PRTIM0); // remove Timer0 bit from the power reduction register
		TCCR0A = (1<<COM0A0)|(1<<WGM01);  // toggle OC0A on compare match, set CTC (count to OCR0A) mode,
	}

	static void SetupCTC(Params Codes) {
		TCCR0B = Codes.PrescalerInd<<CS00;
		OCR0A = Codes.CountTo;
	}
}; // Timer0

struct Timer1: public Timer<uint16_t> {
	static constexpr uint8_t Prescalers[] = {0,3,6,8,10}; // powers of 2

	static void InitCTC() {
		PRR &= ~(1<<PRTIM1); // remove Timer2 bit from the power reduction register
		TCCR1A = (1<<COM1A0);  // toggle OC0A on compare match, set CTC (count to OCR0A) mode,
	}

	//! @param PrescalerI 1-based, because value 0 turns it off
	static void InitTimeCounter(uint16_t ClocksInKibitick, uint8_t PrescalerI) {
		// OK, we build time counter on 16-bits timer 3
		PRR &= ~(1<<PRTIM1); // turn on power on the counter
		OCR1A = ClocksInKibitick; // we do interrupt when we count to about a  millisecond
		TIMSK1 |= (1<<OCIE1A); // enable compare interrupts
		TCCR1A = 0; // normal WGM, output pins disconnected
		TCCR1B = (PrescalerI+1)<<CS10;  // PrescalerI is 1-based, because value 0 turns it off 
		sei();
	} // Init

	static void SetupCTC(Params Codes) {
		TCCR1B = (Codes.PrescalerInd<<CS10)|(1<<WGM12);
		OCR1A = Codes.CountTo;
	}
	
	static constexpr volatile uint16_t *pTCNT() { return &TCNT1; }
}; // Timer1

struct Timer2: public Timer<uint8_t> {
	static constexpr uint8_t Prescalers[] = {0}; // powers of 2

	static void InitCTC() {
		PRR &= ~(1<<PRTIM2); // remove Timer2 bit from the power reduction register
		TCCR2A = (1<<COM2A0)|(1<<WGM21);  // toggle OC0A on compare match, set CTC (count to OCR0A) mode,
	}

	static void SetupCTC(Params Codes) {
		TCCR2B = Codes.PrescalerInd<<CS20;
		OCR2A = Codes.CountTo;
	}
}; // Timer2

#endif /* ATMEGA88A_TIMERS_H_ */