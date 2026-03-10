#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "guitar_dsp.h"
#include "daisy_seed.h"

using namespace daisy;
//DaisySeed   hw;

//#include "Arduino.h"
//#include "DaisyDuino.h"

// *****************************************************************************

void GSP_SignalChain::New()
{
	/*
    To initiate the signal chain
	*/

	uint32_t i;
	for (i = 0; i < MAX_EFFECT_NUMBER; i++)
	{
		sgn_chain[i] = i;
	}
	number_effects 	= MAX_EFFECT_NUMBER;
	
	return;
}

// *****************************************************************************

void GSP_SignalChain::Swap(int32_t effect, int32_t to_pos)
{
	/*
    Change signal chain position
		effect
			effect number (enumerator, like GSP_CHS or GSP_DTN)
		to_pos
			new effect position in signal chain
	
	If "effect" number is greater than the maximum available number of effects or
	it is less than 0 no action will be made on chain.
	If "effect" isn't in chain, it will be added. If to_pos is less than 0 the
	effect will be added to the input (0 pos). If to_pos is greater than the 
	number of effects in chain, it will be added at the output (append)
	*/

	int32_t 	from_pos;

	if (effect < 0) return;
	if (effect >= MAX_EFFECT_NUMBER) return;

	from_pos 	= Locate(effect);

	if (from_pos < 0)
	{
		// this effect isn't in chain
		Append(effect);
	}
	else
	{
		// the effect is already in chain
		ToLast(from_pos);
	}

	if (to_pos < 0)	to_pos = 0;
	if (to_pos < (int32_t)number_effects - 1)
	{
		FromLast(to_pos);
	} 

	return;
}

// *****************************************************************************

void GSP_SignalChain::Clear()
{
	/*
    To clear the effect chain (no effects on chain)
	*/

	number_effects 	= 0;
	
	return;
}

// *****************************************************************************

void GSP_SignalChain::ToLast(int32_t from_pos)
{
	/*
    To append or to move an effect to the end (output) of signal chain
		from_pos
			position in chain to be moved to the last slot (output). from_pos must
			be set between the most left position (input = 0) to the maximum number
			of effects on current chain (number_effects), otherwise no changes will
			be performed on chain.
	*/

	int32_t chain_st, i;
	
	if (from_pos < 0) return;
	if (from_pos >= (int32_t)number_effects-1) return;

	chain_st 	= sgn_chain[from_pos];
	for (i = from_pos+1; i < (int32_t)number_effects; i++)
	{
		sgn_chain[i-1] = sgn_chain[i];
	}
	sgn_chain[number_effects-1]		= chain_st;

	return;
}

// *****************************************************************************

void GSP_SignalChain::FromLast(int32_t to_pos)
{
	/*
    To move the last effect on chain (output) to a given position. 
		to_pos
			effect position in chain. If to_pos is less than zero or greater than
			the number of effects in chain than no change will be done on chain.
	*/

	int32_t chain_st, i;
	
	if (to_pos < 0) return;
	if (to_pos >= (int32_t)number_effects - 1) return;
	
	chain_st 	= sgn_chain[number_effects-1];
	for (i = number_effects-1; i > to_pos; i--)
	{
		sgn_chain[i] = sgn_chain[i-1];
	}
	sgn_chain[to_pos] 	= chain_st;

	return;
}

// *****************************************************************************

void GSP_SignalChain::Append(int32_t effect)
{
	/*
    To append an effect at the end of signal chain
		effect
			effect number (enumerator, like GSP_CHS or GSP_DTN)
	*/

	int32_t effect_pos;
	
	if (effect < 0) return;
	if (effect >= MAX_EFFECT_NUMBER) return;

	effect_pos 	= Locate(effect);
	
	if (effect_pos < 0)
	{
		// this effect isn't in chain
		sgn_chain[number_effects] 	= effect;	// append this effect in chain
		number_effects++; 	// increase the number of effects in chain
	}
	else
	{
		// this effect is already in chain, so it needs to move
		ToLast(effect_pos);
	}

	return;
}

// *****************************************************************************

void GSP_SignalChain::Remove(int32_t effect)
{
	/*
    To remove an effect off of signal chain
		effect
			effect number (enumerator, like GSP_CHS or GSP_DTN)
	*/

	int32_t effect_pos;
	
	if (effect < 0) return;
	if (effect >= MAX_EFFECT_NUMBER) return;

	effect_pos 	= Locate(effect);
	
	if (effect_pos > 0)	
	{						// the effect is in chain
		ToLast(effect_pos);
		number_effects--;
	}

	return;
}

// *****************************************************************************

int32_t GSP_SignalChain::Locate(int32_t effect)
{
	/*
    Locate the position in chain of an effect given by its enumerator number
		effect
			effect number (0, 1, for instance) - See enum gsp_effects
		Locate
			position of the effect (= -1 if there is no effect
			with the provided name)
	*/

	int32_t i;
	
	for (i = 0; i < (int32_t)number_effects; i++)
	{
		if(effect == sgn_chain[i]) return i;
	}
	return -1;
}

