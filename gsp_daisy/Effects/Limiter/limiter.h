#pragma once
#ifndef GSP_LMT
#define GSP_LMT

#include "../guitar_dsp.h"

class GSP_Limiter
{
	public:
		GSP_Limiter() {}
		~GSP_Limiter() {}
		
		void 		Init(uint32_t sampling_rate);
		void 		SetSmooth(float smth);
		void 		SetGain(float i_gain);
		void 		Switch(uint8_t mode);
		int32_t		Process(int32_t sampl);
		void		Printout(uint8_t out_list, int32_t chn_pos, char *printout);
		void 		GetParams(float param[]);
		void 		SetParams(float param[]);

		uint8_t		state;
		float		smooth;
		float 		input_gain;
	
	private:
		uint32_t 	sample_rate_;
		int32_t 	adc_limit_;
		float 		wl_, wl4_, limmw_, limpw_;
 };

#endif 	// GPS_LMT 	Soft limiter
