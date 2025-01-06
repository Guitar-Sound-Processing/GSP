#include <math.h>
#include <stdio.h>

#include "limiter.h"

//**#include "DaisyDuino.h"

// *****************************************************************************

void GSP_Limiter::Init(uint32_t sampling_rate)
{
	/*
    Initiate the Limiter Class.
		sampling_rate
			ADC sampling rate (Hz)
	*/

	sample_rate_ 	= sampling_rate;
	
	adc_limit_ 		= ADC_HALFRES - 1;
	
	SetSmooth(1.);
	SetGain(1.);
	Switch(GSP_OFF);
	
	return;
}

void GSP_Limiter::SetGain(float i_gain)
{
	/*
    To set the input gain of soft limiter
		i_gain
			input gain of limiter (> 0.1).
 	*/

	input_gain 		= fmaxf(i_gain, 0.1);

	return;
}

void GSP_Limiter::SetSmooth(float smth)
{
	/*
    To set the smooth factor for the soft limiter
		smth
			smooth factor (0 : 2)
 	*/

	smooth 		= fmaxf(fminf(smth, 1), 0);
	wl_			= (float)adc_limit_*smooth;
	wl4_ 		= 4*wl_;
	limpw_ 		= adc_limit_ + wl_;
	limmw_ 		= adc_limit_ - wl_;

	return;
}

void GSP_Limiter::Switch(uint8_t mode)
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

int32_t GSP_Limiter::Process(int32_t sampl)
{
    /*
    To compute the soft limiter effect.
		sampl:
			Input sample
		Process
			Processed output
    */

    static int32_t  sout, a_smpl, strg;

	a_smpl 	= input_gain*(float)sampl;
	sout 	= sampl;
	
    if (a_smpl > 0)
	{
        if (a_smpl < limmw_)
		{
			sout	= a_smpl;       // linear
		}
        else
		{
            if (a_smpl > limpw_)
			{
				sout	= adc_limit_;	// threshold
			}
			else
			{
				strg 	= a_smpl - limmw_;		// smooth
				sout	= a_smpl - strg*strg/wl4_;
            }
        }
	}
    else
	{
        if (a_smpl > -limmw_)
		{
			sout	= a_smpl;       // linear
        }
		else
		{
            if (a_smpl < -limpw_)
			{
				sout	= -adc_limit_;	// threshold
			}
			else
			{
				strg 	= -a_smpl - limmw_;		// smooth
				sout	= a_smpl + strg*strg/wl4_;
            }
        }
    }

    return sout;
}

void GSP_Limiter::Printout(uint8_t out_list, int32_t chn_pos, char *printout)
{

    if (out_list == 0)
    {
        sprintf(printout, 
        "->LIM (%ld): OFF(0)|ON(1) %d "
        "| Smooth factor (0-1): %-.3f "
        "| Gain (0- ): %-.3f\n", 
        chn_pos, state,
        smooth, 
        input_gain);
    }
    if (out_list == 1)
    {
        sprintf(printout, 
        "->LIM (%ld) %d %-.3f %-.3f\n", 
        chn_pos, state,
        smooth, 
        input_gain);
    }
	
	return;
}

void GSP_Limiter::GetParams(float fn[])
{
	
	fn[0]   = state;
	fn[1]   = smooth;
	fn[2]   = input_gain;
	
	return;
}

void GSP_Limiter::SetParams(float fn[])
{

	Switch(fn[0]);
	SetSmooth(fn[1]);
	SetGain(fn[2]);

	return;
}