// *****************************************************************************

void GSP_SignalChain::Name(int32_t effect, char *printout)
{
	/*
    Return a pointer to the effect name given its enumerator
		effect
			effect name ("GSP_TML", "GSP_OVD", for instance)
        printout
            pointing to a char array with the effect name 
            as "OVD", "TML", etc.
	*/

	//const char* name;
	
    //name 	= "NUL";
	sprintf(printout, "NUL");
	//if (effect == GSP_CMP) name = "CMP";
    if (effect == GSP_CMP) sprintf(printout, "CMP");
	if (effect == GSP_OVD) sprintf(printout, "OVD");
	if (effect == GSP_PHR) sprintf(printout, "PHR");
	if (effect == GSP_OCT) sprintf(printout, "OCT");
	if (effect == GSP_SFT) sprintf(printout, "SFT");
	if (effect == GSP_DTN) sprintf(printout, "DTN");
	if (effect == GSP_WAH) sprintf(printout, "WAH");
	if (effect == GSP_EQZ) sprintf(printout, "EQZ");
	if (effect == GSP_CHS) sprintf(printout, "CHS");
	if (effect == GSP_VBT) sprintf(printout, "VBT");
	if (effect == GSP_RVB) sprintf(printout, "RVB");
	if (effect == GSP_DFB) sprintf(printout, "DFB");
	if (effect == GSP_EFB) sprintf(printout, "EFB");
	if (effect == GSP_DFF) sprintf(printout, "DFF");
	if (effect == GSP_EFF) sprintf(printout, "EFF");
	if (effect == GSP_TML) sprintf(printout, "TML");
	if (effect == GSP_VOL) sprintf(printout, "VOL");
	if (effect == GSP_LIM) sprintf(printout, "LIM");
	if (effect == GSP_NGT) sprintf(printout, "NGT");

 	return;
}

// *****************************************************************************

int32_t GSP_SignalChain::Number(char* name)
{
	/*
    Return the enumerator number of a given effect name
		name
			Pointer to a 4 character array of the effect name
			Ex: "tml", "ovd", ... The name should be left justified
			and can have any lenght, but only the first three
			characters will be used. It also can be mixed with
			capital letters: 'Phr', 'DTN', 'cHS', etc.
	*/
	
	char st[4];
	st[0] 	= tolower(name[0]);
	st[1] 	= tolower(name[1]);
	st[2] 	= tolower(name[2]);
	st[3] 	= 0;
	
	if (strcmp(st, "cmp") == 0) return GSP_CMP;
	if (strcmp(st, "ovd") == 0) return GSP_OVD;
	if (strcmp(st, "phr") == 0) return GSP_PHR;
	if (strcmp(st, "oct") == 0) return GSP_OCT;
	if (strcmp(st, "sft") == 0) return GSP_SFT;
	if (strcmp(st, "dtn") == 0) return GSP_DTN;
	if (strcmp(st, "wah") == 0) return GSP_WAH;
	if (strcmp(st, "eqz") == 0) return GSP_EQZ;
	if (strcmp(st, "chs") == 0) return GSP_CHS;
	if (strcmp(st, "vbt") == 0) return GSP_VBT;
	if (strcmp(st, "rvb") == 0) return GSP_RVB;
	if (strcmp(st, "dfb") == 0) return GSP_DFB;
	if (strcmp(st, "efb") == 0) return GSP_EFB;
	if (strcmp(st, "dff") == 0) return GSP_DFF;
	if (strcmp(st, "eff") == 0) return GSP_EFF;
	if (strcmp(st, "tml") == 0) return GSP_TML;
	if (strcmp(st, "vol") == 0) return GSP_VOL;
	if (strcmp(st, "lim") == 0) return GSP_LIM;
	if (strcmp(st, "ngt") == 0) return GSP_NGT;

	return -1;
}

// *****************************************************************************

int32_t GSP_SignalChain::RemoveEffect(char ct[])
{
    uint32_t  efnumber;
	char 	*st;
 
    st  = strtok(ct, " ,;");  // effect token
    if (st != NULL)
    {
        efnumber  = Number(st);
        if (efnumber > 0)
        {
            Remove(efnumber);
        }
		else return -1;
    }
	else return -1;
    return 0;
}

// *****************************************************************************

void GSP_SignalChain::Printout(char* printout)
{
    uint32_t i;
    char  pname[4];
    //char* pchar;
    
    sprintf(printout, "->Inp->");
 
    for (i = 0; i < number_effects; i++)
    {
        Name(sgn_chain[i], pname);
        strcat(printout, pname);
        strcat(printout, "->");
    }
    strcat(printout, "Out->\n");
    //*pchar 	= 0; 		//sprintf(pchar, "%c", 0);
    strcat(printout, "\0"); 	//strcat(printout, pchar);
}


