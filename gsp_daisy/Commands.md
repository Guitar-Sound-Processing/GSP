# Commands

The GSP is programmable through ASCII commands from available serial lines. Presently GSP monitors the USB (serial) and UART_1 serial lines. Input commands from serial lines are interpreted and executed by internal GSP procedures. The command string shall terminate with standard ASCII control characters Carriage Return (CR - D13) or Line Feed (LF - D10). After receiving a command, the interpreter answers with knowledge prompt. For instance, the input command:

> ```OVD```

is answered with:

> ```->OVD (1): OFF(0)|ON(1) 0 | Sustain (0.1-1): 0.500 | Tone (0-1): 0.800 | Gain (0-1): 1.000```

On the description that follows, the following rules apply:
 
- {var} indicates a numeric value in ASCII
-	\<var\> indicates a numeric value sent by serial line in binary format
-	/var\ indicates that the string stored in the variable var is sent in ASCII code.
-	*efc* indicates a command or effect name like ovd, phr, wah, pot, and others.

The commands to GSP are separated in Effect Commands, Chain Commands, Interface Commands and Expression Pedal linkage. Additionally, the LFFG section will explain the Low Frequency Function Generator which is needed by some effects. All these effects are processed and interpreted by the Daisy Seed board. There are also some available commands designed to select a whole chain configuration stored on SSD. These and other commands are interpreted by an external processor, like ESP32, and shall be explained later.

## Effect Commands

Any Effect Command must obey the following format:

*efc* [([+][-]*c*)] […[[[[*s*] *p*<sub>1</sub>] *p*<sub>2</sub>] *p*<sub>3</sub>]…]

in which:

-	*efc* stands for the three-characters effect name, like ```ovd``` (for Overdrive), ```tml``` (for Tremolo) and ```phr``` (for phaser).
-	*p*<sub>1</sub>, *p*<sub>2</sub>, … *p*<sub>*n*</sub> are numeric parameters
-	[.] – A variable under brackets means it is optional. The brackets must be omitted on commands
-	(*c*) – A number under parenthesis means a desired chain position of the effect, on command, or the current effect position in chain, on GSP reply. *c* allows changing the effect position in chain (from 0 up to number_effects) and must follow the effect name after a separation (blank) character. The chain position parameter *c* is optional, but if present the parenthesis are necessary to differentiate from the remaining parameters.
-	*s* – The *s* parameter is the effect switch. It can be 0 (OFF, or by-pass) or 1 (ON)
-	*s*, *p*<sub>1</sub>, *p*<sub>2</sub>, *p*<sub>3</sub> … are optional, but if *p*<sub>3</sub> has to be changed, then *s*, *p*<sub>1</sub> and *p*<sub>2</sub> must also be defined in the command.
-	Each parameter *p*<sub>*i*</sub> can be separated with blank, “,” or “;”. Example: *efc* (*c*), *s* *p*<sub>1</sub>, *p*<sub>2</sub>; *p*<sub>3</sub>. Additional blank characters are ignored.

Moreover, the following rules apply

1. For any effect already in chain, the (*c*) parameter in Effect Command swaps the effect efc with the one in c position. 
2. If the effect isn’t already in chain, the (*c*) parameter inserts the effect on the slot pointed by *c*, and shifts the remaining effects to right. 
3. If *c* is greater than the position of the last effect in chain, the effect will be appended at the chain output. 
4. If *c* is negative then the absolute value is disregarded and the effect will be removed from chain. The remaining effects in chain are shifted to left to fulfill the blank position.

Whenever the GSP_CC detects a non valid or a non expected character in command line it discards the command and answers with “?” in console. For example:

> ```ovf 1 0.5```<br>
> ```->?```</br>

since ```“ovf”``` isn’t a valid command.

## Parameter range

