#include <math.h>
#include <stdio.h>

//#include "daisy_seed.h"
#include "delay_fb.h"

//using namespace daisy;
//DaisySeed   hw;

// *****************************************************************************

void GSP_DelayFB::Init(uint32_t sampling_rate, int16_t *ptr_buffer, uint32_t buffer_size)
{
	/*
    Initiate the Feedback Delay Class.
		sampling_rate
			ADC sampling rate (Hz)
		*ptr_buffer
			buffer to store the processed sample. Delay_FB.Process
			updates the buffer with new processed sample.
		buffer_size
			buffer size.
	*/

	sample_rate 	= sampling_rate;
	ptr_buffer_ 	= ptr_buffer;
	buffer_size_ 	= buffer_size;
	gain 			= 1.;
	
	if(type_ == DELAY_FB) SetDelayMilliSeconds(31.);
	if (type_ == ECHO_FB) SetDelayMilliSeconds(1000.);
	
	SetDecayRate(0.7);
	SetGain(1.);
	Switch(GSP_OFF);
	
	return;
}

void GSP_DelayFB::SetDelaySamples(uint32_t dly_spl)
{
	/*
    To set the number of samples for the Feedback Delay effect.
		dly_spl
			delay time in number of samples, limited by the buffer's
			size.
			
	*/

	delay_samples_ 	= fmax(fmin(dly_spl, buffer_size_ - 1), 10);
	delay_ms 		= (float)delay_samples_/sample_rate*1000.;

	return;
}

void GSP_DelayFB::SetDelayMilliSeconds(float dly_ms)
{
	/*
    To set the delay time of the Feedback Delay effect.
		dlyms
			delay time in milli seconds, from 0, but limited by the buffer's
			size.
			
	*/

	float dly_samples;
	dly_samples 	= sample_rate*dly_ms/1000.f;

	SetDelaySamples((uint32_t)dly_samples);

	return;
}

void GSP_DelayFB::SetDecayRate(float decay)
{
	/*
    To set the decay rate of the Feedback Delay effect.
		decay
			decay rate (0 <= decay <= 0.95) 
			The feedback amplitude falls exponentially for decay values
			less than 1. For larger decay the effect turns longer, but the
			feedback can become unstable. The feedback gain is decay^rept,
			where rept is the number of repetitions.
	*/

	decay_rate 		= fmaxf(fminf(decay, 0.95), 0);
	SetGain(gain);

	return;
}

void GSP_DelayFB::SetGain(float output_gain)
{
	/*
    To set the output gain of Feedback Delay effect
		output_gain
			output gain of Feedback Delay effect (> 0.1)
			The Delay_FB has two channels: the feedfoward (direct)
			signal and the feedback delay. These channels are mixed
			by gains. The output_gain affect both channels, while
			the decay rate affects only the feedback channel.
 	*/

	gain 		= fmaxf(output_gain, 0.1);
	// sum of infinite geometric progression:
	scale_ 		= 0.8*sqrt(gain*gain*(1. - decay_rate*decay_rate));		

	return;
}

void GSP_DelayFB::Switch(uint8_t mode)
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

int32_t GSP_DelayFB::Process(int32_t sampl, uint32_t buffer_pointer)
{
	/*
    To compute the Feedback Delay effect.
		sampl:
			Input sample
		buffer_pointer
			buffer pointer (must be updated by user, once per sample)
		Delay_FB.Process
			Processed output
	*/
  
	if (buffer_pointer >= delay_samples_)
	{
		delay_pointer_	= buffer_pointer - delay_samples_;
	}
	else
	{
		delay_pointer_	= buffer_size_ + buffer_pointer - delay_samples_;
	}
  
	out_sampl_		= scale_*sampl + decay_rate*(*(ptr_buffer_ + delay_pointer_));
	
	if (out_sampl_ > ADC_MAXVAL) out_sampl_ = ADC_MAXVAL;
	if (out_sampl_ < ADC_MINVAL) out_sampl_ = ADC_MINVAL;
	
//	ptr_buffer_[buffer_pointer]	= (int16_t)out_sampl_;
	
	return out_sampl_;
}

void GSP_DelayFB::Printout(uint8_t out_list, int32_t chn_pos, char *printout)
{

	if (type_ == DELAY_FB)
	{
        if (out_list == 0)
        {
		    sprintf(printout, 
    		"->DFB (%ld): OFF(0)|ON(1) %d "
	    	"| Delay Time (0.2-100)(ms): %-.1f "
		    "| Decay rate (0-0.95): %-.3f "
    		"| Gain (0-1): %-.3f\n", 
	    	chn_pos, state, delay_ms, decay_rate, 
		    gain);
        }
        if (out_list == 1)
        {
		    sprintf(printout, 
    		"->DFB (%ld) %d %-.1f %-.3f %-.3f\n", 
	    	chn_pos, state, delay_ms, decay_rate, 
		    gain);
        }
	}
	if (type_ == ECHO_FB)
	{
        if (out_list == 0)
        {
    		sprintf(printout, 
	    	"->EFB (%ld): OFF(0)|ON(1) %d "
		    "| Delay Time (50-)(ms): %-.1f "
    		"| Decay rate (0-0.95): %-.3f "
	    	"| Gain (0-1): %-.3f\n", 
		    chn_pos, state, delay_ms, decay_rate, 
    		gain);
        }
        if (out_list == 1)
        {
		    sprintf(printout, 
    		"->EFB (%ld) %d %-.1f %-.3f %-.3f\n", 
	    	chn_pos, state, delay_ms, decay_rate, 
		    gain);
        }
	}
	
	return;
}

void GSP_DelayFB::GetParams(float fn[])
{
	
	fn[0]   = state;
	fn[1]   = delay_ms;
	fn[2]   = decay_rate;
	fn[3]   = gain;
	
	return;
}

void GSP_DelayFB::SetParams(float fn[])
{

	Switch(fn[0]);
	if (type_ == DELAY_FB) fn[1]   = fminf(fn[1], 100);
	if (type_ == ECHO_FB) fn[1]   = fmaxf(fn[1], 50);
	SetDelayMilliSeconds(fn[1]);
	SetDecayRate(fn[2]);
	SetGain(fn[3]);

	return;
}


