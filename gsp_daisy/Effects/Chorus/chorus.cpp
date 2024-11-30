#include <math.h>
#include <stdio.h>

#include "guitar_dsp.h"
#include "chorus.h"
#include "lfo.h"

// *****************************************************************************

void GSP_Chorus::Init(uint32_t sampling_rate, int16_t *ptr_buffer, uint32_t buffer_size)
{
	/*
    Initiate the Chorus Class.
		sampling_rate
			ADC sampling rate (Hz)
	*/

	sample_rate 	= sampling_rate;
	ptr_buffer_ 	= ptr_buffer;
	buffer_size_ 	= buffer_size;
	
	lfo.Init(sample_rate);
	lfo.SetProfile(LFO_SIN);
	lfo.SetPeriod(2000);

	if (type_ == CHORUS) number_params = 8;
	if (type_ == VIBRATO) number_params = 7;
	
	SetGain(1.);
	SetDelayMilliSeconds(1);
	SetDepth(5.);
	SetMixer(0.5);
	Switch(GSP_OFF);

	return;
}


void GSP_Chorus::SetDelaySamples(uint32_t dly_spl)
{
	/*
    To set the number of samples for the Feedback Delay effect.
		dly_spl
			delay time in number of samples, limited by the buffer's
			size.
			
	*/

	delay_samples_ 	= fmax(fmin(dly_spl, buffer_size_ - 1), 10);
	delay_ms 		= (float)delay_samples_/sample_rate*1000.;
	shift_			= delay_ms*sample_rate/1000;

	return;
}

void GSP_Chorus::SetDelayMilliSeconds(float dly_ms)
{
	/*
    To set the delay time of the chorus effect.
		dlyms
			delay time in milli seconds, limited by the buffer's
			size.
			
	*/
	float dly;
	dly 	= fmax(fmin(dly_ms, 1000), 0);
	SetDelaySamples((uint32_t)sample_rate*dly/1000);

	return;
}

void GSP_Chorus::SetDepth(float dpth)
{
	/*
    To set the Depth for the Chorus effect
		dpth
			The depth (milliseconds) is the maximum time the
			chorus will be delayed from the input signal.
	*/

	depth 		= fmax(fmin(dpth, 100), 0.1);
	amplit_		= depth*sample_rate/1000./(float)lfo.GetAmplitude();

	return;
}

void GSP_Chorus::Switch(uint8_t mode)
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

void GSP_Chorus::SetMixer(float mix)
{
	/*
    To mix the chorus output with the dry signal.
		mix
			fraction of mixer. A zero mix has no chorus, although
			the signal is still present on output, while mix equals 
			to 1 has the chorus only, and mimics a vibrato. Best mix 
			value is 0.5.
	*/

	
	mixer 		= fmaxf(fminf(mix, 1), 0);
	if (type_ == VIBRATO) mixer = 1.;
	ComputeParameters();
	
	return;
}

void GSP_Chorus::SetGain(float output_gain)
{
	/*
    To set the output gain of Chorus effect
		output_gain
			output gain of Chorus effect (> 0.1)
	*/

	gain 		= fmaxf(output_gain, 0.1);
	ComputeParameters();

	return;
}

void GSP_Chorus::ComputeParameters()
{
	/*
    To set the Chorus parameters
	*/

	mix_out_	= 1.2*gain*mixer;
	mix_inp_ 	= 1.2*gain*(1. - mixer);

	return;
}

int32_t GSP_Chorus::Process(int32_t sampl, uint32_t buffer_pointer)
{
	/*
    To compute the Chorus effect on input signal.
		sampl:
			Input sample
		Chorus.Process
			Processed output
	*/
 

	static uint32_t   shift, ptr;
  
	shift   = shift_ + amplit_*lfo.GetValue();

	if (buffer_pointer >= shift) ptr     = buffer_pointer - shift;
	else ptr     = buffer_size_ - shift + buffer_pointer;
 
	return mix_inp_*sampl + mix_out_*(*(ptr_buffer_ + ptr));
}

void GSP_Chorus::Printout(uint8_t out_list, int32_t chn_pos, char *printout)
{

	if (type_ == CHORUS)
	{
        if (out_list == 0)
        {
		    sprintf(printout, 
    		"->CHS (%ld): OFF(0)|ON(1) %d "
	    	"| Depth (0.1-100)(ms): %-.1f "
		    "| Delay (0-1000)(ms): %-.1f "
    		"| Mixer: %-.3f "
        	"| Profile: (0-10) %d "
    	    "| Frequency (0.2-5)(Hz): %-.3f "
        	"| Duty Cycle (0-100)(%%): %-.1f "
	    	"| Gain (0-1): %-4.3f\n", 
		    chn_pos, state, depth, delay_ms, mixer,
    		lfo.profile, 1000./(float)lfo.period, lfo.duty, 
	    	gain);
        }
        if (out_list == 1)
        {
		    sprintf(printout, 
    		"->CHS (%ld) %d %-.1f %-.1f %-.3f %d %-.3f %-.1f %-4.3f\n", 
		    chn_pos, state, depth, delay_ms, mixer,
    		lfo.profile, 1000./(float)lfo.period, lfo.duty, 
	    	gain);
        }
	}
	if (type_ == VIBRATO)
	{
        if (out_list == 0)
        {
		    sprintf(printout, 
    		"->VBT (%ld): OFF(0)|ON(1) %d "
	    	"| Depth (0.1-100)(ms): %-.1f "
		    "| Delay (0-1000)(ms): %-.1f "
        	"| Profile: (0-10) %d "
        	"| Frequency (0.2-5)(Hz): %-.3f "
    	    "| Duty Cycle (0-100)(%%): %-.1f "
    		"| Gain (0-1): %-.3f\n", 
	    	chn_pos, state, depth, delay_ms,
		    lfo.profile, 1000./(float)lfo.period, lfo.duty, 
    		gain);
        }
        if (out_list == 1)
        {
		    sprintf(printout, 
    		"->VBT (%ld) %d %-.1f %-.1f %d %-.3f %-.1f %-.3f\n", 
	    	chn_pos, state, depth, delay_ms,
		    lfo.profile, 1000./(float)lfo.period, lfo.duty, 
    		gain);
        }
	}
	
	return;
}

void GSP_Chorus::GetParams(float fn[])
{
	
	fn[0]   = state;
	fn[1]   = depth;
	fn[2]   = delay_ms;
	if(type_ == CHORUS)
	{
		fn[3]   = mixer;
		fn[4]   = lfo.profile;
		fn[5]   = 1000./(float)lfo.period;
		fn[6]   = lfo.duty;
		fn[7]   = gain;
	}
	if (type_ == VIBRATO)
	{
		fn[3]   = lfo.profile;
		fn[4]   = 1000./(float)lfo.period;
		fn[5]   = lfo.duty;
		fn[6]   = gain;
	}
	
	return;
}

void GSP_Chorus::SetParams(float fn[])
{

	Switch(fn[0]);
	SetDepth(fn[1]);
	SetDelayMilliSeconds(fn[2]);
	if (type_ == CHORUS)
	{
		SetMixer(fn[3]);
		lfo.SetProfile((uint8_t)fn[4]);
		lfo.SetFrequency(fn[5]);
		lfo.SetDutyCycle(fn[6]);
		SetGain(fn[7]);
	}
	if (type_ == VIBRATO)
	{
		SetMixer(1.);
		lfo.SetProfile((uint8_t)fn[3]);
		lfo.SetFrequency(fn[4]);
		lfo.SetDutyCycle(fn[5]);
		SetGain(fn[6]);
	}

	return;
}

