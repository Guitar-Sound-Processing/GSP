#pragma once
#ifndef GSP_POWERLEVEL
#define GSP_POWERLEVEL

#include "../guitar_dsp.h"

void 	LevelDetectorSetTimes(uint32_t sample_rate, float atk_ms, float rls_ms);
void 	LevelDetectorSetSamples(uint32_t sample_rate, int32_t atk_smp, int32_t rls_smp);
float 	LevelDetectorGetAttackMS();
float 	LevelDetectorGetReleaseMS();
int32_t	LevelDetectorProcess(int32_t sampl);
int32_t LevelDetectorPower();
void 	LevelDetectorPrintout(uint8_t out_list, char *printout);
void 	LevelDetectorParameters(float fn[]);

#endif 	// GSP_POWERLEVEL 	Power level detector
