# Interface Commands

Some additional commands were created to visualize GSP performance.

### Verbose

Toggle the verbose (command echoes) on and off (off is default). When verbose is on, GSP echoes any command to serial line with leading “<-” string. 

	vbs

By sending this command, GSP answers with

> ->Verbose on

and by sending ```vbs``` again it reply with

> ->Verbose off

When verbose is on, the command ```ovd 0 0.9``` is answered with

> <-ovd 0 0.9 <br>
> ->OVD (1): OFF(0)|ON(1) 0 | Sustain (0.1-1): 0.900 | Tone (0-1): 0.800 | Mixer (0-1): 1.000 | Gain (0-1): 1.000 </br>

### Processing time

Print the processing time at 1 Hz frequency, in percentage of the duty cycle. Small values indicate low processing charges, while high values (close to 90%) indicate overcharging on processor.

	out

The ```out``` command turns on and off the output time. For instance, this command produces the following output in console:

> ->Duty Time on <br>
> 5.286427<br>
> 5.279953<br>
> 5.289541<br>
> 5.291170<br>
> 5.290229<br>
> 5.290130<br>
> 5.289708<br>
> ->Duty Time off</br>

### Standard reply

This command selects between normal printings on console, if *f* is zero, the short format with numeric values only, if *f* is equal to 1, or muted output, if *f* is -1. Any other value or no value at all is considered by GSP as *f* equal to 0. 

	fmt [f]
 		f 	Output reply parameter: standard:0 | short=1 | muted=-1 

The normal output, for instance, is:

> ->OCT (3): OFF(0)|ON(1) 0 | Mixer (0-1): 0.500 | Gain (0-1): 1.000

while the short format is:
 
> ->OCT (3) 0 0.500 1.000 

The short format can be useful for copying and pasting when configuring chain profiles. Care must be taken with *f* = -1, since there will no information if a wrong command is sent to GSP. However, verbose can still work if activated. 

GSP replies to ```fmt 0``` command with

> ->FMT: Mute(-1)|Complete(0)|Data only(1) 0
