/*!
* @brief class to set timer to generate rect wave. It does not enable output pin for output mode
*
* Created: 10/26/2013 7:56:54 PM
*  Author: panasyuk
*/

#ifndef SQRWAVE_h
#define SQRWAVE_h

#include <AVP_LIBS/General/General.h>
#include <AVP_LIBS/General/Math.h>
#include <AVP_LIBS/AVR/HW_Timer.h>

// @note We can not set timer to toggle pin OCRB, only OCRA
// @note When prescaler is set to index 0 output is disabled, first active index is one
template<class Timer> struct SqrWave: public Timer {
  typedef typename Timer::CounterType CounterType;
  typedef struct {
    CounterType CountTo; // divider = CountTo + 1
    uint8_t PrescalerInd; //! prescaler index, first active is 1
  } Params;
public:
  static void Init() {     Timer::Power(1); Timer::InitCTC(); }
  SqrWave() { Init(); } // just to call init when initiating static object

  static uint32_t GetFreq(Params Codes) {
    return avp::RoundRatio<uint32_t>(F_CPU >> (Timer::Prescalers[Codes.PrescalerInd-1]+1),
                                     Codes.CountTo+1);
  } // GetFreq

  /*! @brief calculates Prescaler and CountTo setting to get as close as possible to a given
   * frequency, as well as real output frequency
   * @param[in/out] pFreq - pointer to frequency to approximate,  real one on return
   * @param[in/out] pPrescalerInd - pointer to place to return Prescaler index
   * @retval - CountTo Value;
   */
  static Params GetCodes(uint32_t *pFreq) {
    Params Out = {0,0};
    uint32_t Divider = avp::RoundRatio<uint32_t>(F_CPU>>1, *pFreq);
    uint16_t MinPrescaler = Divider >> Timer::Width; // prescaler should be large enough so
    // the rest of the divider fits into Counter
    while(MinPrescaler >> Timer::Prescalers[Out.PrescalerInd++]) {
      #ifdef DEBUG
      if(Out.PrescalerInd >= N_ELEMENTS(Timer::Prescalers)) {
        *pFreq = 0;
        return Out;
      }
      #endif
    }
    // we added additional 1 to PrescalerInd but it is fine because the index is 1-based
    Out.CountTo = (Divider >> Timer::Prescalers[Out.PrescalerInd-1]) - 1;
    *pFreq = GetFreq(Out);
    return Out;
  }	// GetCodes

  /*! @brief jsut passes codes to timer
   *  @param Codes, struct, PrescalerInd is 1-based index of prescalers,
   *  CountTo - specifies divider as CountTo + 1
   */
  static void SetFreqByCodes(Params Codes) {
    Timer::SetPrescaler(Codes.PrescalerInd);
    Timer::SetCountToValueA(Codes.CountTo);
  }	// SetFreqByCodes

  static uint32_t SetFreq(uint32_t Freq) {
    Params Codes = GetCodes(&Freq);
    SetFreqByCodes(Codes);
    return Freq;
  }	// SetFreq
}; // class SqrWave

#endif // SQRWAVE_h
