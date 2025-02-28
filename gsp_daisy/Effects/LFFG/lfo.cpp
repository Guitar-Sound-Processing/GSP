#include <math.h>

#include "lfo.h"

// *****************************************************************************

void LowFreqOsc::Init(uint32_t sampling_rate)
{
	/*
    Initiate the Low Frequency Oscilator Class.
		sampling_rate
			ADC sampling rate (Hz)
	*/

	sample_rate = sampling_rate;
	max_gain 	= 65532;

	SetProfile(LFO_HALFSINE);
	SetPeriod(500);

	SetDutyCycle(50.f);
	phase_  	= 0.;

	return;
}

void LowFreqOsc::SetPeriod(uint32_t prd)
{
	/*
    To set the period of the Low Frequency Oscilator.
		prd
			period of each cycle in milliseconds
			(100 <= prd <= 5000) ms
	*/

	period 		= fmax(fmin(prd, 5000), 100);
	rate_	   	= 512.*1000. / (float)sample_rate / (float)period;

	return;
}

void LowFreqOsc::SetFrequency(float freq)
{
	/*
    To set the frequency of the Low Frequency Oscilator.
		freq
			frequency of LFO in Hz
			(0.2 <= freq <= 10) Hz
	*/

	SetPeriod(fmax(fmin(1000./freq, 5000), 100));

	return;
}

void LowFreqOsc::SetDutyCycle(float duty_cycle)
{
	/*
    To set the duty_cycle of the LFO_SQUARE, LFO_EXPDECREASE and
	LFO_EXPINCREASE profiles
		duty_cycle
			duty cycle in fraction of the Period (percent) (0 to 100)
	*/

	duty 		= duty_cycle;
	duty_ 		= (uint32_t)fmax(fmin(duty_cycle*5.12, 512), 0);

	SetProfile(profile);
	
	return;
}

void LowFreqOsc::SetProfile(uint8_t prof)
{
	/*
	Set the envelope profile for the Low Frequency Oscilator
	The generated wave has time resolution of 512, with frequency adjusted
	by the SetFrequency(.) method, and amplitude of 65532 (16 bits). 
	The selected profile can be
		prof
			LFO_SIN 	 	Sine <0:2*pi>
			LFO_HALFSINE 		Half sine <0:pi> (default)
			LFO_RAMP		Ramp /|/|/|
			LFO_SAW 		Saw tooth |\|\|\
			LFO_TRIANGLE		Triangle /\/\/\
			LFO_SQUARE		Square |-|_|-|_|
			LFO_EXPDECREASE 	Decreasing exponential |\_
			LFO_EXPINCREASE 	Increasing exponential _/|
			LFO_EXTERNAL 		External signal
			LFO_LEVEL 		Power amplitude of input signal
			LFO_REVERSE_LEVEL 	Reverse amplitude of input signal
			LFO_LAST		None
	The LFO_SQUARE, LFO_EXPDECREASE and LFO_EXPINCREASE profiles needs 
	also a duty cycle to adjust the active phase of LFO_SQUARE and the
	decay time of the exponential profiles (SetDutyCycle(.))
	The amplitude of LFO_EXTERNAL is set by SetExternal(.) method. This
	allows the amplitude to be adjusted by a potentiometer acting
	as volume pedal, for example.
	
	The amplitude is function of time, but since time depends on
	the hardware or, sometimes, also on the compiler, the LFO amplitude
	is computed at sample rate frequency. In other words, each time the
	amplitude is computed (Amplitude() method), the time, or
	more precisely, the angle of the LFO, is increased by a fixed amount.
	So Amplitude shall be called at same rate as the sampling input.
	The amplitude is always between 0 and 65532, whatever the profile be.
	*/

	uint32_t i;

	profile 	= prof;
	
	switch (profile)
	{
		case LFO_SIN:    // sin (0-2pi) -> (0:65535)
			for (i = 0; i < 512; i++)
			{
				ampl_[i] = 32767.*sin(2.*GDSP_PI * (float)(i / 512.)) + 32768;
			}
			break;
		case LFO_HALFSINE:    // sin (0-pi) -> (0:65535)
			for (i = 0; i < 512; i++)
			{
				ampl_[i] = 65535.*sin(GDSP_PI * (float)(i / 512.));
			}
			break;
		case LFO_RAMP:  // ramp /|/|/|/|...
			for (i = 0; i < 512; i++)
			{
				ampl_[i] = 128*i;
			}
			break;
		case LFO_SAW:  // saw tooth |\|\|\|\|...
			for (i = 0; i < 512; i++)
			{
				ampl_[i] = 65408 - 128*i;
			}
			break;
		case LFO_TRIANGLE:  // saw /\/\/\/\...
			for (i = 0; i < 512; i++)
			{
				if (i < 256)
				{
					ampl_[i] = 256*i;
				}
				else
				{
					ampl_[i] = 65535 - 256*(i - 256);
				}
			}
			break;
		case LFO_SQUARE:  // step _|-|_|-|_|...
			for (i = 0; i < 512; i++)
			{
				if (i <= duty) ampl_[i] = 65535;
				else ampl_[i] = 0;
			}
			break;
		case LFO_EXPDECREASE:  // exponential decreasing |\_
			for (i = 0; i < 512; i++)
			{
				ampl_[i] = 65535*(1 - exp(-(float)i/2))*exp(-(float)i/duty);
			}
			break;
		case LFO_EXPINCREASE:  // exponential increasing _/|
			for (i = 0; i < 512; i++)
			{
				ampl_[511-i] = 65535*(1 - exp(-(float)i/2))*exp(-(float)i/duty);
			}
			break;
		case LFO_EXTERNAL:  // External source (volume pedal)

			break;
		case LFO_LEVEL:  // Volume amplitude of input signal

			break;
		case LFO_REVERSE_LEVEL: // Reverse amplitude of input signal

			break;
		case LFO_LAST:  // None

			break;
	}

	return;
}

void LowFreqOsc::SetGain(uint32_t gain)
{
	/*
    To set the LFO gain for LFO_EXTERNAL profile
		gain
			Gain for the Low Frequency Oscilator (0 to 65531)
	*/

	gain_		= gain;

	return;
}

uint32_t LowFreqOsc::GetAmplitude()
{
	/*
	Retrieve the maximum amplitude of the LFO gain, which
	is equal to 65532
	*/

	return max_gain;
}

uint32_t LowFreqOsc::GetValue()
{
	/*
	Compute the amplitude of the Low Frequency Oscilator.
		LowFreqOsc
			Amplitude at given time (0 to 65535)
	*/

	if (profile == LFO_EXTERNAL)
	{
		return gain_;
	}
	else
	{
		if (profile == LFO_LEVEL)
			return LevelDetectorPower();
		else
		{
            if (profile == LFO_REVERSE_LEVEL)
            {
                return max_gain - LevelDetectorPower();
            }
            else
            {
    			phase_  += rate_;
	    		if (phase_ > 512) phase_ = 0;
		    	return ampl_[(uint32_t)phase_];
            }
		}
	}
}

