#pragma once
#ifndef GSP_DETUNE
#define GSP_DETUNE

#include "../guitar_dsp.h"

class GSP_Detune
{
	public:
		GSP_Detune() {}
		~GSP_Detune() {}

		void 		Init(uint32_t sampling_rate, int16_t *ptr_buffer, uint32_t buffer_size);
		void 		SetDetune(float dtne);
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
		float 		detune;
		float 		mixer;
		uint8_t 	state;
		uint8_t 	number_params = 4;

	private:
	
		int16_t 	*ptr_buffer_;
		uint32_t 	buffer_size_;
		uint32_t  	k1_, k2_, k1_0_, k2_0_;
		int32_t   	dk_, di_, i_, d1_, d2_, d3_, d_, mp_;
		float     	outef_, outsg_;

};

#endif 	// GSP_DETUNE 	Detune
