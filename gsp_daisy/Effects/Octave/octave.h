#pragma once
#ifndef GSP_OCTAVE
#define GSP_OCTAVE

#include "../guitar_dsp.h"

class GSP_Octave
{
	public:
		GSP_Octave() {}
		~GSP_Octave() {}

		void 		Init(uint32_t sampling_rate, int16_t *ptr_buffer, uint32_t buffer_size);
		void 		SetShift(float shift);
		void 		SetGain(float out_gain);
		void 		SetMixer(float mix);
		void 		Switch(uint8_t mode);
		void  		ComputeParameters();
		int32_t 	Process(int32_t sampl, uint32_t buffer_pointer);
		void		Printout(uint8_t out_list, int32_t chn_pos, char *printout);
		void 		GetParams(float param[]);
		void 		SetParams(float param[]);

		uint32_t 	sample_rate;	// sampling rate
		float		gain;
		float 		mixer;
		uint8_t 	state;
		uint8_t 	number_params = 3;

	private:
	
		int16_t 	*ptr_buffer_;
		uint32_t 	buffer_size_;
		uint32_t  	k1_, k2_, k1_0_, k2_0_;
		int32_t   	di_, i_, ns_;
		float     	outef_, outsg_;
		float 		sin_[512];
		float 		sfac_;

};

#endif 	// GSP_OCTAVE 	Octave
