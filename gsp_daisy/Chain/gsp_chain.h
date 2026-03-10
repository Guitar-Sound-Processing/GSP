#ifndef GSP_CHAIN_H
#define GSP_CHAIN_H

#include <stdint.h>

#define MAX_EFFECT_NUMBER 	19

enum gsp_effects
{
	GSP_CMP = 0,		 		// Compressor
	GSP_OVD = 1,	 			// Overdrive
	GSP_PHR = 2,	 			// Phaser
	GSP_OCT = 3, 				// Octave
	GSP_SFT = 4, 				// Pitch Shifter
	GSP_DTN = 5,				// Detune
	GSP_WAH = 6,				// Wahwah.
	GSP_EQZ = 7,				// Equalizer
	GSP_CHS = 8, 				// Chorus
	GSP_VBT = 9, 				// Vibrato
	GSP_RVB = 10,				// Reverber
	GSP_DFB = 11,				// Feedback delay
	GSP_EFB = 12, 				// Feedback echo
	GSP_DFF = 13, 				// Feedforward delay
	GSP_EFF = 14, 				// Feedforward echo
	GSP_TML = 15, 				// Tremolo
	GSP_VOL = 16, 				// Output volume
	GSP_LIM = 17,				// Soft limiter
	GSP_NGT = 18,				// Noise Gate
	GSP_LAST, 					// None
};

class GSP_SignalChain
{
	public:
		GSP_SignalChain() {}
		~GSP_SignalChain() {}

		void 		New();
		void 		Swap(int32_t effect, int32_t to_pos);
		void 		Clear();
		void 		ToLast(int32_t effect_number);
		void 		FromLast(int32_t to_pos);
		void 		Append(int32_t effect);
		void 		Remove(int32_t effect);
		int32_t 	Locate(int32_t effect);
		void	    Name(int32_t effect, char *printout);
		int32_t 	Number(char* name);
		int32_t		RemoveEffect(char ct[]);
		void 		Printout(char* printout);

		int32_t 	sgn_chain[MAX_EFFECT_NUMBER];
		uint32_t 	number_effects;
	private:

};

#endif 	// GSP_CHAIN_H

