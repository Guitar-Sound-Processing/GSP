# Driver Board

The Driver Board (DB) provides audio signal conditioning to the Analogue to Digital Convertion (ADC) of Daisy Seed board. Additionally DB perform filtering and amplification of Daisy's audio output from Digital to Analogue Convertion (DAC). 
<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/driver_board_1.png" width="432" height="241" alt="Driver Board"></p>

The major components of the Driver Board are, as shown in the figure above:

- a quadruple operation amplifier LM324 for signal conditioning and amplification
- an analogue Compressor and Expander NE570
- 3 hadware switches to bypass the compressor, expander and Daisy Seed board
- 6 trimpot potentiometers to adjust the input level, Compressor output level, ADC input level, DAC output level, Expander output level, and GSP output level

The Driver Board has a quadruple operation amplifier LM324 to amplify the input signal from guitar at approximately 1V maximum amplitude up to 3.5V (biased at +4.5V) for compressor input, using the input level trimpot. A sine wave signal of 1V amplitude at 440 Hz can be used at input port to trim the potentiometers. The amplitude of the compressor output shall be adjusted by the Compressor output trimpot to have the same level of its input signal. Then the signal is biased to +1.6V and to 1.6V amplitude (0 to 3.2V) using the ADC input trimpot. After being digitally processed by the [Daisy Seed Shield](https://github.com/Guitar-Sound-Processing/GSP/blob/main/hardware/DS_board.md), the analogue signal returns to the Driver Board and pass to a second order low pass filter. It is then biased again to +4.5V and amplified to 3.5V by the DAC output trimmer before entering in to the Expander. The output amplitude of the Expander shall be adjusted by the Expander output trimpot to the same as the input level. Finally the DC level is removed from the output signal and amplified so as to present the same level of the guitar input signal (1V amplitude).

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/driver_board_pcb.png" width="302" height="533" alt="Driver Board PCB"></p>

DB was designed using the free version of Autodesk Eagle. Future versions will be based on Kicad. Moreover the board was conceived so as to avoid SMD components, to make it easy to populate by DIY users. The schematics are available to download as well as the board layout in Eagle files. For those that prefer Kicad instead of Eagle, is worth to mention that Kicad easily opens and converts Eagle files.

<font color="red">Important notes:</font>

  1) Diodes D1 and D2 are protection diodes. They are NOT really required and can distort the audio signal. Please ignore this diode unless you feel unsecured.
  2) Resistor R24 is just an air jumper (0 Ohm). It gives more flexibility case the output of the Expander does not reach the desired voltage.

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/driver_board_sch.png" alt="Driver Board SCH"></p>