Each parameter *p*<sub>*n*</sub> of any effect has its own maximum and minimum allowable values. Sending a command with no parameters, like ```ovd```, produces a printout of the current effect parameters as well as the maximum and minimum allowable values. Any parameter above the maximum or below the minimum allowable values will be internally clipped respectively to maximum or minimum. 

## Effect configuration

Any effect can be configured by a three-character command together with effect parameters. The configuration commands are explained below. Deep explanation on the effect parameters can be found in specific algorithm documentation.

### Level Detector:

The Level Detector is not an “effect” but it is necessary for other effects. It is always active. Therefore it doesn’t have the switch *s*. The Level Detector measures the level of the input audio signal to drive the LFFG when configured in LFO_POWER, and, therefore, ```lvd``` doesn’t change the audio signal. However, the attack and release parameters can be user configured. See LFFG specific documentation for more information.

	lvd atk_tm rel_tm
		rel_tm 		– Attack time (milliseconds)
		rel_tm		– Release time (milliseconds)
 
> ->LVD: Attack (ms):    1.000 | Release (ms): 1000.000

### Chorus:

Duplicates the signal with changes in pitch drove by a LFO

	chs [([+][]c)] s depth delay_ms mixer lfo.profile lfo.freq lfo.duty gain
		depth 		– Depth
		delay_ms 	– Delay (milliseconds)
		mixer 		– Mixer
		lfo.profile 	– LFFG Profile
		lfo.freq 	– LFFG Frequency (Hz)
		lfo.duty	– LFFG Duty cycle
		gain 		– Gain

> ->CHS (8): OFF(0)|ON(1) 0 | Depth (0.1-100)(ms): 5.0 | Delay (0-1000)(ms): 1.0 | Mixer: 0.500 | Profile: (0-10) 0 | Frequency (0.2-5)(Hz): 0.500 | Duty Cycle (0-100)(): 50.0 | Gain (0-1): 1.000

### Compressor:

Reduces the gain for loud signals

 	cmp [([+][]c)] s attack_ms release_ms gain_db threshold_db
		attack_ms 	– Attack time (milliseconds)
		release_ms 	– Release time (milliseconds)
		gain_db		– Gain in dB
		threshold_db 	– Threshold in dB

> ->CMP (0): OFF(0)|ON(1) 0 | Attack (20-2000)(ms): 10.0 | Release (20-2000)(ms): 1000.0 | Gain (0-80)(dB): 20 | Threshold (0-80)(dB): 40

### Delay Feedback:

Mix the signal with a several delayed and attenuated copies

 	dfb [([+][]c)] s delay_ms decay_rate gain
		delay_ms 	– Delay (milliseconds)
		decay_rate 	– Decay rate
		gain 		– Gain

> ->DFB (11): OFF(0)|ON(1) 0 | Delay Time (0.2-100)(ms): 31.0 | Decay rate (0-0.95): 0.700 | Gain (0-1): 1.000

### Delay Feedforward:

Mixes the signal with a given number of copies (maximum 8)

	dff [([+][]c)] s delay_ms decay_rate repeats gain
		delay_ms 	– Delay (milliseconds)
		decay_rate 	– Decay rate
		repeats 	– Number of repeats
		gain 		– Gain

### Detune:

Reduces the frequency of the input signal down to half maximum (inverse of Pitch Shifter)

	dtn [([+][]c)] s detune mixer gain
		detune 		– Pitch (detune)
		mixer 		– Mixer
		gain 		– Gain

> ->DTN (5): OFF(0)|ON(1) 0 | Detune (down) (0-12): 5.000 | Mixer (0-1): 0.500 | Gain (0-1): 1.000

### Echo Feedback:

Same as Delay Feedback but with large time delays

	efb [([+][]c)] s delay_ms decay_rate gain
		delay_ms 	– Delay (milliseconds)
		decay_rate 	– Decay rate
		gain 		– Gain

> ->EFB (12): OFF(0)|ON(1) 0 | Delay Time (50-)(ms): 1000.0 | Decay rate (0-0.95): 0.700 | Gain (0-1): 1.000

