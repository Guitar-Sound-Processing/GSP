#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "guitar_dsp.h"
#include "daisy_seed.h"
#include "gsp_chain.h"
#include "pots.h"

using namespace daisy;
//DaisySeed   hw;

//#include "Arduino.h"
//#include "DaisyDuino.h"

// *****************************************************************************

int32_t GSP_Pots::AssignPot(int32_t effect, int32_t pot)
{
	/*
    To assign a potentiometer pedal given by its ID number to a LFO External profile
		effect
			effect number (0, 1, for instance) - See enum gsp_effects
		pot_id
			potentiometer identifier (0 to 7)
	Returns 0 if succeeded or -1 if failed
	*/

	uint8_t ipot;
	
	if (pot < 0) return -1;
	if (pot > 7) return -1;
	if (effect < 0) return -1;
	if (effect >= MAX_EFFECT_NUMBER) return -1;
	
	// phr - 2
	// wah - 6
	// chs - 8
	// vbt - 9
	// tml - 15
	// vol - 16

    if (effect_to_seq_[effect] >= 0)
    {
        link_id[effect_to_seq_[effect]]   = pot;
     }
    else  return -1;

    //effect_to_seq_[20]  = {-1, -1,  0, -1, -1, -1,  1, -1,  2,  3, -1, -1, -1, -1, -1,  4,  5, -1, -1, -1};
    //effect_id[6]       = {2, 6, 8, 9, 15, 16}; // phr, wah, chs, vbt, tml, vol
    //link_id[6]         = {-1, -1, -1, -1, -1, -1};

    number_pots     = 0;
    for (ipot = 0; ipot < number_lfo; ipot++)
    {
        if (link_id[ipot] >= 0)
        {
            effect_id[number_pots]  = effect_lfo[ipot];
            pot_id[number_pots]     = link_id[ipot];
            number_pots++;
        }
    }

	return 0;
}

// *****************************************************************************

int32_t GSP_Pots::GetPotID(int32_t effect)
{
	/*
    To get the pot identifier assigned to a given effect number
		effect
			effect number 
	Returns the effect number or -1 if failed
	*/

	if (effect < 0) return -1;
	if (effect > MAX_EFFECT_NUMBER) return -1;
    if (effect_to_seq_[effect] >= 0)
    {
	    return link_id[effect_to_seq_[effect]];	
    }
    else return -1;
}

// *****************************************************************************

int32_t GSP_Pots::RemovePot(int32_t effect)
{
	/*
    Remove (unassign) pot from effect
		effect
			effect number (0 to MAX_EFFECT_NUMBER)
	Returns 0 if succeeded or -1 if failed
	*/
	
    uint8_t ipot;

	if (effect < 0) return -1;
	if (effect > MAX_EFFECT_NUMBER) return -1;
	
    if (effect_to_seq_[effect] >= 0)
    {
	    link_id[effect_to_seq_[effect]] = -1;
    }
    else return -1;

    number_pots     = 0;
    for (ipot = 0; ipot < number_lfo; ipot++)
    {
        if (link_id[ipot] >= 0)
        {
            effect_id[number_pots]  = effect_lfo[ipot];
            pot_id[number_pots]     = link_id[ipot];
            number_pots++;
        }
    }

    return 0;
}

// *****************************************************************************
/*
int32_t GSP_Pots::PrintPot(int32_t effect, char* printout)
{

    char  pname[28];
    //char* pchar;
    
	if (effect < 0) return -1;
	if (effect > MAX_EFFECT_NUMBER) return -1;
    
    if (effect_to_seq_[effect] >= 0)
    {
        sprintf(printout, "->POT: Effect: ");
        Name(pot2effect[pot_id], pname);

    }
    else return -1;


    strcat(printout, pname);
    sprintf(pname, " | Potentiometer ID: ");
 	strcat(printout, pname);
	sprintf(pname, "%ld\r\n", pot_id);
 	strcat(printout, pname);

    return 0;
}
*/
