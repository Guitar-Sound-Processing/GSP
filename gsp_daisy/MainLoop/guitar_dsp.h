#ifndef GUITAR_DSP_H
#define GUITAR_DSP_H

#include <stdint.h>

#define GSP_ON 		1
#define GSP_OFF 	0

#define GDSP_PI 	3.14159265358979323846f /* pi */
#define GDSP_2PI 	6.283185307179586476925f /* 2 pi */

#define ADC_RES 		65536				/* ADC 16 bits */
#define ADC_HALFRES 	32768				/* ADC 16 bits/2 */
#define ADC_MAXVAL 		32767				/* ADC maximum value */
#define ADC_MINVAL 		-32768 				/* ADC minimum value */
#define ADC_INVHRESF 	0.000030517578125f 	/* 1/ADC_16_HALFRES */

//**#include  "DaisyDuino.h"

#include "chorus.h"
#include "compressor.h"
#include "delay_fb.h"
#include "delay_ff.h"
#include "detune.h"
#include "equalizer_3b.h"
#include "level_detector.h"
#include "lfo.h"
#include "noise_gate.h"
#include "octave.h"
#include "overdrive.h"
#include "phaser.h"
#include "pitch_shifter.h"
#include "reverber.h"
#include "tremolo.h"
#include "limiter.h"
#include "wahwah.h"

#include "gsp_chain.h"
#include "pots.h"

#endif 	// GUITAR_DSP_H

