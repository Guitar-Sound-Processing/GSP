# GSP Command Decoder

## Description 

The Guitar Sound Processing is configured by interpreted commands by Daisy Seed (DS) and by the External Device - ED (ESP32 or Arduino, for instance), through serial interfaces or USB. There are commands to change the effects setup and the effect position in chain. More specificaly DS can accept commands to:

- change effect parameters,
- select and activate the effect in chain,
- configure and set the effect sequence on chain,
- link a given effect to an expression pedal.

On the other hand, the ED will be in charge to

- select pre-recorded chain and effect settings,
- store chains and effect setups on SSD memory,
- communicate with the application software to receive and retransmit commands,
- read Volume or Expression pedals and send these data to DS

Therefore, from the Application point of view, some of the commands are interpreted by the Daisy Seed, while others are interpreted by the External Device, aiming to let the DS to process the effect algorithms instead to interpret all commands. DS is then responsible to interpret a minimum set of commands, while the External Device interpret high level commands. 

The complete list of configuration commands to GSP comprises

- Effects commands
- Chain commands
- Interface commands
- Expression Pedal commands

All these effects are processed and interpreted by the Daisy Seed board. There are also some available commands designed to select a whole chain configuration stored on SSD. These and other commands are interpreted by the External Device, like ESP32, explained in:

- Chain profiles

The effect modules are codified in classes of C++, and obey some guidelines described in

- Effect Software Directives

## GSP Programming 

GSP is programmable through ASCII commands from available serial lines. Presently GSP monitors the USB (serial) and UART_1 serial lines. Input commands from serial lines are interpreted and executed by internal GSP procedures. The command string shall terminate with standard ASCII control characters Carriage Return (CR - D13) or Line Feed (LF - D10). After receiving a command, the interpreter answers with knowledge prompt. For instance, the input command:

```OVD```

is answered with:

> ```->OVD (1): OFF(0)|ON(1) 0 | Sustain (0.1-1): 0.500 | Tone (0-1): 0.800 | Gain (0-1): 1.000```

## Available Effects:

Currently there are 19 available effects in GSP 1.0.0, besides a multi-purpose Low Frequency and Function Generator (LFFG) to provide low frequency audio modulation for several effects. The available effects are:

- Chorus – Mixes the input signal with a LFFG delayed copy.
- Compressor – Compress the input signal to increase the dynamic range.
- Delay Feedback – Mixes the input signal with a feedback attenuated and delayed copy.
- Delay Feedforward – Mixes the input signal with a fixed number of attenuated and delayed copies.
- Detune – Decreases continuously the pitch, to up one octave below.
- Echo Feedback– Mixes the input signal with a feedback time delayed copy.
- Echo Feedforward – Mixes the input signal with a fixed number of attenuated and long time delayed copies.
- Equalizer – Three-band equalizer with adjustable frequencies.
- Limiter – Applies a non-linear threshold on the input level.
- Noise Gate – Mutes the output signal when the input power falls below a given threshold.
- Octave – Increases the pitch by one octave (double frequency).
- Overdrive – Distortion based on Big Muff.
- Phaser – Mixes the input signal with an out-of-phase copy driven by a LFFG.
- Pitch Shifter – Increases continuously the pitch, up to one octave above.
- Reverber – Simulates the sound of reverberation rooms.
- Tremolo – Applies a LFFG envelop to the amplitude of the input signal.
- Vibrato – Produces a LFFG delayed signal that mimics a vibrato.
- Volume – Applies a LFFG to the input signal (same as Tremolo).
- Wahwah – Applies a BPF (Band Pass Filter) adjusted by a LFFG.

There is also a Level Detector which isn’t exactly an effect, but rather a user configurable level measurement of the input signal, before the signal is changed by any effect:

- Level Detector – Measures the level of the input signal. It can be used to drive the LFFG (Low Frequency Function Generator) when the selected envelop amplitude is LFO_POWER mode.

Besides these effects, a Tone_LPHP (Low-Pass High-Pass or Treble) class was developed for the Overdrive effect, but can be used in any future developments.

Some effects share the same code, but with different default parameters. They are:

- Chorus and Vibrato
- Delay Feedback and Echo Feedback
- Delay Feedforward and Echo Feedforward
- Tremolo and Volume

GSP software is composed by the effect algorithms and command interface modules. The algorithms will be described later on separated document. The user interface is shown here.

## Parameter format

On the description of the commands, the following rules apply to the parameters:
 
- {var} indicates a numeric value in ASCII
- \<var\> indicates a numeric value sent by serial line in binary format
- /var\ indicates that the string stored in the variable var is sent in ASCII code.
- *efc* indicates a command or effect name like ovd, phr, wah, pot, and others.

