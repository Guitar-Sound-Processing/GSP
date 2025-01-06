#pragma once
#ifndef GSP_WAHWAH
#define GSP_WAHWAH

#include "../guitar_dsp.h"
#include "../LFFG/lfo.h"

class GSP_WahWah
{
	public:
		GSP_WahWah() {}
		~GSP_WahWah() {}

		void 		Init(uint32_t sampling_rate);
		void 		SetGain(float input_gain);
		void 		Switch(uint8_t mode);
		int32_t 	Process(int32_t sampl);
		void		Printout(uint8_t out_list, int32_t chn_pos, char *printout);
		void 		GetParams(float param[]);
		void 		SetParams(float param[]);

		uint32_t 	sample_rate;	// sampling rate
		float		gain;
		uint8_t 	state;
		uint8_t 	number_params = 5;
		LowFreqOsc  lfo;

	private:
	
		float 		ampl_;
		float 		c0_, c1_;
		float 		bb2_, bb1_, bb0_;
		float 		a2_0_, a2_1_, a1_0_; 
		float 		a1_1_, a0_0_, a0_1_;
		int32_t 	s1_, s2_, e1_, e2_;

};

#endif 	// GSP_WAHWAH 	wah-wah
