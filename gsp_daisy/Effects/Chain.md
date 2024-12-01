# Chains

## Definition

A Chain (or a gig) is a sequence of effects such that the output of a given effect is the input of the next one in the Chain. GSP 1.0.0 supports only a configurable single Chain (SISO – single input, single output), with some or all available effects included. A possible chain configuration could be something like:

Input $\rightarrow$ Compressor $\rightarrow$ Overdrive $\rightarrow$ Equalizer $\rightarrow$ Delay $\rightarrow$ Output

The number of effects in the chain and their order can be configured by commands, although only one instance of each effect can be used in the current GSP version. From input to output, the effect position is numbered starting from 0. In the above example, Compressor is placed in 0, Overdrive in 1, Equalizer in 2 and so on.

Like any analog gear, the GSP effects need two conditions to process the input signal:

1)	The effect must be inserted into chain.
2)	The effect must be switched on (see effect switch *s*).

The effect switch acts like a by-pass: if turned on (*s* = 1) the effect is active in chain. Otherwise, when *s* = 0, the effect is disabled and the signal is by-passed through the chain.

Here are some examples of chain command output (see Chain Command section):

```->Inp->CMP->OVD->WAH->EQZ->DFB->NGT->Out->``` <br>
```->Inp->OCT->EQZ->CHS->RVB->LIM->Out->```</br>

## Chain Commands:

Any Chain can be configured by using a small set or commands. Some commands were created to check the chain sequence, to create a new one and to erase the whole chain. They are:

### New Chain

Erase the current chain and start a new one with the complete set of effects. Although the previous chain is erased, all the effect parameters remain with their preceding settings.

```new```

> ->CMP->OVD->PHR->OCT->SFT->DTN->WAH->EQZ->CHS->VBT->RVB->DFB->EFB->DFF->EFF->TML->VOL->LIM->NGT->


### Current Chain

Prints the current chain sequence.

```all```

> ->Inp->CMP->OVD->RVB->Out->

### Clear Chain

Clear the whole chain sequence (no effects left on chain)

```clr```

> ->Inp->Out->



