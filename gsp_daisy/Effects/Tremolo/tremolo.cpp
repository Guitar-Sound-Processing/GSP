#include <math.h>
#include <stdio.h>

#include "tremolo.h"

// *****************************************************************************

void GSP_Tremolo::Init(uint32_t sampling_rate)
{
	/*
    Initiate the Tremolo Class.
		sampling_rate
			ADC sampling rate (Hz)
	*/

	sample_rate = sampling_rate;
	
	lfo.Init(sample_rate);
	SetGain(1.);
	Switch(GSP_OFF);

	return;
}

void GSP_Tremolo::Switch(uint8_t mode)
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

void GSP_Tremolo::SetGain(float output_gain)
{
	/*
    To set the output gain of Tremolo effect
		output_gain
			output gain of Tremolo effect (> 0.1)
	*/

	gain 		= fmaxf(output_gain, 0.1);

	return;
}

int32_t GSP_Tremolo::Process(int32_t sampl)
{
	/*
    To compute the Tremolo effect on input signal.
		sampl:
			Input sample
		Tremolo.Process
			Processed output
	*/
 
	//check 	= lfo.Amplitude();
	return ((int32_t)(sampl * gain * lfo.GetValue())) >> 16;
}

void GSP_Tremolo::Printout(uint8_t out_list, int32_t chn_pos, char *printout)
{

    if (out_list == 0)
    {
        sprintf(printout, 
        "->TML (%ld): OFF(0)|ON(1) %d "
    	"| Profile: (0-10) %d "
   	    "| Frequency (0.2-5)(Hz): %-.3f "
       	"| Duty Cycle (0-100)(%%): %-.1f "
        "| Gain (0-1): %-.3f\n", 
        chn_pos, state,
        lfo.profile, 1000./(float)lfo.period, lfo.duty, 
        gain);
    }
    if (out_list == 1)
    {
        sprintf(printout, 
        "->TML (%ld) %d %d %-.3f %-.1f %-.3f\n", 
        chn_pos, state,
        lfo.profile, 1000./(float)lfo.period, lfo.duty, 
        gain);
    }
	
	return;
}

void GSP_Tremolo::GetParams(float fn[])
{
	
	fn[0] 	= state;
	fn[1] 	= lfo.profile;
	fn[2] 	= 1000./(float)lfo.period;
	fn[3] 	= lfo.duty;
	fn[4] 	= gain;
	
	return;
}

void GSP_Tremolo::SetParams(float fn[])
{

	Switch(fn[0]);
	lfo.SetProfile((uint8_t)fn[1]);
	lfo.SetFrequency(fn[2]);
	lfo.SetDutyCycle(fn[3]);
	SetGain(fn[4]);

	return;
}

