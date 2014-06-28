/*!
* @brief class to set timer to generate rect wave. It does not enable output pin for output mode
*
* Created: 10/26/2013 7:56:54 PM
*  Author: panasyuk
*/

#ifndef SQRWAVE_h
#define SQRWAVE_h

#include <General.h>

// @note We can not set timer to toggle pin OCRB, only OCRA
// @note When prescaler is set to index 0 output is disabled, first active index is one
#ifdef __AVR_ATmega1284__
#include "ATmega1284P_Timers.h"
#endif
#ifdef __AVR_ATmega88A__
#include "ATmega88A_Timers.h"
#endif

template<class TimerX> class SqrWave {
	public:
	static void Init() { TimerX::InitCTC(); }
	SqrWave() { Init(); } // just to call init when initiating static object

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
		TimerX::SetupCTC(Codes);
	}	// SetFreqByCodes

	static uint32_t SetFreq(uint32_t Freq) {
		typename TimerX::Params Codes = GetCodes(&Freq);
		SetFreqByCodes(Codes);
		return Freq;
	}	// SetFreq
}; // class SqrWave

#endif // SQRWAVE_h