### Echo Feedforward:

Same as Delay Feedforward but with large time delays

	eff [([+][]c)] s delay_ms decay_rate repeats gain
		delay_ms 	– Delay (milliseconds)
		decay_rate 	– Decay rate
		repeats 	– Number of repeats
		gain 		– Gain

> ->EFF (14): OFF(0)|ON(1) 0 | Delay Time (50-)(ms): 1000.0 | Decay rate (0-1): 0.900 | Number of repeats (1-8): 4 | Gain (0-1): 1.000

### Equalizer:

Three band equalizer with frequency adjustment

	eqz [([+][]c)] s g_low g_mean g_high f_low f_high
		g_low 		– Low frequency gain
		g_mean 		– Medium frequency gain
		g_high 		– High frequency gain
		f_low 		– Low frequency cutoff
		f_high 		– High frequency cutoff

> ->EQZ (7): OFF(0)|ON(1) 0 | Gains (0-1): Low 1.000 , Medium  1.000 , High 1.000 | Cutoff frequencies (Hz): Low (200-) 200.000 , High (-2000) 800.0

### Limiter: 

Soft level clipping, to avoid signal distortion

	lim [([+][]c)] s smooth input_gain
		smooth 		– Smoothness
		input_gain 	– Input gain

> ->LIM (17): OFF(0)|ON(1) 0 | Smooth factor (0-1): 1.000 | Gain (0- ): 1.000

### Noise Gate:

Mutes the output for low level signals

	ngt atk_tm rel_tm gain threshold
		rel_tm 		– Attack time (milliseconds)
		rel_tm		– Release time (milliseconds)
		gain		– Output gain
		threshold	– Output threshold

. ->NGT (18): OFF(0)|ON(1) 0 | Attack (20-2000)(ms): 10.0 | Release (20-2000)(ms): 1000.0 | Gain (0.1-1): 1.000 | Threshold (0-1): 0.100

### Octave:

Double the signal frequency and mixes with the input

	oct [([+][]c)] s mixer gain
		mixer 		– Mixer
		gain 		– Gain

> ->OCT (3): OFF(0)|ON(1) 0 | Mixer (0-1): 0.500 | Gain (0-1): 1.000

### Overdrive:

Applies a hard clipping on the signal (distortion)

	ovd [([+][]c)] s sustain tone gain
		sustain 	– Sustain
		tone 		– Tone
		gain 		– Output gain

> ->OVD (1): OFF(0)|ON(1) 0 | Sustain (0.1-1): 0.500 | Tone (0-1): 0.800 | Gain (0-1): 1.000

### Phaser:

Mixes the signal with an out of phase copy drove by a LFO

	phr [([+][]c)] s depth level lfo.profile lfo.freq lfo.duty gain
		depth 		– Depth
		level 		– Level
		lfo.profile 	– LFFG Profile
		lfo.freq	– LFFG Frequency (Hz)
		lfo.duty	– LFFG Duty cycle
		gain 		– Gain
> ->PHR (2): OFF(0)|ON(1) 0 | Depth (0-1): 0.500 | Level (0-1000): 10.0 | Profile: (0-10) 0 | Frequency (0.2-5)(Hz): 0.250 | Duty Cycle (0-100)(): 50.0 | Gain (0-1): 1.000

### Pitch Shifter:

Increases the frequency of the input signal continously up to two (Inverse of Detune)

	sft [([+][]c)] s pshift mixer gain
		pshift 		– Pitch (shift)
		mixer 		– Mixer
		gain 		– Gain

> ->SFT (4): OFF(0)|ON(1) 0 | Shift (up) (0-12): 5.000 | Mixer (0-1): 0.500 | Gain (0-1): 1.000

### Reverber:

Mimics the reverberation of a large room

	rvb [([+][]c)] s rvbtime_time gain
		rvbtime_time 	– Reverber time
		gain 		– Gain

