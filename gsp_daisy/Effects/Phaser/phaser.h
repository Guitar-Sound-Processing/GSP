#pragma once
#ifndef GSP_PHASER
#define GSP_PHASER

#include "../guitar_dsp.h"
#include "../LFFG/lfo.h"

class GSP_Phaser
{
	public:
		GSP_Phaser() {}
		~GSP_Phaser() {}

		void 		Init(uint32_t sampling_rate);
		void 		SetLevel(float lvl);
		void 		SetDepth(float dpth);
		void 		SetGain(float output_gain);
		void 		Switch(uint8_t mode);
		int32_t 	Process(int32_t sampl);
		void		Printout(uint8_t out_list, int32_t chn_pos, char *printout);
		void 		GetParams(float param[]);
		void 		SetParams(float param[]);

		uint32_t 	sample_rate;	// sampling rate
		uint8_t   	state;
		float     	depth;
		float     	level;
		float     	gain;
		float 		mixer;
		uint8_t 	number_params = 7;
		LowFreqOsc  lfo;

	private:
		float     	gain_ef_;
		float 		am_, bm_;
		float 		dt_;
		float 		u0_, u1_, u2_, u3_, u4_, u5_, u6_, u7_, u8_, u9_, u10_;
};


#endif 	// GPS_PHASER 	Phaser
