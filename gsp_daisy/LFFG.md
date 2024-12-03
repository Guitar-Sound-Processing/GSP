# Low Frequency Function Generator LFFG

Some guitar effects need a LFFG (Low Frequency Function Generator) in order to generate a time varying tone or amplitude envelop. Most of the GSP generated functions are periodic and, therefore, can be said that are, in fact, Low Frequency Oscillators (LFO). Sometimes ‘LFO’ and ‘LFFG’ are used indistinctly, mainly in the LFFG configuration parameters, like in lfo.duty, for instance. The effects that are modulated by LFFG are:

-	Chorus (chr)
-	Phaser (phr)
-	Tremolo (tml)
-	Vibrato (vbt)
-	Volume (vol)
-	Wahwah (wah)

The LFFG can be configured by several parameters and profiles. The profiles change the LFFG amplitude between 0 (lowest) and 1 (maximum) with time dependent math functions. The LFFG period TLFO (or the frequency fLFO) can be adjusted anytime, and the modulation amplitude a(t) can be obtained by calling an appropriate function. The profiles are stored in lfo_wave enumerator and are:

1)	LFO_SIN (0). Sine wave profile:

   
3)	LFO_HALFSINE (1). Half sine wave (default profile): 

 

4)	LFO_RAMP (2). Increasing linear ramp: 

 

5)	LFO_SAW (3). Decreasing linear ramp: 

 

6)	LFO_TRIANGLE (4). Triangle wave: 

 

7)	LFO_SQUARE (5). Square wave:

 

8)	LFO_EXPDECREASE (6). Decreasing exponential with continuous attack and decay:

 

9)	LFO_EXPINCREASE (7). Increasing exponential with continuous attack and decay:

 

10)	LFO_EXTERNAL (8). External signal from potentiometer (volume pedal).
 
11)	LFO_LEVEL (9). Power amplitude of input signal, coming from the Level Detector (lvd) procedure (Section 3.1.2).

12)	LFO_REVERSE_LEVEL (9). Reversed power amplitude of the input signal, coming from the Level Detector (lvd) procedure (Section 3.1.2)

13)	LFO_LAST (10). No profile.

Besides the profiles, the Frequency fLFO (or the Period TLFO) and the Duty Cycle of the LFFG can be individually adjusted for each one of the six above mentioned effects. The duty cycle parameter D (lfo.duty) is necessary only in LFO_SQUARE, LFO_EXPDECREASE and LFO_EXPINCREASE profiles. However, the efc commands still require the lfo.duty parameter, even when no one of these profiles is selected, for instance:

phr 1 0.8 100 4 0.5 0 0.9

in which the Phaser parameters are specified with triangle envelope (4) and unused duty cycle of 0. 


