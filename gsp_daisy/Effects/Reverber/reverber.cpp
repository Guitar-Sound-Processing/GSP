#include <math.h>
#include <stdio.h>

#include "reverber.h"

//**#include "DaisyDuino.h"

// *****************************************************************************

void GSP_Reverber::Init(uint32_t sampling_rate, int16_t *ptr_buffer, uint32_t buffer_size)
{
	/*
    Initiate the Reverber Class.
		sampling_rate
			ADC sampling rate (Hz)
		*ptr_buffer
			buffer to store the processed sample. Shall store at least 8000 
			values uint16_t, and shall be multiple of 4. This buffer is for 
			reverber use only and shall not be shared with other effects.
		buffer_size
			buffer size (at least 8000).
	*/

	sample_rate 	= sampling_rate;
	buffer_size_ 	= buffer_size/4;
	ptr_buffer_0 	= ptr_buffer;
	ptr_buffer_1 	= ptr_buffer + buffer_size_;
	ptr_buffer_2 	= ptr_buffer + 2*buffer_size_;
	ptr_buffer_3 	= ptr_buffer + 3*buffer_size_;
	
	SetReverberTimeMS(1000.);
	SetGain(1.);
	Switch(GSP_OFF);
	ComputeParameters();
	
	return;
}

void GSP_Reverber::SetReverberTimeMS(float rvb_ms)
{
	/*
    To set the reverber time in milliseconds.
		rvb_ms
			reverber time in milliseconds (0 < rvb_ms < 20000).
			
	*/

	reverber_time 	= fmaxf(fminf(rvb_ms, 20000.), 0.);
	ComputeParameters();

	return;
}

void GSP_Reverber::SetGain(float output_gain)
{
	/*
    To set the output gain of Reverber effect
		output_gain
			gain of Reverber (> 0.1).
 	*/

	gain 		= fmaxf(output_gain, 0.1);
	ComputeParameters();

	return;
}

void GSP_Reverber::Switch(uint8_t mode)
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

void GSP_Reverber::ComputeParameters()
{
    /*
    To compute the Reverber parameters and filter
    */

    float     t60fs, t60a0, bb;
    float     r0Mi_0, r0Mi_1, r0Mi_2, r0Mi_3;
    float     rpiMi_0, rpiMi_1, rpiMi_2, rpiMi_3; 
    float     gip_0, gip_1, gip_2, gip_3;
    uint16_t  i;
    
    t60fs   	= 0.1;  	// decay time at frequency sampler_rate/2
    t60a0   	= reverber_time/1000.;    // decay time (s) for a dc signal

	cinv_ 		= 1./(float)conf_;
	
    // H filter
    r0Mi_0 		= powf(10, -3.*Mi_0_/t60a0/(float)sample_rate);
    r0Mi_1 		= powf(10, -3.*Mi_1_/t60a0/(float)sample_rate);
    r0Mi_2 		= powf(10, -3.*Mi_2_/t60a0/(float)sample_rate);
    r0Mi_3 		= powf(10, -3.*Mi_3_/t60a0/(float)sample_rate);

    gip_0 		= r0Mi_0;
    gip_1 		= r0Mi_1;
    gip_2 		= r0Mi_2;
    gip_3 		= r0Mi_3;
    
    rpiMi_0 	= powf(10, (-3.*Mi_0_/t60fs/(float)sample_rate));
    rpiMi_1 	= powf(10, (-3.*Mi_1_/t60fs/(float)sample_rate));
    rpiMi_2 	= powf(10, (-3.*Mi_2_/t60fs/(float)sample_rate));
    rpiMi_3 	= powf(10, (-3.*Mi_3_/t60fs/(float)sample_rate));

    pip_0_ 		= conf_*(r0Mi_0 - rpiMi_0)/(r0Mi_0 + rpiMi_0);
    pip_1_ 		= conf_*(r0Mi_1 - rpiMi_1)/(r0Mi_1 + rpiMi_1);
    pip_2_ 		= conf_*(r0Mi_2 - rpiMi_2)/(r0Mi_2 + rpiMi_2);
    pip_3_ 		= conf_*(r0Mi_3 - rpiMi_3)/(r0Mi_3 + rpiMi_3);

    pip1_0_ 	= 0.5*gip_0*(conf_ - pip_0_);	// 0.5 comes from A matrix
    pip1_1_ 	= 0.5*gip_1*(conf_ - pip_1_);
    pip1_2_ 	= 0.5*gip_2*(conf_ - pip_2_);
    pip1_3_ 	= 0.5*gip_3*(conf_ - pip_3_);

    // Tone corrector
    bb  		= (1 - alfa_)/(1 + alfa_);
    bb_ 		= conf_*bb;
    bbinv_ 		= cinv_/(1 - bb);

    // Initialize buffers
    for (i = 0; i < buffer_size_; i++)
    {
        ptr_buffer_0[i] 	= 0;  // xi
        ptr_buffer_1[i] 	= 0;  // xi
        ptr_buffer_2[i] 	= 0;  // xi
        ptr_buffer_3[i] 	= 0;  // xi
    }

    rim1_0_ 	= 0;  // rim1
    rim1_1_ 	= 0;  // rim1
    rim1_2_ 	= 0;  // rim1
    rim1_3_ 	= 0;  // rim1

    ynm1_ 		= 0;
    n_ 			= 0;

    // Y: A   = 1/2*[1 1 1 1; 1 -1 1 -1; 1 1 -1 -1; 1 -1 -1 1];

    //_c_0      = 1;  // c
    //_c_1      = 1;  // c
    //_c_2      = 1;  // c
    //_c_3      = 1;  // c

	return;
}

