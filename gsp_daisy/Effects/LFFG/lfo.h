#pragma once
#ifndef GSP_LFO
#define GSP_LFO

#include "../guitar_dsp.h"
#include "../Level_Detector/level_detector.h"

enum lfo_wave
{
	LFO_SIN, 			// Sine <0:2*pi>
	LFO_HALFSINE, 		// Half sine <0:pi>
	LFO_RAMP,			// Saw tooth /|/|/|
	LFO_SAW, 			// Saw tooth |\|\|\.
	LFO_TRIANGLE,		// Saw tooth /\/\/\.
	LFO_SQUARE,			// Square |-|_|-|_|
	LFO_EXPDECREASE, 	// Decreasing exponential |\_
	LFO_EXPINCREASE, 	// Increasing exponential _/|
	LFO_EXTERNAL, 		// External signal ('pot')
	LFO_LEVEL, 			// Power amplitude of input signal
    LFO_REVERSE_LEVEL,  // Reverse amplitude level of input signal
	LFO_LAST			// None
};

class LowFreqOsc
{
	public:
		LowFreqOsc() {}
		~LowFreqOsc() {}
		
		void 		Init(uint32_t sampling_rate);
		void 		SetPeriod(uint32_t prd);
		void 		SetFrequency(float freq);
		void 		SetDutyCycle(float duty_cycle);
		void 		SetProfile(uint8_t prof);
		void 		SetGain(uint32_t gain);
		uint32_t	GetAmplitude();
		uint32_t 	GetValue();

		uint32_t 	sample_rate;	// sampling rate
		uint8_t   	profile;
		uint32_t  	period;  		// period of LFO in milliseconds
		float   	duty;     		// duty cycle (on) for some profiles, in percentile of period
		uint32_t 	max_gain; 		// LFO Amplitude
		
	private:
		uint32_t  	ampl_[512]; 	// lookup table (internal use)
		float     	phase_;   		// current phase (internal use)
		float     	rate_;			// phase angle increment
		uint32_t 	gain_;			// amplitude of LFO for LFO_EXTERNAL profile
									// or decay rate for exponential decay or inverse ED
		uint32_t 	duty_; 			// duty cycle in fraction of time lenght
};

#endif 	// GPS_LFO 	Low Frequency Oscillator
