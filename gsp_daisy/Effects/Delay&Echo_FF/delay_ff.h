#pragma once
#ifndef GSP_DELAY_FF
#define GSP_DELAY_FF

#include "../guitar_dsp.h"

enum delayff
{
	DELAY_FF 	= 0,
	ECHO_FF 	= 1,
};

class GSP_DelayFF
{
	private:
		int32_t type_ = DELAY_FF;
	public:
		GSP_DelayFF() {}
		GSP_DelayFF(int32_t type) {type_ = type;}
		~GSP_DelayFF() {}

		void 		Init(uint32_t sampling_rate, int16_t *ptr_buffer, uint32_t buffer_size);
		void 		SetDelaySamples(uint32_t dly_spl);
		void 		SetDelayMilliSeconds(float dly_ms);
		void 		SetDecayRate(float decay);
		void 		SetRepeats(uint32_t rpts);
		void 		SetGain(float output_gain);
		void 		Switch(uint8_t mode);
		int32_t 	Process(int32_t sampl, uint32_t buffer_pointer);
		void		Printout(uint8_t out_list, int32_t chn_pos, char *printout);
		void 		GetParams(float param[]);
		void 		SetParams(float param[]);
		
		uint32_t 	sample_rate;	// sampling rate
		uint8_t   	state;
		float  		delay_ms;
		uint32_t 	repeats;
		float     	gain;
		float     	decay_rate;
		uint8_t 	number_params = 5;
		
	private:
		uint32_t  	delay_samples_;
		int16_t 	*ptr_buffer_;
		uint32_t 	buffer_size_;
		uint32_t 	delay_pointer_;
		int32_t 	out_sampl_;
		float 		scale_;
};

#endif 	// GPS_DELAY_FF 	Feedback Delay
