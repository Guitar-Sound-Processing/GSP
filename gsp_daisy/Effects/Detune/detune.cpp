#include <math.h>
#include <stdio.h>

#include "detune.h"

// *****************************************************************************

void GSP_Detune::Init(uint32_t sampling_rate, int16_t *ptr_buffer, uint32_t buffer_size)
{
	/*
    Initiate the Detune Class.
		sampling_rate
			ADC sampling rate (Hz)
		*ptr_buffer
			buffer to store the processed sample. Detune.Process
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
	SetDetune(5.);
	ComputeParameters();
	Switch(GSP_OFF);
	
	return;
}

void GSP_Detune::SetDetune(float dtne)
{
	/*
    To set the tone shift of Detune effect
		dtne
			tone shift (down) in semitones (0 to 12)
	*/

	detune  	= fmaxf(fminf(dtne, 12.), 0.);

	return;
}

void GSP_Detune::SetGain(float out_gain)
{
	/*
    To set the output gain of Detune effect
		out_gain
			output gain of Detune effect (> 0.1)
	*/

	gain 		= fmaxf(out_gain, 0.1);
	ComputeParameters();

	return;
}

void GSP_Detune::SetMixer(float mix)
{
	/*
    To mix the Detune output with the main signal.
		mix
			fraction of mixer. A mix zero has no shift, although
			the signal is still present on output, while mix equals 
			to 1 has only the detune signal
	*/

	mixer 	= fmaxf(fminf(mix, 1), 0);
	ComputeParameters();
	
	return;
}

void GSP_Detune::Switch(uint8_t mode)
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

void  GSP_Detune::ComputeParameters()
{
	/*
    To compute the Detune parameters.
	*/

	float p;
	uint32_t  me, ms;
  
	outef_ 		= mixer*gain;
	outsg_ 		= (1 - mixer)*gain;
  
	p   		= powf(2, detune/12); // frequency ratio

	me 			= 4000;
	ms    		= (float)me*p;  // n. de amostras da janela expandida 
	mp_ 		= ms - me;

	k1_0_ 		= 0;
	k1_ 		= k1_0_;
	k2_0_ 		= (float)me*(p - 1)/p;
	k2_ 		= k2_0_;

	dk_ 		= (float)me*(p - 1)/p;
	di_ 		= me;

	i_ 			= 0;
	d1_ 		= 2*dk_ - di_;
	d2_ 		= 2*(dk_ - di_);
	d3_ 		= 2*dk_;
	d_ 			= d1_;

	return;
}
 
int32_t GSP_Detune::Process(int32_t sampl, uint32_t buffer_pointer)
{
	/*
    To compute the Detune effect.
		sampl:
			Input sample
		buffer_pointer
			buffer pointer (must be updated by user, once per sample)
		Detune.Process
			Processed output
	*/
 
    static int32_t    sout;
    static uint32_t   pt1, pt2;
    static float      alfa;
  
    if (i_ < mp_)
    {
        // mixing window
        alfa 		= (float)i_/mp_;
		
        if (buffer_pointer >= k1_) pt1 	= buffer_pointer - k1_;
        else pt1 	= buffer_size_ - k1_ + buffer_pointer;
		
        if (buffer_pointer >= k2_) pt2 	= buffer_pointer - k2_;
        else pt2 	= buffer_size_ - k2_ + buffer_pointer;
		
        sout    = alfa*(*(ptr_buffer_ + pt1)) + (1 - alfa)*(*(ptr_buffer_ + pt2));
        i_++;
		
        if (d_ > 0)
        {
			k1_++;
            k2_++;
            d_ 		+= d2_;
        }
        else d_ 	+= d3_;
    }
    else
    {
        // single window
        if (buffer_pointer >= k1_) pt1   = buffer_pointer - k1_;
        else pt1  = buffer_size_ - k1_ + buffer_pointer;
        
		sout    = (*(ptr_buffer_ + pt1));
        i_++;
		
        if (d_ > 0)
        {
			k1_++;
            d_ 		+= d2_;
        }
        else d_ 	+= d3_;
    }
	
    if (i_ >= di_)
    {
        i_ 		= 0;
        k1_ 	= k1_0_;
        k2_ 	= k2_0_;
        d_ 		= d1_;
    }
    
    return (int32_t)(outsg_*sampl + outef_*sout);
}

void GSP_Detune::Printout(uint8_t out_list, int32_t chn_pos, char *printout)
{

    if (out_list == 0)
    {
        sprintf(printout, 
        "->DTN (%ld): OFF(0)|ON(1) %d "
        "| Detune (down) (0-12): %-.3f "
        "| Mixer (0-1): %-.3f "
        "| Gain (0-1): %-.3f\n", 
        chn_pos, state, detune, mixer, 
        gain);
    }
    if (out_list == 1)
    {
        sprintf(printout, 
        "->DTN (%ld) %d %-.3f %-.3f %-.3f\n", 
        chn_pos, state, detune, mixer, 
        gain);
    }
	
	return;
}

void GSP_Detune::GetParams(float fn[])
{
	
	fn[0]   = state;
	fn[1]   = detune;
	fn[2]   = mixer;
	fn[3]   = gain;
	
	return;
}

void GSP_Detune::SetParams(float fn[])
{

	Switch(fn[0]);
	SetDetune(fn[1]);
	SetMixer(fn[2]);
	SetGain(fn[3]);

	return;
}



