/*
 * Timer.h
 *
 * Created: 6/14/2014 8:27:48 PM
 *  Author: panasyuk
 */ 


#ifndef TIMER_H_
#define TIMER_H_

//! a set of hardware timer classes to use in SqrWave template. Each class constains hardware information how to program given timer
template<typename CounterType> struct Timer {
	static constexpr uint8_t Width = sizeof(CounterType) << 3;
	typedef struct {
		CounterType CountTo; // divider = CountTo + 1
		uint8_t PrescalerInd; //! prescaler index, first active is 1
	} Params;
}; // Timer8bit

#endif /* TIMER_H_ */