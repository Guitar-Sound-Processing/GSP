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

      $\large a(t)=\frac{1}{2}(\sin2\pi t + 1)$
   
3)	LFO_HALFSINE (1). Half sine wave (default profile): 

      $\large a(t)=\sin\pi t$

4)	LFO_RAMP (2). Increasing linear ramp: 

      $\large a(t)= t$

5)	LFO_SAW (3). Decreasing linear ramp: 

      $\large a(t)=1-t$

6)	LFO_TRIANGLE (4). Triangle wave: 
  	    
```math
\large a(t) = \left\{ \begin{array}{ll} 2t, \; 0 \leq t < 0.5 \\
2-2t, \; 0.5 \leq t < 1 \end{array} \right.
``` 
      
7)	LFO_SQUARE (5). Square wave:

```math
\large a(t) = \left\{ \begin{array}{ll} 0, \; 0 \leq t < D \\
1, \; D \leq t < 1 \end{array} \right.
``` 

8)	LFO_EXPDECREASE (6). Decreasing exponential with continuous attack and decay:

      $\large a(t)=[1-\mathrm{e}^{-t/2}] \mathrm{e}^{(-t/D)}$
 
9)	LFO_EXPINCREASE (7). Increasing exponential with continuous attack and decay:

      $\large a(t)=[1-\mathrm{e}^{t/2-1/2}] \mathrm{e}^{(t/D-1/D)}$

10)	LFO_EXTERNAL (8). External signal from potentiometer (volume pedal).
 
11)	LFO_LEVEL (9). Power amplitude of input signal, coming from the Level Detector (lvd) procedure.

12)	LFO_REVERSE_LEVEL (9). Reversed power amplitude of the input signal, coming from the Level Detector (lvd) procedure.

13)	LFO_LAST (10). No profile.

Besides the profiles, the Frequency $f_{LFO}$ (or the Period $T_{LFO}$) and the Duty Cycle of the LFFG can be individually adjusted for each one of the six above mentioned effects. The Period scales *t* so when *t* = 1 the output *a*(*t*) starts a new cycle. The duty cycle parameter *D* (lfo.duty) is necessary only in LFO_SQUARE, LFO_EXPDECREASE and LFO_EXPINCREASE profiles, in percentage of the Period (0 < *D* < 100). It's worth to mention that the *efc* commands still require the lfo.duty parameter, even when no one of these profiles is selected, for instance:

```phr 1 0.8 100 4 0.5 0 0.9```

in which the Phaser parameters are specified with triangle envelope (4) and useless duty cycle of 0. 

In addition, the Attack and Release times of LFO_LEVEL can be adjusted using the ```lvd``` command.

