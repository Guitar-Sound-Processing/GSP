# Effect Software Directives

There are a small set of rules regarding software coding for effect support. All effects shall be coded in C++ classes. 

## Standard headers

There are standard headers available in GSP that are accessible by any effect:

	guitar_dsp.h
 
This header defines numerical parameters for unit conversion and integer ranges:

```#define GDSP_PI 	3.14159265358979323846f 	/* pi */
#define GDSP_2PI 	6.283185307179586476925f 	/* 2 pi */
#define ADC_RES 	65536				/* ADC 16 bits */
#define ADC_HALFRES 	32768				/* ADC 16 bits/2 */
#define ADC_MAXVAL 	32767				/* ADC maximum value */
#define ADC_MINVAL 	-32768 				/* ADC minimum value */
#define ADC_INVHRESF 	0.000030517578125f 		/* 1/ADC_16_HALFRES */
```

and effect switching values:

```
#define GSP_ON 		1
#define GSP_OFF 	0
```
## Standard effect names
	
The GSP effect class shall be addressed by 

	GSP_\EffectName\

There are two kind of methods required for any GSP effect: [processing](#prcsmt) methods and [configuration](#cfgr) methods. These are presented below.

### <h3 id="prcsmt">Processing methods</h3>

Each effect class shall have standard methods to process the audio signal. Only three processing methods are required in GSP:

- [Init](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/SWDirectives.md#init-methods)
- [Process](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/SWDirectives.md#process-methods)
- [Switch](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/SWDirectives.md#switch-method)

#### Init methods

There are two versions of Init method:

	GSP_\EffectName\::Init(uint32_t sampling_rate)
	GSP_\EffectName\::Init(uint32_t sampling_rate, int16_t *ptr_buffer, uint32_t buffer_size)

The first one can be employed in effects that do not require buffers to store audio samples, like Overdrive, for instance. The second one provides pointers to access the buffer and its size, like the Delay effects. Caution must be taken, since the buffer is for read-only purposes. The audio samples are store in the buffer by the GSP effects loop.

#### Process methods

Similarly, there are two Process methods: 

	int32_t GSP_\EffectName\::Process(int32_t sampl,)
	int32_t GSP_\EffectName\::Process(int32_t sampl, uint32_t buffer_pointer)

to support effects that need and do not need audio buffer. The ```sampl``` input is the current audio sample (mono channel), and ```buffer_pointer``` is the current storage position of the audio sample in ```ptr_buffer```. This buffer is updated with the current audio sampler by GSP before any effect processing. Exceptions are the Feedback Delay and Feedback Echo effects, which updates the audio buffer with the processed audio. Any effect after these ones in chain who needs the audio buffer will be affected by the changes made by the GSP in the buffer’s content. Reverber uses its own buffer.

The Process method shall perform all the necessary computation to change the audio sample according to the effect algorithm. The output of Process method is the processed audio sample which is transferred to the main GSP loop by the returning value. 

It is important to note that GSP Main Loop retrieves the audio samples from Daisy Seed in ```float``` format, but delivers it to the Process methods in ```int32_t``` format. Numerical computation using integer numbers are significantly faster than floating, although scaling some variables is sometimes required. Not any effect can be processed with integer computation. In this case casting the sample input to ```float``` and the output to ```int32_t``` inside the Process method can be easily done.

#### Switch method

The Switch method stores the effect on-off (by-pass) switch state, and has the format:

	void GSP_\EffectName\::Switch(uint8_t mode)

where ```mode``` can be ```GSP_ON``` or ```GSP_OFF```. The GSP main loop is responsible to act according to the Switch state, which means that the Process method shall not interpret the switching state.

### <h3 id="cfgr">Configuration methods</h3>

The methods usually employed for effect configuration can be classified in two categories: Single and Multiple parameter configurations. 

#### Single parameter config

The Single parameter configurations are methods required to set and to retrieve a single parameter. Methods to set a given parameter are usually named as SetParameter whereas methods to retrieve its value shall be named GetParameter. Both methods shall address one and only one parameter. Obviously the heading of these methods are:

	void GSP_\EffectName\::SetParameter(\type\ NewValue)
	\type\ GSP_\EffectName\::GetParameter()

The configuration parameters are dependent of the effect and may change between them. Common parameters are, for instance:

- Gain
- Mixer
- Depth
- Treble
- Volume, etc.

However the ```state``` parameter shall be included in any effect to store the Switch status (```GSP_ON``` or ```GSP_OFF```). GSP gets this parameter in main loop to call or to by-pass each effect according to its Switch state.

#### Multiple parameter config

There are three methods to store and to retrieve multiple effect parameters, but all these methods are not essential for proper effect operation: they just make coding easier. They are:

	void GSP_\EffectName\::Printout(uint8_t out_list, int32_t chn_pos, char *printout)
	void GSP_\EffectName\::GetParams(float fn[])
	void GSP_\EffectName\::SetParams(float fn[])

The Printout method writes all the effect parameters in the output buffer ```*printout``` in ASCII format (maximum 250 characters), according to the standard output format, like the normal (long) form :

> ->OVD (1): OFF(0)|ON(1) 0 | Sustain (0.1-1): 0.500 | Tone (0-1): 0.800 | Gain (0-1): 1.000 

or the short format:

> ->OVD (1) 0 0.500 0.800 1.000

The ```out_list``` selects the required output format: long or short (see ```fmt``` in Interface Commands), while the input ```chn_pos``` has the current effect position in the chain (see Effect Commands section).

The GetParams method retrieves all the effect parameters in a single float array ```fn```. The parameters shall be stored in ```fn``` in the same sequence as they appear in Command line, always starting with the switch state (Switch method). Typecast and unit conversion, if required, must be internally made by GetParams method. 

The SetParams method set all the effect parameters with the values stored in the float array ```fn```, including the Switch state. The GSP command interpreter retrieves the parameter values from Command line and calls the corresponding SetParams method to change the effect setup. Normally SetParams just provides callings to the single parameter configuration methods explained before.


