#pragma once
#ifndef GSP_CHORUS
#define GSP_CHORUS

#include "lfo.h"

enum chorus
{
	CHORUS 		= 0,
	VIBRATO 	= 1,
};

class GSP_Chorus
{
	private:
		int32_t type_ = CHORUS;
	public:
		GSP_Chorus() {}
		GSP_Chorus(int32_t type) {type_ = type;}
		~GSP_Chorus() {}

		void 		Init(uint32_t sampling_rate, int16_t *ptr_buffer, uint32_t buffer_size);
		void 		SetDelaySamples(uint32_t dly_spl);
		void 		SetDelayMilliSeconds(float dly_ms);
		void 		SetDepth(float dpth);
		void 		Switch(uint8_t mode);
		void 		SetMixer(float mix);
		void 		SetGain(float output_gain);
		void 		ComputeParameters();
		int32_t 	Process(int32_t sampl, uint32_t buffer_pointer);
		void		Printout(uint8_t out_list, int32_t chn_pos, char *printout);
		void 		GetParams(float param[]);
		void 		SetParams(float param[]);

		uint32_t 	sample_rate;	// sampling rate
		uint8_t   	state;
		float   	delay_ms;
		float     	mixer;
		float     	gain;
		float     	depth;
		uint8_t 	number_params = 8;
		LowFreqOsc  lfo;

	private:
		uint32_t  	delay_samples_;
		int16_t 	*ptr_buffer_;
		uint32_t 	buffer_size_;
		float     	amplit_;
		float     	mix_inp_, mix_out_;
		uint32_t  	shift_;
};


#endif 	// GPS_CHORUS 	Chorus
