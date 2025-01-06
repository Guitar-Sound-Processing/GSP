#include <math.h>
#include <stdio.h>

#include "wahwah.h"
//**#include "DaisyDuino.h"

// *****************************************************************************

void GSP_WahWah::Init(uint32_t sampling_rate)
{
	/*
    Initiate the WahWah Class.
		sampling_rate
			ADC sampling rate (Hz)
	*/

	float dt, dt2, l, r;
	
 	sample_rate 	= sampling_rate;
  
	lfo.Init(sample_rate);
	SetGain(1.);

	dt 		= 1./sample_rate;
	dt2		= dt*dt;

	l 		= 2*0.600; 	// indutance (mH)
	r 		= 33000;	// resistance (Ohm)

	//C0  = 6.8e-9;
	//C1  = 93.2e-9;

	c0_		= 15e-9;	// minimum capacitance (F)
	c1_		= 95e-9/(float)lfo.GetAmplitude();	// increment capacitance (F)

	bb2_	= r*dt2;
	bb1_ 	= 2*r*dt2;
	bb0_ 	= r*dt2;
  
	a2_0_ 	=  r*dt2 - 2*l*dt;
	a2_1_ 	=  4*l*r;
	a1_0_ 	=  2*r*dt2; 
	a1_1_ 	= -8*l*r;
	a0_0_ 	=  r*dt2 + 2*l*dt;
	a0_1_ 	=  4*l*r;

	s1_ 	= 0;
	s2_ 	= 0;
	e1_ 	= 0;
	e2_ 	= 0;

	Switch(GSP_OFF);
	
	return;
}

void GSP_WahWah::SetGain(float input_gain)
{
	/*
    To set the input gain of WahWah effect
		input_gain
			input gain of WahWah effect (> 0.1)
	*/

	gain 		= fmaxf(input_gain, 0.1);
	ampl_ 		= 0.6*gain;

	return;
}

void GSP_WahWah::Switch(uint8_t mode)
{
	/*
    To switch the effect on and off.
		mode
			Condition switch: ON (1) or OFF (0)
	*/

	state 		= GSP_ON;
	if (mode == GSP_OFF) state = GSP_OFF;
	
	return;

}

int32_t GSP_WahWah::Process(int32_t sampl)
{
	/*
    To compute the WahWah effect.
		sampl:
			Input sample
		WahWah.Process
			Processed output
	*/
 
    static float e0, s0, c, aa0, aa1, aa2;
    
    e0 		= sampl;
    c 		= c0_ + lfo.GetValue()*c1_;
    aa2 	= a2_0_ + a2_1_*c;
    aa1 	= a1_0_ + a1_1_*c;
    aa0 	= a0_0_ + a0_1_*c;
    s0 		= (bb0_*e0 + bb1_*e1_ + bb2_*e2_ - aa1*s1_ - aa2*s2_)/aa0;
    s2_ 	= s1_;
    s1_ 	= s0;
    e2_ 	= e1_;
    e1_ 	= e0;

    return (int32_t)(ampl_*s0);
}

void GSP_WahWah::Printout(uint8_t out_list, int32_t chn_pos, char *printout)
{

    if (out_list == 0)
    {
        sprintf(printout, 
        "->WAH (%ld): OFF(0)|ON(1) %d "
        "| Profile: (0-10) %d "
        "| Frequency (0.2-5)(Hz): %-.3f "
        "| Duty Cycle (0-100)(%%): %-.1f "
        "| Gain (0-1): %-.3f\n", 
        chn_pos, state, lfo.profile, 1000./(float)lfo.period, 
        lfo.duty, gain);
    }
    if (out_list == 1)
    {
        sprintf(printout, 
        "->WAH (%ld) %d %d %-.3f %-.1f %-.3f\n", 
        chn_pos, state, lfo.profile, 1000./(float)lfo.period, 
        lfo.duty, gain);
    }
	
	return;
}

void GSP_WahWah::GetParams(float fn[])
{
	
	fn[0]   = state;
	fn[1]   = lfo.profile;
	fn[2]   = 1000./(float)lfo.period;
	fn[3]   = lfo.duty;
	fn[4]   = gain;
	
	return;
}

void GSP_WahWah::SetParams(float fn[])
{

	Switch(fn[0]);
	lfo.SetProfile((uint8_t)fn[1]);
	lfo.SetFrequency(fn[2]);
	lfo.SetDutyCycle(fn[3]);
	SetGain(fn[4]);

	return;
}



