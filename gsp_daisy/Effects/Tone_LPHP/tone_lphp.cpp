#include <math.h>

#include "tone_lphp.h"

// *****************************************************************************

void GSP_Tone::Init(uint32_t sampling_rate)
{
	/*
    Initiate the Tone Class.
		sampling_rate
			ADC sampling rate (Hz)
	*/
	float 	c1 	= 3.9e-9; 	// 3.9 nF
	float 	c2 	= 10.e-9; 	// 10 nF
	float 	r1 	= 39000.; 	// 39 kOhm
	float 	r2 	= 100000.; 	// 100 kOhm
	float 	p 	= 100000.; 	// 100 kOhm potentiometer
	float 	x1, x2, cbb0, cbb1, cbb2, ab0, ab1, ab2, dbb0, dbb1, dbb2;

	sample_rate 	= sampling_rate;
	
	x1 		= 2*c1*sample_rate;
	x2 		= 2*c2*sample_rate;
	
    cbb2 	= p + r2 - p*r2*x1 - r1*r2*x1;
    cbb1 	= 2*p + 2*r2;
    cbb0 	= p + r2 + p*r2*x1 + r1*r2*x1;
    dbb2 	= p*r1*r2*x1*x2 - p;
    dbb1 	= -2*p - 2*p*r1*r2*x1*x2;
    dbb0 	= p*r1*r2*x1*x2 - p;
    ab2 	= p + r1 + r2 - p*r1*x2 - p*r2*x1 - r1*r2*x1 - r1*r2*x2 + p*r1*r2*x1*x2;
    ab1 	= 2*p + 2*r1 + 2*r2 - 2*p*r1*r2*x1*x2;
    ab0 	= p + r1 + r2 + p*r1*x2 + p*r2*x1 + r1*r2*x1 + r1*r2*x2 + p*r1*r2*x1*x2;
 
    cb2_ 	= cbb2/ab0;
    cb1_ 	= cbb1/ab0;
    cb0_ 	= cbb0/ab0;
    db2_ 	= dbb2/ab0;
    db1_ 	= dbb1/ab0;
    db0_ 	= dbb0/ab0;
    a2_ 	= ab2/ab0;
    a1_ 	= ab1/ab0;
	
	SetTone(0.8);

	return;
}

void  GSP_Tone::SetTone(float tne)
{
	/*
	Set the tone amount for the LPF/HPF filter
		tne
			tonality: 0 (bass) to 1 (treble). 
	*/

	tone   	= fmaxf(fminf(tne, 1), 0); 	// tone
	
	b0_  	=  cb0_ + tone*db0_;
	b1_  	=  cb1_ + tone*db1_;
	b2_  	=  cb2_ + tone*db2_;

	u_out_1_  = 0;
	u_out_2_  = 0;
	u_inp_1_  = 0;
	u_inp_2_  = 0;

	return;
}

int32_t GSP_Tone::Process(int32_t sampl)
{
	/*
    To compute the Tone filter.
		sampl:
			Input sample
		Tone.Process
			Processed output
	*/
 
    u_out_0_ 	= -a1_*u_out_1_ - a2_*u_out_2_ + b0_*sampl + 
        b1_*u_inp_1_ + b2_*u_inp_2_;
    u_out_2_ 	= u_out_1_;
    u_out_1_ 	= u_out_0_;
    u_inp_2_ 	= u_inp_1_;
    u_inp_1_ 	= sampl;

    return u_out_0_;  
}


