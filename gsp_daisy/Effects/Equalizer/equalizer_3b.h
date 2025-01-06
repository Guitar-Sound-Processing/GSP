#pragma once
#ifndef GSP_EQL
#define GSP_EQL

#include "../guitar_dsp.h"

class GSP_Equalizer
{
	public:
		GSP_Equalizer() {}
		~GSP_Equalizer() {}
		
		void 		Init(uint32_t sampling_rate);
		void 		SetGains(float g_low, float g_medium, float g_high);
		void 		SetFilterFrequencies(float f_low, float f_high);
		void 		ComputeFilter();
		void 		Switch(uint8_t mode);
		int32_t		Process(int32_t sampl);
		void		Printout(uint8_t out_list, int32_t chn_pos, char *printout);
		void 		GetParams(float param[]);
		void 		SetParams(float param[]);

		uint8_t		state;
		uint32_t 	sample_rate;
		float		gain_low, gain_medium, gain_high;
		float		freq_low, freq_high;
		uint8_t 	number_params = 6;
	
	private:
		float		b_l_1_,	b_l_0_,	a_l_1_;
		float 		b_h_1_,	b_h_0_,	a_h_1_;
		float		b_1_2_,	b_1_1_,	b_1_0_,	a_1_2_,	a_1_1_;
		int32_t		u_inp_0_, u_inp_1_, u_inp_2_;
		int32_t		u_out_l_0_, u_out_l_1_;
		int32_t		u_out_1_0_,	u_out_1_1_,	u_out_1_2_;
		int32_t 	u_out_h_0_, u_out_h_1_;
		
 };

#endif 	// GPS_EQL 	3-Band Equalizer
