#pragma once
#ifndef GSP_OVERDRIVE
#define GSP_OVERDRIVE

#include "../guitar_dsp.h"
#include "../Tone_LPHP/tone_lphp.h"

class GSP_Overdrive
{
	public:
		GSP_Overdrive() {}
		~GSP_Overdrive() {}

		void 		Init(uint32_t sampling_rate);
		void 		SetSustain(float sustn);
		void 		SetMixer(float mixer);
		void 		SetGain(float output_gain);
		void 		Switch(uint8_t mode);
		int32_t 	Process(int32_t sampl);
		void		Printout(uint8_t out_list, int32_t chn_pos, char *printout);
		void 		GetParams(float param[]);
		void 		SetParams(float param[]);

		uint32_t 	sample_rate;	// sampling rate
		float     	sustain;
		float 		gain;
		float 		mixer;
		uint8_t 	state;
		uint8_t 	number_params = 5;
		GSP_Tone 	tone;

	private:
		int32_t   	conf_;    // 2^16 float to int convertion
		float   	a1_us_, b0_us_, b1_us_, c0_us_, c1_us_;
		float   	a1_ub_, b0_ub_, b1_ub_, c0_ub_, c1_ub_;
		int32_t   	us1_0_, us1_1_, ue1_1_, vd1_0_, vd1_1_, ub1_1_, ub1_0_;
		int32_t   	us2_0_, us2_1_, ue2_1_, vd2_0_, vd2_1_, ub2_1_, ub2_0_;
		float     	amp1_, amp2_, amp3_;
		float 		mix_dry_, mix_wet_;
		int32_t   	diode_[256];
		int32_t 	cr10_, c16b_;
		int32_t   	v2i_, ln10t10_, adc2_;
};

#endif 	// GSP_OVERDRIVE 	Big Muff Overdrive
