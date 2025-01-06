#include <math.h>
#include <stdio.h>

#include "octave.h"

//**#include "DaisyDuino.h"

// *****************************************************************************

void GSP_Octave::Init(uint32_t sampling_rate, int16_t *ptr_buffer, uint32_t buffer_size)
{
	/*
    Initiate the Octave Class.
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
	
	for (int i = 0; i < 512; i++)
	{
		sin_[i] 	= sin(float(i)/512*GDSP_PI);
	}

	ComputeParameters();
	Switch(GSP_OFF);
	
	return;
}

void GSP_Octave::SetGain(float out_gain)
{
	/*
    To set the output gain of Octave effect
		out_gain
			output gain of Octave effect (> 0.1)
	*/

	gain 		= fmaxf(out_gain, 0.1);
	ComputeParameters();

	return;
}

void GSP_Octave::SetMixer(float mix)
{
	/*
    To mix the Octave output with the main signal.
		mix
			fraction of mixer. A mix zero has no shift, although
			the signal is still present on output, while mix equals 
			to 1 has only the shift signal
	*/

	mixer 	= fmaxf(fminf(mix, 1), 0);
	ComputeParameters();
	
	return;
}

void GSP_Octave::Switch(uint8_t mode)
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

void  GSP_Octave::ComputeParameters()
{
	/*
    To compute the Octave parameters.
	*/

	uint32_t  me;
  
	me 		= 3000;
	k1_0_ 	= me;
	k2_0_ 	= 3*me/2;
	ns_		= me/2;
	sfac_ 	= (float)512./me;
	
	k1_ 	= k1_0_;
	k2_ 	= k2_0_;
	
/*
	me 		= 4000;
	ne 		= 358;

	ns_   	= ne/2;
	k1_0_ 	= ns_ + me;
	k1_   	= k1_0_;
	k2_0_ 	= ns_;
	k2_   	= k2_0_;

	i_    	= 0;
	di_   	= me;
*/
 	outef_  = mixer*gain;
	outsg_  = (1 - mixer)*gain;

	return;
}
 
int32_t GSP_Octave::Process(int32_t sampl, uint32_t buffer_pointer)
{
	/*
    To compute the Octave effect.
		sampl:
			Input sample
		buffer_pointer
			buffer pointer (must be updated by user, once per sample)
		Octave.Process
			Processed output
	*/
 
    static int32_t  sout;
    static uint32_t pt1, pt2;
	static float 	alf1, alf2;

	if (i_ < ns_)
	{
        if (buffer_pointer >= k1_) pt1 	= buffer_pointer - k1_;
        else pt1 	= buffer_size_ - k1_ + buffer_pointer;
        
		sout 		= *(ptr_buffer_ + pt1);
		k1_--;
	}
	else
	{
		if (buffer_pointer >= k1_) pt1 	= buffer_pointer - k1_;
        else pt1 	= buffer_size_ - k1_ + buffer_pointer;
		if (buffer_pointer >= k2_) pt2 	= buffer_pointer - k2_;
        else pt2 	= buffer_size_ - k2_ + buffer_pointer;
		
		alf1 	= sfac_*i_;
		alf2 	= alf1 - 256;
		
        sout 		= sin_[(uint16_t)alf1]*(*(ptr_buffer_ + pt1)) + 
			sin_[(uint16_t)alf2]*(*(ptr_buffer_ + pt2));
        k1_--;
        k2_--;
	}
	
	i_++;
	
	if ((uint32_t)i_ >= k1_0_)
	{
		i_ 		= 0;
		k1_ 	= k1_0_;
		k2_ 	= k2_0_;
	}
	
/*  
    if (i_ < ns_)
    {
        alfa 		= (float)i_/(float)ns_;

        
		if (buffer_pointer >= k2_) pt2 	= buffer_pointer - k2_;
        else pt2 	= buffer_size_ - k2_ + buffer_pointer;
		
        sout 		= alfa*(*(ptr_buffer_ + pt1)) + (1 - alfa)*(*(ptr_buffer_ + pt2));
		
        i_++;
        k1_--;
        k2_--;
    }
    else
    {
        if (buffer_pointer >= k1_) pt1 	= buffer_pointer - k1_;
        else pt1 	= buffer_size_ - k1_ + buffer_pointer;

        sout 		= *(ptr_buffer_ + pt1);

        i_++;
        k1_--;
    }
    if (i_ >= di_)
    {
        i_ 			= 0;
        k1_ 		= k1_0_;
        k2_ 		= k2_0_;
    }
*/
    return (int32_t)(outsg_*sampl + outef_*sout);
}

void GSP_Octave::Printout(uint8_t out_list, int32_t chn_pos, char *printout)
{

    if (out_list == 0)
    {
        sprintf(printout, 
        "->OCT (%ld): OFF(0)|ON(1) %d "
        "| Mixer (0-1): %-.3f "
        "| Gain (0-1): %-.3f\n", 
        chn_pos, state, mixer, 
        gain);
    }
    if (out_list == 1)
    {
        sprintf(printout, 
        "->OCT (%ld) %d %-.3f %-.3f\n", 
        chn_pos, state, mixer, 
        gain);
    }
	
	return;
}

void GSP_Octave::GetParams(float fn[])
{
	
	fn[0]   = state;
	fn[1]   = mixer;
	fn[2]   = gain;
	
	return;
}

void GSP_Octave::SetParams(float fn[])
{

	Switch(fn[0]);
	SetMixer(fn[1]);
	SetGain(fn[2]);

	return;
}

