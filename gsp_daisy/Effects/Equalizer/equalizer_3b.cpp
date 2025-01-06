// *****************************************************************************
// 	3 Band Equalizer

#include <math.h>
#include <stdio.h>

#include "equalizer_3b.h"

void GSP_Equalizer::Init(uint32_t sampling_rate)
{
	/*
    Initiate the Equalizer Class.
		sampling_rate
			ADC sampling rate (Hz)
	*/

	sample_rate		= sampling_rate;
	gain_low 		= 1.;
	gain_medium 	= 1.;
	gain_high 		= 1.;
	freq_low		= 200;
	freq_high		= 800;
	SetGains(gain_low, gain_medium, gain_high);
	SetFilterFrequencies(freq_low, freq_high);
	Switch(GSP_OFF);
	
	return;
}

void GSP_Equalizer::SetGains(float g_low, float g_medium, float g_high)
{
	/*
    To set the Equalizer gains (0. to 1.).
		g_low, g_medium, g_high
			equalizer gains at low, medium and high frequency 
			(between 0 and 1)
	*/

	gain_low    = fmaxf(g_low, 0.);
	gain_medium = fmaxf(g_medium, 0.);
	gain_high   = fmaxf(g_high, 0.);

	ComputeFilter();
	
	return;

}

void GSP_Equalizer::SetFilterFrequencies(float f_low, float f_high)
{
	/*
    To set the Equalizer cut-off frequencies (100. to 2000.).
		f_low, f_high
			cut-off frequencies of the equalizer filters (Hz):
			100 < f_low < f_high < 2000
	*/

	float fl;

	freq_low    = f_low;
	freq_high   = f_high;
	if (freq_low > freq_high)
	{
		fl        = freq_low;
		freq_low  = freq_high;
		freq_high = fl;
	}
	if (freq_low == freq_high)
	{
		freq_low  = 200.;
		freq_high = 800.;
	}
  
	if (freq_low < 100) freq_low = 100.;
	if (freq_high > 2000) freq_high = 2000.;

	ComputeFilter();
	
	return;

}

void GSP_Equalizer::ComputeFilter()
{
	/*
    To compute the filter parameters.
	*/

	float dt, tb, th, t1b, t1h;
	float a_l_0, a_h_0, a_1_0;

	dt		= 1./sample_rate;

	tb		= 1./2./GDSP_PI/freq_low;  // Time constant of the low pass filter (LPF)
	th		= 1./2./GDSP_PI/freq_high; // Time constant of the high pass filter (HPF)
	t1b		= th;                 // Time constant of the LPF in the band pass filter
	t1h		= tb;                 // Time constant of the HPF in the band pass filter

	// LPF 	- Low pass filter
	a_l_0		= dt + 2*tb;
	b_l_1_		= dt/a_l_0;
	b_l_0_		= dt/a_l_0;
	a_l_1_		= (dt - 2*tb)/a_l_0;
	
	// HPF 	- High pass filter
	a_h_0		= -dt - 2*th;
	b_h_1_		= 2*th/a_h_0;
	b_h_0_		= -2*th/a_h_0;
	a_h_1_		= (2*th - dt)/a_h_0;

	// Band Pass Filter
	a_1_0		= -dt*dt - 2*t1b*dt - 2*t1h*dt - 4*t1b*t1h;
	b_1_2_		= 2*t1h*dt/a_1_0;
	b_1_1_		= 0;
	b_1_0_		= -2*t1h*dt/a_1_0;
	a_1_2_		= (2*t1b*dt - dt*dt + 2*t1h*dt - 4*t1b*t1h)/a_1_0;
	a_1_1_		= (8*t1b*t1h - 2*dt*dt)/a_1_0;

	// Equalizer coeficients (Z transform) (simplified version)
	/*
	a_0_		= a_l_0*gain_low + a_1_0*gain_medium + a_h_0*gain_high;
	b_2_		= b_1_2*gain_medium/a_0_;
	b_1_		= (b_l_1*gain_low + b_1_1*gain_medium + b_h_1*gain_high)/a_0_;
	b_0_		= (b_l_0*gain_low + b_1_0*gain_medium + b_h_0*gain_high)/a_0_;
	a_2_		= a_1_2*gain_medium/a_0_;
	a_1_		= (a_l_1*gain_low + a_1_1*gain_medium + a_h_1*gain_high)/a_0_;
	a_0_		= 1;
	*/
	
	// Initiate variables
	u_inp_0_	= 0;
	u_inp_1_	= 0;
	u_inp_2_	= 0;
	u_out_l_1_	= 0;
	u_out_1_1_	= 0;
	u_out_1_2_	= 0;
	u_out_h_1_	= 0;
 	
	return;

}

void GSP_Equalizer::Switch(uint8_t mode)
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

int32_t GSP_Equalizer::Process(int32_t sampl)
{
	/*
    Compute the equalizer filter.
		sampl:
			Input sample
		Equalizer.Process
			Processed output
	*/

	u_inp_0_ 		= sampl;
	u_out_l_0_		= b_l_0_*u_inp_0_ + b_l_1_*u_inp_1_ - a_l_1_*u_out_l_1_;
	u_out_1_0_		= b_1_0_*u_inp_0_ + b_1_1_*u_inp_1_ + b_1_2_*u_inp_2_ 
					- a_1_1_*u_out_1_1_ - a_1_2_*u_out_1_2_;
	u_out_h_0_		= b_h_0_*u_inp_0_ + b_h_1_*u_inp_1_ - a_h_1_*u_out_h_1_;
	u_inp_2_		= u_inp_1_;
	u_inp_1_		= u_inp_0_;
	u_out_l_1_ 		= u_out_l_0_;
	u_out_1_2_ 		= u_out_1_1_;
	u_out_1_1_ 		= u_out_1_0_;
	u_out_h_1_ 		= u_out_h_0_;

	return 	gain_low*u_out_l_0_ + gain_medium*u_out_1_0_ + gain_high*u_out_h_0_;;

}

void GSP_Equalizer::Printout(uint8_t out_list, int32_t chn_pos, char *printout)
{

    if (out_list == 0)
    {
        sprintf(printout, 
        "->EQZ (%ld): OFF(0)|ON(1) %d "
        "| Gains (0-1): Low %-.3f "
        ", Medium  %-.3f "
        ", High %-.3f "
        "| Cutoff frequencies (Hz): Low (200-) %-.3f "
        ", High (-2000) %-.1f\n", 
        chn_pos, state, gain_low, gain_medium, gain_high,
        freq_low, freq_high);
    }
    if (out_list == 1)
    {
        sprintf(printout, 
        "->EQZ (%ld) %d %-.3f %-.3f %-.3f %-.3f %-.1f\n", 
        chn_pos, state, gain_low, gain_medium, gain_high,
        freq_low, freq_high);
    }
	
	return;
}

void GSP_Equalizer::GetParams(float fn[])
{
	
	fn[0]   = state;
	fn[1]   = gain_low;
	fn[2]   = gain_medium;
	fn[3]   = gain_high;
	fn[4]   = freq_low;
	fn[5]   = freq_high;
	
	return;
}

void GSP_Equalizer::SetParams(float fn[])
{

	Switch(fn[0]);
	SetGains(fn[1], fn[2], fn[3]);
	SetFilterFrequencies(fn[4], fn[5]);

	return;
}



