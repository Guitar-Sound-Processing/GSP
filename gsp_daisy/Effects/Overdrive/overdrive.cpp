#include <math.h>
#include <stdio.h>

#include "overdrive.h"

// *****************************************************************************

void GSP_Overdrive::Init(uint32_t sampling_rate)
{
	/*
    Initiate the Overdrive Class.
		sampling_rate
			ADC sampling rate (Hz)
	*/

	float 		dt, cap, res, a0_us, a0_ub, vmax, dv;
	uint32_t 	i;
	
	sample_rate 	= sampling_rate;
	
	tone.Init(sample_rate);
	tone.SetTone(0.8);

	dt 		= 1./sample_rate;
	cap 	= 100e-9;	// 100 nF
	res 	= 100000.;	// 100 kOhm
  
	conf_ 	= ADC_HALFRES;
	c16b_	= 65535;
	
	a0_us 	= 2*cap*res + dt;
	a1_us_ 	= (2*cap*res - dt)/a0_us;
	b0_us_  = dt/a0_us;
	b1_us_  = dt/a0_us;
	c0_us_  = 2*cap*res/a0_us;
	c1_us_  = -2*cap*res/a0_us;

	a0_ub   = 2*cap*res + dt;
	a1_ub_  = (2*cap*res - dt)/a0_ub;
	b0_ub_  = dt/a0_ub;
	b1_ub_  = dt/a0_ub;
	c0_ub_  = -dt/a0_ub;
	c1_ub_  = -dt/a0_ub;

	vmax    = 0.5;  // Maximum diode voltage (V)
	dv      = vmax/255.;
	adc2_ 	= ADC_HALFRES/2;

	for (i = 0; i < 256; i++)
	{
		diode_[i] = conf_*res*pow(10, 10.*i*dv - 9.);
	}
	
	v2i_    = 255./vmax + 0.5;

	ln10t10_ 	= 10.*log(10.);
	cr10_ 	= (float)conf_*res*1.e-9;

	us1_1_ 	= 0;
	ue1_1_ 	= 0;
	vd1_0_ 	= 0;
	vd1_1_ 	= 0;
	ub1_1_ 	= 0;
	ub1_0_ 	= 0;

	us2_1_ 	= 0;
	ue2_1_ 	= 0;
	vd2_0_ 	= 0;
	vd2_1_ 	= 0;
	ub2_1_ 	= 0;
	ub2_0_ 	= 0;

	amp2_  	= 2;         // 2nd stage amplification
	SetSustain(0.5);
	SetGain(1.);
	SetMixer(1.);
	Switch(GSP_OFF);

	return;
}

void  GSP_Overdrive::SetSustain(float sustn)
{
	/*
	Set the Overdrive sustain
		sustn
			sustain level: 0.1 to 1. 
	*/

	sustain   	= fmaxf(fminf(sustn, 1), 0.);
	amp1_ 		= 3*(0.95*sustain + 0.05); // first stage amplification
	SetGain(gain);

	return;
}

void  GSP_Overdrive::SetGain(float output_gain)
{
	/*
	Set the Overdrive gain
		output_gain
			output gain: 0 to 1. 
	*/

	gain   		= fmaxf(fminf(output_gain, 1), 0);
	amp3_  		= 0.6*gain*(4 - 2.5*sustain);  // output amplification

	return;
}

void GSP_Overdrive::SetMixer(float mix)
{
	/*
    To mix the wet overdrive with  the dry signal.
		mix
			fraction of mixer. A zero mix has no overdrive, although
			the signal is still present on output, while mix equals 
			to 1 has overdrive only. Normally mix is equal to 1.
	*/
	
	mixer		= fmaxf(fminf(mix, 1), 0);
	mix_wet_	= mixer;
	mix_dry_ 	= 1. - mix_wet_;
	
	return;
}
void GSP_Overdrive::Switch(uint8_t mode)
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

