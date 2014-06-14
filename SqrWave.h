/*!
* @brief class to set timer to generate rect wave. It does not enable output pin for output mode
*
* Created: 10/26/2013 7:56:54 PM
*  Author: panasyuk
*/

#ifndef SQRWAVE_h
#define SQRWAVE_h

#include <avr/io.h>
#include <General.h>

// @note We can not set timer to toggle pin OCRB, only OCRA
// @note When prescaler is set to index 0 output is disabled, first active index is one
#ifndef __AVR_ATmega1284__
#error Invalid for this processor!
#endif

//! a set of hardware timer classes to use in SqrWave template. Each class constains hardware information how to program given timer
template<typename CounterType> struct Timer {
	static constexpr uint8_t Width = sizeof(CounterType) << 3;
	typedef struct {
		CounterType CountTo; // divider = CountTo + 1
		uint8_t PrescalerInd; //! prescaler index, first active is 1
	} Params;
}; // Timer8bit

class Timer0: public Timer<uint8_t> {
	static constexpr uint8_t Prescalers[] = {0,3,5,8,10}; // powers of 2

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
	static constexpr uint8_t Prescalers[] = {0,3,5,8,10}; // powers of 2

	static inline void Init() {
		PRR0 &= ~(1<<PRTIM1); // remove Timer bit from the power reduction register
		TCCR1A = (1<<COM1A0);  // toggle OC0A on compare match, set CTC (count to OCR0A) mode,
	}

	static inline void Setup(Params Codes) {
		TCCR1B = (Codes.PrescalerInd<<CS10)|(1<<WGM12);
		OCR1A = Codes.CountTo;
	}
}; // Timer1

struct Timer2: public Timer<uint8_t> {
	static constexpr uint8_t Prescalers[] = {0,3,4,5,7,8,10}; // powers of 2

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
	static constexpr uint8_t Prescalers[] = {0,3,5,8,10}; // powers of 2

	static inline void Init() {
		PRR1 &= ~(1<<PRTIM3); // remove Timer bit from the power reduction register
		TCCR3A = (1<<COM3A0);  // toggle OC0A on compare match, set CTC (count to OCR0A) mode,
	}

	static inline void Setup(Params Codes) {
		TCCR3B = (Codes.PrescalerInd<<CS30)|(1<<WGM32);
		OCR3A = Codes.CountTo;
	}
}; // Timer3

template<class TimerX> class SqrWave {
	public:
	static void Init() { TimerX::Init(); }

	static uint32_t GetFreq(typename TimerX::Params Codes) {
		return avp::RoundRatio<uint32_t>(F_CPU >> (TimerX::Prescalers[Codes.PrescalerInd-1]+1),
		Codes.CountTo+1);
	} // GetFreq

	/*! @brief calculates Prescaler and CountTo setting to get as close as possible to a given
	 * frequency, as well as real output frequency
	 * @param[in/out] pFreq - pointer to frequency to approximate,  real one on return
	 * @param[in/out] pPrescalerInd - pointer to place to return Prescaler index
	 * @retval - CountTo Value;
	 */
	static typename TimerX::Params GetCodes(uint32_t *pFreq) {
		typename TimerX::Params Out = {0,0};
		uint32_t Divider = avp::RoundRatio<uint32_t>(F_CPU>>1, *pFreq);
		uint16_t MinPrescaler = Divider >> TimerX::Width; // prescaler should be large enough so
		// the rest of the divider fits into Counter
		while(MinPrescaler >> TimerX::Prescalers[Out.PrescalerInd++]) {
			#ifdef DEBUG
			if(Out.PrescalerInd > N_ELEMENTS(TimerX::Prescalers)) {
				*pFreq = 0;
				return Out;
			}
			#endif
		}
    // we added additional 1 to PrescalerInd but it is fine because the index is 1-based
		Out.CountTo = (Divider >> TimerX::Prescalers[Out.PrescalerInd-1]) - 1;
		*pFreq = GetFreq(Out);
		return Out;
	}	// GetCodes

	/*! @brief jsut passes codes to timer
   *  @param Codes, struct, PrescalerInd is 1-based index of prescalers, 
   *  CountTo - specifies divider as CountTo + 1
   */
  static void SetFreqByCodes(typename TimerX::Params Codes) {
		TimerX::Setup(Codes);
	}	// SetFreqByCodes

	static uint32_t SetFreq(uint32_t Freq) {
		typename TimerX::Params Codes = GetCodes(&Freq);
		SetFreqByCodes(Codes);
		return Freq;
	}	// SetFreq
}; // class SqrWave

#endif // SQRWAVE_h
