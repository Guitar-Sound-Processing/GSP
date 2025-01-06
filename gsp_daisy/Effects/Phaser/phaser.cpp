#include <math.h>
#include <stdio.h>

#include "phaser.h"

// *****************************************************************************

void GSP_Phaser::Init(uint32_t sampling_rate)
{
	/*
    Initiate the Phaser Class.
		sampling_rate
			ADC sampling rate (Hz)
	*/

	sample_rate 	= sampling_rate;
	
	lfo.Init(sample_rate);
	lfo.SetProfile(LFO_SIN);
	lfo.SetPeriod(4000);
	
	SetGain(1.);
	SetDepth(0.5);
	SetLevel(10);
	Switch(GSP_OFF);

	dt_			= 1./sample_rate;

	u0_ 		= 0;
	u1_ 		= 0;
	u2_ 		= 0;
	u3_ 		= 0;
	u4_ 		= 0; 
	u5_ 		= 0;
	u6_ 		= 0;
	u7_ 		= 0;
	u8_ 		= 0; 
	u9_ 		= 0;
	u10_ 		= 0;

	return;
}

void GSP_Phaser::SetLevel(float lvl)
{
	/*
    To set the level for the Phaser effect
		lvl
			The level is the maximum amplitude of the effect
	*/

	level 		= fmax(fmin(lvl, 1000.), 0.);;
	am_   		= 5e-5*level/(float)lfo.GetAmplitude(); // 0 a 0.05 linear, effect level
  
	return;
}

void GSP_Phaser::SetDepth(float dpth)
{
	/*
    To set the Depth for the Phaser effect
		dpth
			Effect depth (0 to 1).
	*/

	depth 		= fmax(fmin(dpth, 1.), 0.);
	bm_  		= 1. - (1. - exp(-10.*depth))/(1. - exp(-10.));

	return;
}

void GSP_Phaser::SetGain(float output_gain)
{
	/*
    To set the output gain of Phaser effect
		output_gain
			output gain of Phaser effect (> 0.1)
	*/

	gain 		= fmaxf(output_gain, 0.1);
 	gain_ef_  	= 0.7*gain;

	return;
}

void GSP_Phaser::Switch(uint8_t mode)
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

int32_t GSP_Phaser::Process(int32_t sampl)
{
	/*
    To compute the Phaser effect on input signal.
		sampl:
			Input sample
		Phaser.Process
			Processed output
	*/
 
	static float rr, aa, bb, b;
	static int32_t u0, u1, u2, u3, u4, u5, u6, u7, u8, u9, u10, s0;
  
	rr    	= am_*lfo.GetValue() + bm_;

	aa    	= rr + dt_;
	bb    	= rr - dt_;
	b     	= bb/aa;
	u0    	= sampl;
	u1    	= b*(u0 + u1_) - u0_;
	u2    	= b*(u1 + u2_) - u1_;
	u3    	= b*(u2 + u3_) - u2_;
	u4    	= b*(u3 + u4_) - u3_;
	u5    	= b*(u4 + u5_) - u4_;
	u6    	= b*(u5 + u6_) - u5_;
	u7    	= b*(u6 + u7_) - u6_;
	u8   	= b*(u7 + u8_) - u7_;
	u9    	= b*(u8 + u9_) - u8_;
	u10   	= b*(u9 + u10_) - u9_;
	u0_  	= u0;
	u1_  	= u1;
	u2_  	= u2;
	u3_  	= u3;
	u4_  	= u4;
	u5_  	= u5;
	u6_  	= u6;
	u7_  	= u7;
	u8_  	= u8;
	u9_  	= u9;
	u10_ 	= u10;
	s0    	= gain_ef_*(u0 + u10);

	return s0;  // inserir mixer
}

void GSP_Phaser::Printout(uint8_t out_list, int32_t chn_pos, char *printout)
{
    if (out_list == 0)
    {
        sprintf(printout, 
        "->PHR (%ld): OFF(0)|ON(1) %d "
        "| Depth (0-1): %-.3f "
        "| Level (0-1000): %-.1f "
        "| Profile: (0-10) %d "
        "| Frequency (0.2-5)(Hz): %-.3f "
        "| Duty Cycle (0-100)(%%): %-.1f "
        "| Gain (0-1): %-.3f\n", 
        chn_pos, state, depth, level, lfo.profile,
        1000./(float)lfo.period, lfo.duty, gain);
    }
    if (out_list == 1)
    {
        sprintf(printout, 
        "->PHR (%ld) %d %-.3f %-.1f %d %-.3f %-.1f %-.3f\n", 
        chn_pos, state, depth, level, lfo.profile,
        1000./(float)lfo.period, lfo.duty, gain);
    }
	
	return;
}

void GSP_Phaser::GetParams(float fn[])
{
	
	fn[0]   = state;
	fn[1]   = depth;
	fn[2]   = level;
	fn[3]   = lfo.profile;
	fn[4]   = 1000./(float)lfo.period;
	fn[5]   = lfo.duty;
	fn[6]   = gain;
	
	return;
}

void GSP_Phaser::SetParams(float fn[])
{

	Switch(fn[0]);
	SetDepth(fn[1]);
	SetLevel(fn[2]);
	lfo.SetProfile((uint8_t)fn[3]);
	lfo.SetFrequency(fn[4]);
	lfo.SetDutyCycle(fn[5]);
	SetGain(fn[6]);

	return;
}


