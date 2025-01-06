#pragma once
#ifndef GSP_NOISEGATE
#define GSP_NOISEGATE

#include "../guitar_dsp.h"

class GSP_NoiseGate
{
	public:
		GSP_NoiseGate() {}
		~GSP_NoiseGate() {}

		void 		Init(uint32_t sampling_rate);
		void 		SetAttackSamples(uint32_t atk_spl);
		void 		SetAttackMilliSeconds(float atk_ms);
		void 		SetReleaseSamples(uint32_t rel_spl);
		void 		SetReleaseMilliSeconds(float rel_ms);
		void 		SetGain(float gain);
		void 		SetThreshold(float thrsd);
		void 		Switch(uint8_t mode);
		int32_t 	Process(int32_t sampl);
		void		Printout(uint8_t out_list, int32_t chn_pos, char *printout);
		void 		GetParams(float param[]);
		void 		SetParams(float param[]);

		uint32_t 	sample_rate;	// sampling rate
		float	    gain;
		float   	threshold;
		float     	attack_ms, release_ms;
		uint8_t 	number_params = 5;
		uint8_t 	state;

	private:
	
		float   	y1_, yL_;
        float       level_;
		float   	alfa_atk_, alfa_rel_;
};

#endif 	// GSP_NOISEGATE 	Noise_Gate
