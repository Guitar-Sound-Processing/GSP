#include <math.h>
#include <stdio.h>

#include "guitar_dsp.h"
#include "compressor.h"

// *****************************************************************************

void GSP_Compressor::Init(uint32_t sampling_rate)
{
	/*
    Initiate the Compressor Class.
		sampling_rate
			ADC sampling rate (Hz)
	*/

	sample_rate 	= sampling_rate;
	
	SetAttackMilliSeconds(10.);
	SetReleaseMilliSeconds(1000.);
	SetGainDB(20);
	SetThresholdDB(40);
	Switch(GSP_OFF);
	
	return;
}

void GSP_Compressor::SetAttackSamples(uint32_t atk_spl)
{
	/*
	Set the Compressor attack (response time).
		atk_spl
			attack in samples: 1 to 2*sample_rate. 
	*/

	SetAttackMilliSeconds((float)atk_spl/sample_rate*1000.);

	return;
}

void GSP_Compressor::SetAttackMilliSeconds(float atk_ms)
{
	/*
    To set the attack of the Compressor effect (response time).
		atk_ms
			attack in milliseconds, (1000/sample_rate to 2000).
			
	*/

	attack_ms   = fmaxf(fminf(atk_ms, 2000), 1000/sample_rate);
	alfa_atk_ 	= expf(-1./attack_ms/sample_rate*1000.);

	return;
}

void GSP_Compressor::SetReleaseSamples(uint32_t rel_spl)
{
	/*
	Set the Compressor release time (sustain time).
		rel_spl
			release in samples: 1 to 2*sample_rate. 
	*/

	SetReleaseMilliSeconds((float)rel_spl/sample_rate*1000.);

	return;
}

void GSP_Compressor::SetReleaseMilliSeconds(float rel_ms)
{
	/*
    To set the Compressor release time  (sustain time).
		rel_ms
			release time in milliseconds, (1000/sample_rate to 2000).
			
	*/

	release_ms  = fmaxf(fminf(rel_ms, 2000), 1000/sample_rate);
	alfa_rel_ 	= expf(-1./release_ms/sample_rate*1000.);

	return;
}

void 	GSP_Compressor::SetGainDB(uint32_t gain)
{
	/*
	Set the Compressor gain in dB. If gain is non zero, it amplifies the
	signal when the input power is below the threshold limit. The gain
	shall be kept below half of the threshold limit (SetThresholdDB()), 
	altough there is no imposition on that.
		gain
			compressor gain in dB: 0 to 80. 
	*/

	gain_db		= fmax(fmin(gain, 80), 0);
	M_    		= powf(10, gain_db/20);

	return;
}

void  GSP_Compressor::SetThresholdDB(uint32_t thrsd)
{
	/*
	Set the Compressor threshold in dB. Compression is higher for
	thrsd close to 60, and lower (no compression) if null. The 
	threshold is negative, but it is considered positive for
	easy using. 
		thrsd
			threshold gain in dB: 0 to 80.
	*/

	float R, T, cmax, caux;
	uint32_t i;

	threshold_db	= fmaxf(fminf(thrsd, 80), 0);
	
	TdB_  		= -threshold_db;
	T 			= powf(10, (float)TdB_/20);
	T_ 			= ADC_HALFRES*T;
	aTdB_		= ADC_HALFRES*TdB_;

	R 			= 2;        // compression factor
	Rinv_ 		= 1./R;

	y1_			= 0.;
	yL_ 		= 0.;

	for (i = 0; i < NDB; i++)
	{
		lgyt_[i] 	= ADC_HALFRES*20*log10((float)i/(float)NDB + T); // log lookup table
	}

	yL2i_ 		= (float)NDB/ADC_HALFRES;
	
	cmax 		= (20*log10(1 + T) - TdB_)*(1 - R)*Rinv_;
	caux 		= cmax/(float)NDB/20.;

	for (i = 0; i < NDB; i++) 
	{
		// exponential lookup table
		ex10_[i] 	= powf(10, (float)i*caux);
	}

	cB2i_ 		= (float)NDB/((float)ADC_HALFRES*cmax);

	return;
}

void GSP_Compressor::Switch(uint8_t mode)
{
	/*
    To switch the effect on and off.
		mode
			Condition switch: ON or OFF
	*/

	state 		= GSP_ON;
	if (mode == GSP_OFF) state = GSP_OFF;

	return;

}

int32_t GSP_Compressor::Process(int32_t sampl)
{
	/*
    To compute the Compressor effect.
		sampl:
			Input sample
		Compressor.Process
			Processed output
	*/
 
    static int32_t 	k;
	static float 	xL, yt, xG, yG, sout;

    if (sampl >= 0) xL 		= sampl - T_;
    else xL 	= -sampl - T_;
 
    yt 		= alfa_rel_*(y1_ - xL) + xL; // release level detector

    if (xL > yt) y1_ 		= xL;
    else y1_ 	= yt;

    yL_ 	= alfa_atk_*(yL_ - y1_) + y1_; // attack level detector
    k		= yL_*yL2i_;
   
    if (k < 0) k 	= 0;
    if (k > NDB) k 	= NDB;
    
    xG    	= lgyt_[k]; // dB convertion
    
    if (xG <= aTdB_) yG  	= xG;    // threshold
    else yG  	= (xG - aTdB_)*Rinv_ + aTdB_;
   
    k 		= (yG - xG)*cB2i_;
    
    if (k < 0) k  	= 0;
    if (k > NDB) k	= NDB;
    
	sout 	= M_*ex10_[k]*sampl;    
	
	return (int32_t)sout;
}

void GSP_Compressor::Printout(uint8_t out_list, int32_t chn_pos, char *printout)
{
    if (out_list == 0)
    {
        sprintf(printout, "->CMP (%ld): OFF(0)|ON(1) %d "
	    "| Attack (20-2000)(ms): %-.1f "
        "| Release (20-2000)(ms): %-.1f "
	    "| Gain (0-80)(dB): %ld "
    	"| Threshold (0-80)(dB): %ld\n", 
        chn_pos, state, attack_ms, release_ms, 
        gain_db, threshold_db);
    }
    if (out_list == 1)
    {
        sprintf(printout, "->CMP (%ld) %d %-.1f %-.1f %ld %ld\n", 
        chn_pos, state, attack_ms, release_ms, 
        gain_db, threshold_db);
    }
	
	return;
}

void GSP_Compressor::GetParams(float fn[])
{
	
	fn[0]   = state;
	fn[1]   = attack_ms;
	fn[2]   = release_ms;
	fn[3]   = gain_db;
	fn[4]   = threshold_db;
	
	return;
}

void GSP_Compressor::SetParams(float fn[])
{

	Switch(fn[0]);
	SetAttackMilliSeconds(fn[1]);
	SetReleaseMilliSeconds(fn[2]);
	SetGainDB(fn[3]);
	SetThresholdDB(fn[4]);

	return;
}
