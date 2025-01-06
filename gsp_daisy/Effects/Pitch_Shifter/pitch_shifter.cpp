#include <math.h>
#include <stdio.h>

#include "pitch_shifter.h"

// *****************************************************************************

void GSP_PitchShifter::Init(uint32_t sampling_rate, int16_t *ptr_buffer, uint32_t buffer_size)
{
	/*
    Initiate the PitchShifter Class.
		sampling_rate
			ADC sampling rate (Hz)
		*ptr_buffer
			buffer to store the processed sample. Delay_FB.Process
			updates the buffer with new processed sample.
		buffer_size
			buffer size. Minimum buffer size shall be greater than 'me' variable
			(ComputeParameters()): 4000 (can't be equal to)
	*/
	
 	sample_rate 	= sampling_rate;
	ptr_buffer_ 	= ptr_buffer;
	buffer_size_ 	= buffer_size;
  
	gain 		= 1.;
	mixer 		= 0.5;
	SetShift(5.);
	ComputeParameters();
	Switch(GSP_OFF);
	
	return;
}

void GSP_PitchShifter::SetShift(float shift)
{
	/*
    To set the input gain of PitchShifter effect
		shift
			tone shift (up) in semitones (0 to 12)
	*/

	pshift  = fmaxf(fminf(shift, 12.), 0.);

	return;
}

void GSP_PitchShifter::SetGain(float out_gain)
{
	/*
    To set the output gain of PitchShifter effect
		out_gain
			output gain of PitchShifter effect (> 0.1)
	*/

	gain 		= fmaxf(out_gain, 0.1);
	ComputeParameters();

	return;
}

void GSP_PitchShifter::SetMixer(float mix)
{
	/*
    To mix the PitchShifter output with the main signal.
		mix
			fraction of mixer. A mix zero has no shift, although
			the signal is still present on output, while mix equals 
			to 1 has only the shift signal
	*/

	mixer 	= fmaxf(fminf(mix, 1), 0);
	ComputeParameters();
	
	return;
}

void GSP_PitchShifter::Switch(uint8_t mode)
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

void  GSP_PitchShifter::ComputeParameters()
{
	/*
    To compute the PitchShifter parameters.
	*/

	float p;
	uint32_t  me, ne;
  
	outef_ 		= mixer*gain;
	outsg_ 		= (1 - mixer)*gain;
  
	p   		= powf(2, pshift/12); // frequency ratio

	me 			= 4000;
	ne 			= 357;

	k1_0_ 		= ((float)ne/p + me)*(p - 1);
	k1_   		= k1_0_;
	k2_0_ 		= (float)ne*(p - 1)/p;
	k2_   		= k2_0_;

	i_ 			= 0;
	dk_ 		= -(float)me*(p - 1);
	di_ 		= me;
	d1_ 		= -2*dk_ - di_;
	d2_ 		= -2*(dk_ + di_);
	d3_ 		= -2*dk_;
	d_ 			= d1_;
	ns_ 		= (float)ne/p;

	return;
}
 
int32_t GSP_PitchShifter::Process(int32_t sampl, uint32_t buffer_pointer)
{
	/*
    To compute the PitchShifter effect.
		sampl:
			Input sample
		buffer_pointer
			buffer pointer (must be updated by user, once per sample)
		PitchShifter.Process
			Processed output
	*/
 
    static int32_t    sout;
    static uint32_t   pt1, pt2;
    static float      alfa;
  
    if (i_ < ns_)
    {
        alfa    = (float)i_/(float)ns_;

        if (buffer_pointer >= k1_) pt1   = buffer_pointer - k1_;
        else pt1  	= buffer_size_ - k1_ + buffer_pointer;

        if (buffer_pointer >= k2_) pt2   = buffer_pointer - k2_;
        else pt2  	= buffer_size_ - k2_ + buffer_pointer;

        sout    = alfa*(*(ptr_buffer_ + pt1)) + (1 - alfa)*(*(ptr_buffer_ + pt2));
        i_++;
		
        if (d_ > 0)
        {
            k1_--;
            k2_--;
            d_ 		+= d2_;
        }
        else d_ 	+= d3_;
    }
    else
    {
        if (buffer_pointer >= k1_) pt1   = buffer_pointer - k1_;
        else pt1  	= buffer_size_ - k1_ + buffer_pointer;

        sout    = *(ptr_buffer_ + pt1);
        i_++;
		
        if (d_ > 0)
        {
            k1_--;
            d_ 		+= d2_;
        }
        else d_ 	+= d3_;
		
    }
    if (i_ >= di_)
    {
        i_ 		= 0;
        k1_		= k1_0_;
        k2_		= k2_0_;
        d_ 		= d1_;
    }

    return (int32_t)(outsg_*sampl + outef_*sout);
}

void GSP_PitchShifter::Printout(uint8_t out_list, int32_t chn_pos, char *printout)
{

    if (out_list == 0)
    {
        sprintf(printout, 
        "->SFT (%ld): OFF(0)|ON(1) %d "
        "| Shift (up) (0-12): %-.3f "
        "| Mixer (0-1): %-.3f "
        "| Gain (0-1): %-.3f\n", 
        chn_pos, state, pshift, mixer, 
        gain);
    }
    if (out_list == 1)
    {
        sprintf(printout, 
        "->SFT (%ld) %d %-.3f %-.3f  %-.3f\n", 
        chn_pos, state, pshift, mixer, 
        gain);
    }
	
	return;
}

void GSP_PitchShifter::GetParams(float fn[])
{
	
	fn[0]   = state;
	fn[1]   = pshift;
	fn[2]   = mixer;
	fn[3]   = gain;
	
	return;
}

void GSP_PitchShifter::SetParams(float fn[])
{

	Switch(fn[0]);
	SetShift(fn[1]);
	SetMixer(fn[2]);
	SetGain(fn[3]);

	return;
}


