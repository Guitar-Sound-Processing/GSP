#ifndef GSP_POTS_H
#define GSP_POTS_H

#include <stdint.h>

class GSP_Pots
{
	public:
		GSP_Pots() {}
		~GSP_Pots() {}

		int32_t 	AssignPot(int32_t effect, int32_t pot_number);
        int32_t     GetPotID(int32_t effect);
        int32_t     RemovePot(int32_t pot_id);
//        int32_t     PrintPot(int32_t pot_id, char* printout);

		uint32_t 	number_pots;
		int32_t 	effect_id[6];
        int32_t     pot_id[6];
	private:
        int8_t      effect_to_seq_[20]  = {-1, -1,  0, -1, -1, -1,  1, -1,  2,  3, -1, -1, -1, -1, -1,  4,  5, -1, -1, -1};
        int32_t     effect_id_[6]       = {2, 6, 8, 9, 15, 16}; // phr, wah, chs, vbt, tml, vol
        int32_t     link_id_[6]         = {-1, -1, -1, -1, -1, -1};
};

#endif 	// GSP_CHAIN_H

// effect_id_       {2, 6, 8, 9, 15, 16}
// link_id_          1 -1  0  3   1  -1

// effect_id        {2  8  9  15}   number_pots = 4
// pot_id           {1  0  3   1}

// pot_id            0  1  2  3  4  5  6  7
