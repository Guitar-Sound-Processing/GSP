# Interface Commands

Some additional commands were created to visualize GSP performance.

### Verbose

Toggle the verbose (command echoes) on and off (off is default). When verbose is on, GSP echoes any command to serial line with leading “<-” string. 

    vbs

By sending a ```ovd 0 0.9``` command with verbose on, for instance, GSP answers with

> <-ovd 0 0.9 <br>
> ->OVD (1): OFF(0)|ON(1) 0 | Sustain (0.1-1): 0.900 | Tone (0-1): 0.800 | Mixer (0-1): 1.000 | Gain (0-1): 1.000 </br>

### Processing time

Print the processing time at 1 Hz frequency, in percentage of the duty cycle. Small values indicate low processing charges, while high values (close to 90%) indicate overcharging on processor.

```out```

### Standard reply

This command selects between normal printings on console, if *f* is zero, the reduced format with numeric values only, if *f* is equal to 1, or muting output, if *f* is -1. Any other value or no value at all is considered by GSP as *f* equal to 0. 

```fmt [*f*]```

The normal output, for instance, is:

> ->OCT (3): OFF(0)|ON(1) 0 | Mixer (0-1): 0.500 | Gain (0-1): 1.000

while the short format is:
 
> ->OCT (3) 0 0.500 1.000 

The short format can be useful for copying and pasting when configuring chain profiles. Care must be taken with *f* = -1, since there will no information if a wrong command is sent to GSP. However, verbose can still work if activated. 