int32_t GSP_Reverber::Process(int32_t sampl)
{
    /*
    To compute the Reverber effect.
		sampl:
			Input sample
		Process
			Processed output
   */

    static uint32_t nMi;
    static int32_t  w_0, w_1, w_2, w_3;
    static int32_t  awp01, awp23, awd01, awd23;
    static int32_t  aw_0, aw_1, aw_2, aw_3;
    static int32_t  ri_0, ri_1, ri_2, ri_3;
    static float    yn;
    static int32_t  sout;

	// Get the delay lines
    if (n_ < Mi_0_)  nMi   = n_ - Mi_0_ + buffer_size_;
    else  nMi     = n_ - Mi_0_;
    w_0 		= *(ptr_buffer_0 + nMi);
    if (n_ < Mi_1_)  nMi   = n_ - Mi_1_ + buffer_size_;
    else  nMi     = n_ - Mi_1_;
    w_1 		= *(ptr_buffer_1 + nMi);
    if (n_ < Mi_2_)  nMi   = n_ - Mi_2_ + buffer_size_;
    else  nMi     = n_ - Mi_2_;
    w_2 		= *(ptr_buffer_2 + nMi);
    if (n_ < Mi_3_)  nMi   = n_ - Mi_3_ + buffer_size_;
    else  nMi     = n_ - Mi_3_;
    w_3 		= *(ptr_buffer_3 + nMi);

	// Multiply by the A matrix
    // Y: A   = 1/2*[1 1 1 1; 1 -1 1 -1; 1 1 -1 -1; 1 -1 -1 1];
    awp01 		= w_0 + w_1;    // 2*A*_w (1/2 factor went to pip1_i_)
    awp23 		= w_2 + w_3;
    awd01 		= w_0 - w_1;
    awd23 		= w_2 - w_3;
    
    aw_0 		= awp01 + awp23;  // 2*A*_w
    aw_1 		= awd01 + awd23;
    aw_2 		= awp01 - awp23;
    aw_3 		= awd01 - awd23;
    
	// Low Pass Filter Hi(z)
    ri_0 		= (float)(pip1_0_*aw_0 + pip_0_*rim1_0_)*cinv_;
    ri_1 		= (float)(pip1_1_*aw_1 + pip_1_*rim1_1_)*cinv_;
    ri_2 		= (float)(pip1_2_*aw_2 + pip_2_*rim1_2_)*cinv_;
    ri_3 		= (float)(pip1_3_*aw_3 + pip_3_*rim1_3_)*cinv_;
	
	rim1_0_		= ri_0;
	rim1_1_		= ri_1;
	rim1_2_		= ri_2;
	rim1_3_		= ri_3;
	
	// Input to the delay lines
	ri_0    	+= sampl;
    ri_1    	+= sampl;
    ri_2    	+= sampl;
    ri_3    	+= sampl;

	if (ri_0 > ADC_MAXVAL)	ri_0 = ADC_MAXVAL;
	if (ri_0 < ADC_MINVAL)	ri_0 = ADC_MINVAL;
	if (ri_1 > ADC_MAXVAL)	ri_1 = ADC_MAXVAL;
	if (ri_1 < ADC_MINVAL)	ri_1 = ADC_MINVAL;
	if (ri_2 > ADC_MAXVAL)	ri_2 = ADC_MAXVAL;
	if (ri_2 < ADC_MINVAL)	ri_2 = ADC_MINVAL;
	if (ri_3 > ADC_MAXVAL)	ri_3 = ADC_MAXVAL;
	if (ri_3 < ADC_MINVAL)	ri_3 = ADC_MINVAL;
	
	// Store the delay on buffer
    ptr_buffer_0[n_] 	= ri_0;
    ptr_buffer_1[n_] 	= ri_1;
    ptr_buffer_2[n_] 	= ri_2;
    ptr_buffer_3[n_] 	= ri_3;

	// Combine outputs
	yn 			= w_0 + w_1 + w_2 + w_3;
	
	// Apply tonal corrector
//    sout 		= gain*0.25*((float)conf_*yn - (float)bb_*ynm1_)*bbinv_;
    sout 		= gain*((float)conf_*yn - (float)bb_*ynm1_)*bbinv_;
	if (sout > ADC_MAXVAL)	sout = ADC_MAXVAL;
	if (sout < ADC_MINVAL)	sout = ADC_MINVAL;

    ynm1_  		= yn;

    n_++;
    if (n_ >= buffer_size_) n_ 	= 0;

    return sout;
}

void GSP_Reverber::Printout(uint8_t out_list, int32_t chn_pos, char *printout)
{

    if (out_list == 0)
    {
        sprintf(printout, 
        "->RVB (%ld): OFF(0)|ON(1) %d "
        "| Reverber Time (0-20000)(ms): %-.1f "
        "| Gain (0-1): %-.3f\n", 
        chn_pos, state, reverber_time,
        gain);
    }
    if (out_list == 1)
    {
        sprintf(printout, 
        "->RVB (%ld) %d %-.1f %-.3f\n", 
        chn_pos, state, reverber_time,
        gain);
    }
	
	return;
}

void GSP_Reverber::GetParams(float fn[])
{
	
	fn[0]   = state;
	fn[1]   = reverber_time;
	fn[2]   = gain;
	
	return;
}

void GSP_Reverber::SetParams(float fn[])
{

	Switch(fn[0]);
	SetReverberTimeMS(fn[1]);
	SetGain(fn[2]);

	return;
}

 
