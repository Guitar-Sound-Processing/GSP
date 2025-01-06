#include <math.h>
#include <stdio.h>

#include "delay_ff.h"

// *****************************************************************************

void GSP_DelayFF::Init(uint32_t sampling_rate, int16_t *ptr_buffer, uint32_t buffer_size)
{
	/*
    Initiate the Feedforward Delay Class.
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
	
	repeats = 4;
	gain 	= 1;
	
	if(type_ == DELAY_FF) SetDelayMilliSeconds(31.);
	if(type_ == ECHO_FF) SetDelayMilliSeconds(1000.);

	SetRepeats(4);
	SetDecayRate(0.9);
	SetGain(1.);
	Switch(GSP_OFF);
	
	return;
}

void GSP_DelayFF::SetDelaySamples(uint32_t dly_spl)
{
	/*
    To set the number of samples for the Feedforward Delay effect.
		dly_spl
			delay time in number of samples, limited by the buffer's
			size.
			
	*/

	delay_samples_ 	= fmax(fmin(dly_spl, buffer_size_/repeats - 1), 10);
	delay_ms 		= (float)delay_samples_/sample_rate*1000.;

	return;
}

void GSP_DelayFF::SetDelayMilliSeconds(float dly_ms)
{
	/*
    To set the delay time of the Feedforward Delay effect.
		dlyms
			delay time in milli seconds, limited by the buffer's
			size.
			
	*/

	SetDelaySamples((uint32_t)sample_rate*dly_ms/1000);

	return;
}

void GSP_DelayFF::SetDecayRate(float decay)
{
	/*
    To set the decay rate of the Feedforward Delay effect.
		decay
			decay rate (0 <= decay <= 1) 
			The echo amplitude falls exponentially for decay values
			less than 1. The echo gain is decay^rept,
			where rept is the number of repetitions.
	*/

	decay_rate 		= fmaxf(fminf(decay, 1.), 0.);
	SetGain(gain);
	
	return;
}

void GSP_DelayFF::SetGain(float output_gain)
{
	/*
    To set the output gain of Feedforward Delay effect
		output_gain
			gain of Feedforward Delay (> 0.1).
 	*/

	gain 		= fmaxf(output_gain, 0.1);
	
	// sum of geometric progression
	if (decay_rate < 1)
		if (repeats > 1)
			scale_ 		= sqrt(gain*gain*(1. - decay_rate*decay_rate)/
				(1. - powf(decay_rate, 2*repeats)));
		else
			scale_ 		= gain;
	else
			scale_ 		= sqrt(gain*gain/repeats);
	return;
}

void GSP_DelayFF::SetRepeats(uint32_t rpts)
{
	/*
    To set the number of repetitions for the Feedforward Delay effect.
		rpts
			number of repetitions, (1 to 8). The number of repetitions
			can change the delay time due to the buffer's size.
	*/

	repeats 	= fmax(fmin(rpts, 8), 1);
	
	SetDelaySamples(delay_samples_);
	SetGain(gain);

	return;
}

void GSP_DelayFF::Switch(uint8_t mode)
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

int32_t GSP_DelayFF::Process(int32_t sampl, uint32_t buffer_pointer)
{
	/*
    To compute the Feedback Delay effect.
		sampl:
			Input sample
		Delay_FB.Process
			Processed output
	*/
  
	static int32_t out_sampl;
	static uint32_t i;
	static float   gn;
  
	out_sampl   	= scale_*sampl;

	if (buffer_pointer >= delay_samples_) delay_pointer_     = buffer_pointer - delay_samples_;
	else delay_pointer_	= buffer_size_ - delay_samples_ + buffer_pointer;
	
	gn    		= scale_;
  
	for (i = 1; i < repeats; i++) 
	{
		gn  		*= decay_rate;
		out_sampl  	+= gn*(*(ptr_buffer_ + delay_pointer_));
		if (delay_pointer_ >= delay_samples_) delay_pointer_ 	-= delay_samples_;
		else delay_pointer_ 	+= buffer_size_ - delay_samples_;
	}
  
	if (out_sampl > ADC_MAXVAL) out_sampl = ADC_MAXVAL;
	if (out_sampl < ADC_MINVAL) out_sampl = ADC_MINVAL;
	
	return out_sampl;
}

void GSP_DelayFF::Printout(uint8_t out_list, int32_t chn_pos, char *printout)
{

	if (type_ == DELAY_FF)
	{
        if (out_list == 0)
        {
    		sprintf(printout, 
	    	"->DFF (%ld): OFF(0)|ON(1) %d "
		    "| Delay Time (0.2-100)(ms): %-.1f "
    		"| Decay rate (0-1): %-.3f "
	    	"| Number of repeats (1-8): %ld "
		    "| Gain (0-1): %-.3f\n", 
    		chn_pos, state, delay_ms, decay_rate, 
	    	repeats, gain);
        }
        if (out_list == 1)
        {
    		sprintf(printout, 
	    	"->DFF (%ld) %d %-.1f %-.3f %ld %-.3f\n", 
    		chn_pos, state, delay_ms, decay_rate, 
	    	repeats, gain);
        }
 	}
	if (type_ == ECHO_FF)
	{
        if (out_list == 0)
        {
    		sprintf(printout, 
	    	"->EFF (%ld): OFF(0)|ON(1) %d "
		    "| Delay Time (50-)(ms): %-.1f "
    		"| Decay rate (0-1): %-.3f "
	    	"| Number of repeats (1-8): %ld "
		    "| Gain (0-1): %-.3f\n", 
    		chn_pos, state, delay_ms, decay_rate, 
	    	repeats, gain);
        }
        if (out_list == 1)
        {
    		sprintf(printout, 
	    	"->EFF (%ld) %d %-.1f %-.3f %ld %-.3f\n", 
    		chn_pos, state, delay_ms, decay_rate, 
	    	repeats, gain);
        }
	}
	return;
}

void GSP_DelayFF::GetParams(float fn[])
{
	
	fn[0]   = state;
	fn[1]   = delay_ms;
	fn[2]   = decay_rate;
	fn[3]   = repeats;
	fn[4]   = gain;
	
	return;
}

void GSP_DelayFF::SetParams(float fn[])
{

	Switch(fn[0]);
    if (type_ == DELAY_FF)	fn[1]   = fminf(fn[1], 100);
    if (type_ == ECHO_FF)	fn[1]   = fmaxf(fn[1], 50);
	SetDelayMilliSeconds(fn[1]);
	SetDecayRate(fn[2]);
	SetRepeats(fn[3]);
	SetGain(fn[4]);

	return;
}


