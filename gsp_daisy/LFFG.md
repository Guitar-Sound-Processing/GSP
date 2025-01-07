# Low Frequency Function Generator LFFG

Some guitar effects need a LFFG (Low Frequency Function Generator) in order to generate a time varying tone or amplitude envelop. Most of the GSP generated functions are periodic and, therefore, can be said that are, in fact, Low Frequency Oscillators (LFO). Sometimes ‘LFO’ and ‘LFFG’ are used indistinctly, mainly in the LFFG configuration parameters, like in lfo.duty, for instance. The effects that are modulated by LFFG are:

-	[Chorus](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/Effects.md#efcchs) (chr)
-	[Phaser](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/Effects.md#efcphr) (phr)
-	[Tremolo](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/Effects.md#efctml) (tml)
-	[Vibrato](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/Effects.md#efcvbt) (vbt)
-	[Volume](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/Effects.md#efcvol) (vol)
-	[Wahwah](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/Effects.md#efcwah) (wah)

The LFFG can be configured by several parameters and profiles. The profiles change the LFFG shape that modulates the signal amplitude between 0 (lowest) and 65535 (maximum) with time dependent math functions. The LFFG period TLFO (or the frequency fLFO) can be adjusted anytime, and the modulation amplitude a(t) can be retrieved by the public variables:

- ```lfo.profile``` - function profile (1 to 12)
- ```lfo.period``` - modulation period (miliseconds)
- ```lfo.duty``` - duty cycle (profiles 7, 8 and 9)

More can be found in [LFFG specific documentation](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/Effects/LFFG/LFFG.pdf). The profiles are stored in ```lfo_wave``` enumerator and are:

1)	```LFO_SIN```. Sine wave profile:

      $\large a(t)=\frac{1}{2}(\sin2\pi t + 1)$
   
3)	```LFO_HALFSINE```. Half sine wave (default profile): 

      $\large a(t)=\sin\pi t$

4)	```LFO_RAMP```. Increasing linear ramp: 

      $\large a(t)= t$

5)	```LFO_SAW```. Decreasing linear ramp: 

      $\large a(t)=1-t$

6)	```LFO_TRIANGLE```. Triangle wave: 
  	    
```math
\large a(t) = \left\{ \begin{array}{ll} 2t, \; 0 \leq t < 0.5 \\
2-2t, \; 0.5 \leq t < 1 \end{array} \right.
``` 
      
7)	```LFO_SQUARE```. Square wave:

```math
\large a(t) = \left\{ \begin{array}{ll} 0, \; 0 \leq t < D \\
1, \; D \leq t < 1 \end{array} \right.
``` 

8)	```LFO_EXPDECREASE```. Decreasing exponential with continuous attack and decay:

      $\large a(t)=[1-\mathrm{e}^{-t/2}] \mathrm{e}^{(-t/D)}$
 
9)	```LFO_EXPINCREASE```. Increasing exponential with continuous attack and decay:

      $\large a(t)=[1-\mathrm{e}^{t/2-1/2}] \mathrm{e}^{(t/D-1/D)}$

10)	```LFO_EXTERNAL```. External signal from potentiometer (volume pedal).
 
11)	```LFO_LEVEL```. Power amplitude of input signal, coming from the Level Detector (lvd) procedure.

12)	```LFO_REVERSE_LEVEL```. Reversed power amplitude of the input signal, coming from the Level Detector (lvd) procedure.

13)	```LFO_LAST```. No profile.

In the above expressions, $\large 0 \leq t \leq 1$ is the time for function generation and $\large D$ is the duty cycle.

Besides the profiles, the Frequency $\large f_{LFO}$ (or the Period $\large T_{LFO}$) and the Duty Cycle of the LFFG can be individually adjusted for each one of the six above mentioned effects. The Period scales $\large t$ so when $\large t = 1$ the output $\large a(t)$ starts a new cycle. The duty cycle parameter $\large D$ (```lfo.duty```) is necessary only in ```LFO_SQUARE```, ```LFO_EXPDECREASE``` and ```LFO_EXPINCREASE``` profiles, in percentage of the Period ($\large 0 \leq D \leq 100$). It's worth to mention that the *efc* commands still require the ```lfo.duty``` parameter, even when no one of these profiles is selected, for instance:

```phr 1 0.8 100 4 0.5 0 0.9```

in which the Phaser parameters are specified with triangle envelope (4) and useless duty cycle of 0. 

In addition, the Attack and Release times of ```LFO_LEVEL``` can be adjusted using the [Level Detector](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/Effects.md#efclvd) ```lvd``` command.