int32_t GSP_Overdrive::Process(int32_t sampl)
{
	/*
    To compute the Overdrive filter.
		sampl:
			Input sample
		Overdrive.Process
			Processed output
	*/
 
	int32_t 	eb, ivd, evd, ue1_0, ue2_0, fv, flv;
	int32_t 	sout;
  
	ue1_0 	= amp1_*sampl;
	
	// First stage
	eb        = ue1_0 - ub1_0_;
	if (eb < 0) eb = -eb;

	if (vd1_0_ < 0) vd1_0_  = -vd1_0_;

	if (eb == 0) vd1_0_   = 0;
	else
	{
		ivd     = (vd1_0_*v2i_)/conf_;
		if (ivd > 255) ivd  = 255;
		evd     = diode_[ivd];
		fv      = eb - vd1_0_ - evd + cr10_;
		if (fv > c16b_) fv = c16b_;
		if (fv < -c16b_) fv = -c16b_;
		fv 		= conf_*fv;
		flv     = evd*ln10t10_ + conf_;
		vd1_0_  = vd1_0_ + fv/flv;
	}

	if (ue1_0 < ub1_0_) vd1_0_   = -vd1_0_;

	us1_0_  	= b0_us_*ue1_0 + b1_us_*ue1_1_ + 
		c0_us_*vd1_0_ + c1_us_*vd1_1_ + a1_us_*us1_1_;
	ub1_0_  	= b0_ub_*ue1_0 + b1_ub_*ue1_1_ + 
		c0_ub_*vd1_0_ + c1_ub_*vd1_1_ + a1_ub_*ub1_1_;
	us1_1_ 		= us1_0_;
	vd1_1_ 		= vd1_0_;
	ue1_1_ 		= ue1_0;
	ub1_1_ 		= ub1_0_;

	// Second stage
	ue2_0 		= amp2_*us1_0_;
	eb 			= ue2_0 - ub2_0_;
	if (eb < 0) eb = -eb;

	if (vd2_0_ < 0) vd2_0_ 	= -vd2_0_;

	if (eb == 0) vd2_0_		= 0;
	else
	{
		ivd 	= (vd2_0_*v2i_)/conf_;
		if (ivd > 255) ivd  = 255;
		evd     = diode_[ivd];
		fv      = eb - vd2_0_ - evd + cr10_;
		if (fv > c16b_) fv = c16b_;
		if (fv < -c16b_) fv = -c16b_;
		fv 		= conf_*fv;
		flv     = evd*ln10t10_ + conf_;
		vd2_0_  = vd2_0_ + fv/flv;
	}

	if (ue2_0 < ub2_0_) vd2_0_   = -vd2_0_;

	us2_0_ 		= b0_us_*ue2_0 + b1_us_*ue2_1_ + 
		c0_us_*vd2_0_ + c1_us_*vd2_1_ + a1_us_*us2_1_;
	ub2_0_ 		= b0_ub_*ue2_0 + b1_ub_*ue2_1_ + 
		c0_ub_*vd2_0_ + c1_ub_*vd2_1_ + a1_ub_*ub2_1_;
	us2_1_ 		= us2_0_;
	vd2_1_ 		= vd2_0_;
	ue2_1_ 		= ue2_0;
	ub2_1_ 		= ub2_0_;

	sout    = amp3_*(float)us2_0_;
	
	return (int32_t)(mix_wet_*tone.Process(sout) + mix_dry_*sampl); 
	//return sout; 

}

void GSP_Overdrive::Printout(uint8_t out_list, int32_t chn_pos, char *printout)
{

    if (out_list == 0)
    {
        sprintf(printout, 
        "->OVD (%ld): OFF(0)|ON(1) %d "
        "| Sustain (0.1-1): %-.3f "
        "| Tone (0-1): %-.3f "
		"| Mixer (0-1): %-.3f "
        "| Gain (0-1): %-.3f\n", 
        chn_pos, state, sustain, tone.tone, mixer, gain);
    }
    if (out_list == 1)
    {
        sprintf(printout, 
        "->OVD (%ld) %d %-.3f %-.3f %-.3f %-.3f\n", 
        chn_pos, state, sustain, tone.tone, mixer, gain);
    }
	
	return;
}

void GSP_Overdrive::GetParams(float param[])
{
	
    param[0] 	= (float)state;
    param[1] 	= sustain;
    param[2] 	= tone.tone;
	param[3] 	= mixer;
    param[4] 	= gain;
	
	return;
}

void GSP_Overdrive::SetParams(float param[])
{
	Switch((uint8_t)param[0]);
	SetSustain(param[1]);
	tone.SetTone(param[2]);
	SetMixer(param[3]);
	SetGain(param[4]);

	return;
}
