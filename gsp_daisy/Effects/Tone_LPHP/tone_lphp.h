#pragma once
#ifndef GSP_TONE
#define GSP_TONE

#include "../guitar_dsp.h"

class GSP_Tone
{
	public:
		GSP_Tone() {}
		~GSP_Tone() {}

		void 		Init(uint32_t sampling_rate);
		void  		SetTone(float tne);
		int32_t 	Process(int32_t sampl);

		uint32_t 	sample_rate;	// sampling rate
		float     	tone;

	private:
		float 		cb2_, cb1_, cb0_, db2_, db1_, db0_, a2_, a1_, b2_, b1_, b0_;
		int32_t 	u_out_0_, u_out_1_, u_out_2_;
		int32_t 	u_inp_1_, u_inp_2_;
};

#endif 	// GSP_TONE 	Tone Big Muff
