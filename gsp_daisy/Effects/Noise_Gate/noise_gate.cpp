#include <math.h>
#include <stdio.h>

#include "noise_gate.h"

// *****************************************************************************

void GSP_NoiseGate::Init(uint32_t sampling_rate)
{
	/*
    Initiate the NoiseGate Class.
		sampling_rate
			ADC sampling rate (Hz)
	*/

	sample_rate 	= sampling_rate;
	
	SetAttackMilliSeconds(10.);
	SetReleaseMilliSeconds(1000.);
	SetGain(1.);
	SetThreshold(0.1);
	Switch(GSP_OFF);
	
	return;
}

void GSP_NoiseGate::SetAttackSamples(uint32_t atk_spl)
{
	/*
	Set the NoiseGate attack (response time).
		atk_spl
			attack in samples: 1 to 2*sample_rate. 
	*/

	SetAttackMilliSeconds((float)atk_spl/sample_rate*1000.);

	return;
}

void GSP_NoiseGate::SetAttackMilliSeconds(float atk_ms)
{
	/*
    To set the attack of the NoiseGate effect (response time).
		atk_ms
			attack in milliseconds, (1000/sample_rate to 2000).
			
	*/

	attack_ms   = fmaxf(fminf(atk_ms, 2000), 1000/sample_rate);
	alfa_atk_ 	= expf(-1./attack_ms/sample_rate*1000.);

	return;
}

void GSP_NoiseGate::SetReleaseSamples(uint32_t rel_spl)
{
	/*
	Set the Compressor release time (sustain time).
		rel_spl
			release in samples: 1 to 2*sample_rate. 
	*/

	SetReleaseMilliSeconds((float)rel_spl/sample_rate*1000.);

	return;
}

void GSP_NoiseGate::SetReleaseMilliSeconds(float rel_ms)
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

void 	GSP_NoiseGate::SetGain(float gn)
{
	/*
	Set the NoiseGate output gain. 
		gn
			NoiseGate gain: 0.1 to 1. 
	*/

	gain		= fmax(fmin(gn, 1.), 0.1);

	return;
}

void  GSP_NoiseGate::SetThreshold(float thrsd)
{
	/*
	Set the NoiseGate threshold. The NoiseGate drives the output
    to ground (0) whenever the input level became below the threshold. 
    If threshold is zero then the NoiseGate bypass the input. If
    threshold is close to 1 then the output will always be zero.
		thrsd
			threshold gain: 0 to 1.
	*/

	threshold	= fmaxf(fminf(thrsd, 1.), 0.);
	
	y1_			= 0.;
	yL_ 		= 0.;

	return;
}

void GSP_NoiseGate::Switch(uint8_t mode)
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

int32_t GSP_NoiseGate::Process(int32_t sampl)
{
	/*
    To compute the NoiseGate effect.
		sampl:
			Input sample
		NoiseGate.Process
			Processed output
	*/
 
	static float 	xL, yt;
    static int32_t  sout;

    if (sampl >= 0) xL 		= sampl;
    else xL 	= -sampl;
 
    yt 		= alfa_rel_*(y1_ - xL) + xL; // release level detector

    if (xL > yt) y1_ 		= xL;
    else y1_ 	= yt;

    yL_ 	= alfa_atk_*(yL_ - y1_) + y1_; // attack level detector

    level_ 	= yL_;
    
    if (level_ < 0) level_ 		= 0;
    if (level_ >= ADC_HALFRES) level_ 	= ADC_HALFRES;
    if (level_ > threshold) sout    = (int32_t)(gain*(float)sampl);
    else sout   = 0;

    return sout;
}

void GSP_NoiseGate::Printout(uint8_t out_list, int32_t chn_pos, char *printout)
{

    if (out_list == 0)
    {
        sprintf(printout, "->NGT (%ld): OFF(0)|ON(1) %d "
	    "| Attack (20-2000)(ms): %-.1f "
        "| Release (20-2000)(ms): %-.1f "
	    "| Gain (0.1-1): %-.3f "
    	"| Threshold (0-1): %-.3f\n", 
        chn_pos, state, attack_ms, release_ms, 
        gain, threshold);
    }
    if (out_list == 1)
    {
        sprintf(printout, "->NGT (%ld) %d %-.1f  %-.1f %-.3f %-.3f\n", 
        chn_pos, state, attack_ms, release_ms, 
        gain, threshold);
    }
	
	return;
}

void GSP_NoiseGate::GetParams(float fn[])
{
	
	fn[0]   = state;
	fn[1]   = attack_ms;
	fn[2]   = release_ms;
	fn[3]   = gain;
	fn[4]   = threshold;
	
	return;
}

void GSP_NoiseGate::SetParams(float fn[])
{

	Switch(fn[0]);
	SetAttackMilliSeconds(fn[1]);
	SetReleaseMilliSeconds(fn[2]);
	SetGain(fn[3]);
	SetThreshold(fn[4]);

	return;
}