> ->RVB (10): OFF(0)|ON(1) 0 | Reverber Time (0-20000)(ms): 1000.0 | Gain (0-1): 1.000

### Tremolo:

Changes the input amplitude with a LFO

	tml [([+][]c)] s lfo.profile lfo.freq lfo.duty gain
		lfo.profile 	– LFFG Profile
		lfo.freq 	– LFFG Frequency (Hz)
		lfo.duty	– LFFG Duty cycle
		gain 		– Gain

> ->TML (15): OFF(0)|ON(1) 0 | Profile: (0-10) 1 | Frequency (0.2-5)(Hz): 2.000 | Duty Cycle (0-100)(): 50.0 | Gain (0-1): 1.000

### Vibrato:

Changes the input frequency with a LFO

	vbt [([+][]c)] s depth delay_ms lfo.profile lfo.freq lfo.duty gain
		depth 		– Depth
		delay_ms 	– Delay (milliseconds)
		lfo.profile 	– LFFG Profile
		lfo.freq 	– LFFG Frequency (Hz)
		lfo.duty	– LFFG Duty cycle
		gain 		– Gain

> ->VBT (9): OFF(0)|ON(1) 0 | Depth (0.1-100)(ms): 5.0 | Delay (0-1000)(ms): 1.0 | Profile: (0-10) 0 | Frequency (0.2-5)(Hz): 0.500 | Duty Cycle (0-100)(): 50.0 | Gain (0-1): 1.000

### Volume:

Changes the input level based on a LFO with external (potentiometer) signal

	vol [([+][]c)] s lfo.profile lfo.freq lfo.duty gain
		lfo.profile 	– LFFG Profile
		lfo.freq 	– LFFG Frequency (Hz)
		lfo.duty	– LFFG Duty cycle
		gain 		– Gain

> ->VOL (16): OFF(0)|ON(1) 0 | Profile: (0-10) 1 | Frequency (0.2-5)(Hz): 2.000 | Duty Cycle (0-100)(): 50.0 | Gain (0-1): 1.000

### WahWha:

Modifies the tone of the input signal with a LFO

	wah [([+][]c)] s lfo.profile lfo.freq lfo.duty gain
		lfo.profile 	– LFFG Profile
		lfo.freq 	– LFFG Frequency (Hz)
		lfo.duty	– LFFG Duty cycle
		gain 		– Gain 

> ->WAH (6): OFF(0)|ON(1) 0 | Profile: (0-10) 1 | Frequency (0.2-5)(Hz): 2.000 | Duty Cycle (0-100)(): 50.0 | Gain (0-1): 1.000




ovd
phr (5)
tml (1)
cmp (+)
dfb 1 22 0.8
vbt
colocar as respostas.


->CMP (0) 0 10.0 1000.0 20 40
->LVD: Attack (ms):    1.000 | Release (ms): 1000.000
->OVD (1) 0 0.500 0.800 1.000
->PHR (2) 0 0.500 10.0 0 0.250 50.0 1.000
->OCT (3) 0 0.500 1.000
->SFT (4) 0 5.000 0.500  1.000
->DTN (5) 0 5.000 0.500 1.000
->WAH (6) 0 1 2.000 50.0 1.000
->EQZ (7) 0 1.000 1.000 1.000 200.000 800.0
->CHS (8) 0 5.0 1.0 0.500 0 0.500 50.0 1.000
->VBT (9) 0 5.0 1.0 0 0.500 50.0 1.000
->RVB (10) 0 1000.0 1.000
->DFB (11) 0 31.0 0.700 1.000
->EFB (12) 0 1000.0 0.700 1.000
->DFF (13) 0 31.0 0.900 4 1.000
->EFF (14) 0 1000.0 0.900 4 1.000
->TML (15) 0 1 2.000 50.0 1.000
->VOL (16) 0 1 2.000 50.0 1.000
->LIM (17) 0 1.000 1.000
->NGT (18) 0 10.0  1000.0 1.000 0.100


