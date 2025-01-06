#pragma once
#ifndef GSP_REVERBER
#define GSP_REVERBER

#include "../guitar_dsp.h"

class GSP_Reverber
{
	public:
		GSP_Reverber() {}
		~GSP_Reverber() {}

		void 		Init(uint32_t sampling_rate, int16_t *ptr_buffer, uint32_t buffer_size);
		void 		SetReverberTimeMS(float rvb_ms);
		void 		SetGain(float output_gain);
		void 		Switch(uint8_t mode);
		void 		ComputeParameters();
		int32_t 	Process(int32_t sampl);
		void		Printout(uint8_t out_list, int32_t chn_pos, char *printout);
		void 		GetParams(float param[]);
		void 		SetParams(float param[]);

		uint32_t 	sample_rate;	// sampling rate
		uint8_t   	state;
		float  		reverber_time;    
		float     	gain;
		uint8_t 	number_params = 3;

	private:
		int32_t   	rim1_0_, rim1_1_, rim1_2_, rim1_3_;
		int32_t   	ynm1_;
		uint32_t  	n_;
		int32_t   	bb_;
		int32_t   	pip_0_, pip_1_, pip_2_, pip_3_;
		int32_t   	pip1_0_, pip1_1_, pip1_2_, pip1_3_;
		float 		cinv_, bbinv_;
		int32_t   	conf_ = 4096;    // float to int convertion
		float     	alfa_ = 0.99;  // tone corrector gain (less than 0.999)
		uint32_t  	Mi_0_ = 1619, Mi_1_ = 1493, Mi_2_ = 1361, Mi_3_ = 1117; // delay interval
		
		uint32_t  	delay_samples_;
		int16_t 	*ptr_buffer_0, *ptr_buffer_1, *ptr_buffer_2, *ptr_buffer_3;
		uint32_t 	buffer_size_;
		uint32_t 	delay_pointer_;
};

#endif 	// GPS_REVERBER 	Reverber
