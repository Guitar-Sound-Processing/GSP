#pragma once
#ifndef GSP_COMPRESSOR
#define GSP_COMPRESSOR

#define NDB 	512

class GSP_Compressor
{
	public:
		GSP_Compressor() {}
		~GSP_Compressor() {}

		void 		Init(uint32_t sampling_rate);
		void 		SetAttackSamples(uint32_t atk_spl);
		void 		SetAttackMilliSeconds(float atk_ms);
		void 		SetReleaseSamples(uint32_t rel_spl);
		void 		SetReleaseMilliSeconds(float rel_ms);
		void 		SetGainDB(uint32_t gain);
		void 		SetThresholdDB(uint32_t thrsd);
		void 		Switch(uint8_t mode);
		int32_t 	Process(int32_t sampl);
		void		Printout(uint8_t out_list, int32_t chn_pos, char *printout);
		void 		GetParams(float param[]);
		void 		SetParams(float param[]);

		uint32_t 	sample_rate;	// sampling rate
		uint32_t	gain_db;
		int32_t   	threshold_db;
		float     	attack_ms, release_ms;
		uint8_t 	number_params = 5;
		uint8_t 	state;

	private:
	
		float   	T_, TdB_, aTdB_;
		float     	M_;
		float   	y1_, yL_;
		float 		Rinv_;
		float   	alfa_atk_, alfa_rel_;
		float 		yL2i_, cB2i_;
		float   	lgyt_[NDB], ex10_[NDB];
};

#endif 	// GSP_COMPRESSOR 	Compressor 
