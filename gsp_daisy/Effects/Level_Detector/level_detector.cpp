#include <math.h>
#include <stdio.h>

#include "level_detector.h"

int32_t 	conv_ 	= 65536;
float   	y1_ 	= 0;
float		yL_		= 0;
float 		ytm_ 	= 1;
int32_t 	atk_samples_	= 48;
int32_t 	rel_samples_ 	= 48000;
float 		atk_ms_ 	= 0.001;
float 		rel_ms_ 	= 1.0;
float   	alfa_atk_ 	= 0.979382181331240;	// atk = 0.001 s
float 		alfa_rel_ 	= 0.999979166883679;	// rel = 1 s
int32_t 	lgti_ 		= 0;

// *****************************************************************************

void LevelDetectorSetTimes(uint32_t sample_rate, float atk_ms, float rel_ms)
{
	/*
    Initiate the LevelDetector at given parameters
		sample_rate
			ADC sampling rate (samples per second)
		atk_tm
			attack time in milliseconds 
			atk_smp = atk_ms*sample_rate/1000 where atk_smp
			is the attack time in samples
		rel_tm
			release time in milliseconds 
			rel_smp = rel_ms*sample_rate/1000 where rel_smp
			is the release time in samples
	*/

	atk_ms_ 		= fmaxf(atk_ms, 10./(float)sample_rate);
	atk_samples_ 	= atk_ms_*sample_rate/1000.;
	
	rel_ms_ 		= fmaxf(rel_ms, 10./(float)sample_rate);
	rel_samples_ 	= rel_ms_*sample_rate/1000.;

	alfa_atk_ 	= expf(-1./(float)atk_samples_);
	alfa_rel_ 	= expf(-1./(float)rel_samples_);
	
	y1_			= 0;
	yL_ 		= 0;

	ytm_ 		= (float)conv_/ADC_HALFRES;
	
	return;
}

void LevelDetectorSetSamples(uint32_t sample_rate, int32_t atk_smp, int32_t rel_smp)
{
	/*
    Initiate the LevelDetector at given parameters
		sample_rate
			ADC sampling rate (samples per second)
		atk_smp
			attack time in number of samples 
			atk_smp = atk_ms*sample_rate/1000 where atk_ms
			is the attack time in milliseconds
		rel_smp
			release time in number of samples 
			rel_smp = rel_ms*sample_rate/1000 where rel_ms
			is the release time in milliseconds
	*/

	float atkms, relms;
	
	atkms 		= (float)atk_smp/sample_rate*1000.;
	relms 		= (float)rel_smp/sample_rate*1000.;
	LevelDetectorSetTimes(sample_rate, atkms, relms);
	
	return;
}

float LevelDetectorGetAttackMS()
{
	/*
    Retrieve the attack time in milliseconds from
	LevelDetector
		LevelDetectorGetAttack
			attack time in number of samples 
			atk_smp = atk_ms*sample_rate/1000 where atk_ms
			is the attack time in milliseconds
	*/
	
	return atk_ms_;
}

float LevelDetectorGetReleaseMS()
{
	/*
    Retrieve the release time in milliseconds from
	Level Detector
		LevelDetectorGetRelease
			release time in number of samples 
			rel_smp = rel_ms*sample_rate/1000 where rel_ms
			is the release time in milliseconds
	*/
	
	return rel_ms_;
}

int32_t LevelDetectorProcess(int32_t sampl)
{
	/*
    To compute the LevelDetector. The power level can
	be retrieved by LevelDetectorPower(). Power level range
	from 0 to 65535.
		sampl:
			Input sample
	*/
 
	static float 	xL, yt;

    if (sampl >= 0) xL 		= sampl;
    else xL 	= -sampl;
 
    yt 		= alfa_rel_*(y1_ - xL) + xL; // release level detector

    if (xL > yt) y1_ 		= xL;
    else y1_ 	= yt;

    yL_ 	= alfa_atk_*(yL_ - y1_) + y1_; // attack level detector

	
    lgti_ 	= yL_*conv_;
    
    if (lgti_ < 0) lgti_ 		= 0;
    if (lgti_ >= conv_) lgti_ 	= conv_ - 1;
    
    return lgti_;
}

int32_t LevelDetectorPower()
{
	/*
    To retrieve the power level computed from the stream
	input. The power level is scaled from 0 to 65535
		LevelDetector Power
			level power ranging from 0 to 65535
	*/
 
    return lgti_;
}

void LevelDetectorPrintout(uint8_t out_list, char *printout)
{

    if (out_list == 0)
    {
        sprintf(printout, "->LVD: Attack (ms): %8.3f "
        "| Release (ms): %8.3f\n", 
        LevelDetectorGetAttackMS(),
        LevelDetectorGetReleaseMS());
    }
    if (out_list == 1)
    {
        sprintf(printout, "->LVD %8.3f %8.3f\n", 
        LevelDetectorGetAttackMS(),
        LevelDetectorGetReleaseMS());
    }
	
	return;
}

void LevelDetectorParameters(float fn[])
{
	
	fn[0]   = LevelDetectorGetAttackMS();
	fn[1]   = LevelDetectorGetReleaseMS();
	
	return;
}

// void LevelDetectorSetParams switch to 
//	LevelDetectorSetTimes(sample_rate, fn[0], fn[1]);


