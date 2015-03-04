// EVERYTHING is done for 10-bit ADC
#include <AVP_LIBS/General/General.h>
#include "ADC.h"

namespace avp {
  namespace ADC_ {
    // FREE RUNNING - ADC does one sample after another and integrate into SamplesSum, SamplesNum
    // default interrupt handler runs maximum oversampling - it adds result to SamplesSum and SamplesNum and starts new convertion
    static uint32_t SamplesSum;
    static volatile uint16_t SamplesNum;


    __weak void ConvertionComplete_Interrupt() {
      SamplesSum += ReadOut();
      SamplesNum++;
      // Start();
    }
    void StartFreeRunning() {
      EnableInterrupts();
      sei();
      SetAutoTriggerSource(FREE);
      AutoTrigger();
      Start();
    };
    uint16_t ReadFreeRun() { // the range is full 16-bit vs 10-bit ADC due to oversampling. To really get this precision we have to oversample by 4096
      DisableInterrupts();
      uint16_t Result = (SamplesSum << 6)/SamplesNum;
      // uint16_t Result = SamplesNum;
      SamplesSum = 0;
      SamplesNum = 0;
      EnableInterrupts();
      return Result;
    }
    uint16_t GetCurrentNumSamples() { return SamplesNum; }
  }
}

ISR(ADC_vect) {
  avp::ADC_::ConvertionComplete_Interrupt();
}