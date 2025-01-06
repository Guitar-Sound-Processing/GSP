#pragma once
#ifndef GSP_TREMOLO
#define GSP_TREMOLO

#include "../guitar_dsp.h"
#include "../LFFG/lfo.h"

class GSP_Tremolo
{
	public:
		GSP_Tremolo() {}
		~GSP_Tremolo() {}
		
		void 		Init(uint32_t sampling_rate);
		void 		Switch(uint8_t mode);
		void 		SetGain(float output_gain);
		int32_t 	Process(int32_t sampl);
		void		Printout(uint8_t out_list, int32_t chn_pos, char *printout);
		void 		GetParams(float param[]);
		void 		SetParams(float param[]);
		
		uint32_t 	sample_rate;	// sampling rate
		uint8_t   	state;
		float     	gain;
		uint8_t 	number_params = 5;
		LowFreqOsc  lfo;

	private:

};

#endif 	// GPS_TREMOLO 	Tremolo effect
