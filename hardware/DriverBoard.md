# Driver Board

The Driver Board (DB) provides audio signal conditioning to the Analogue to Digital Convertion (ADC) of Daisy Seed board. Additionally DB perform filtering and amplification of Daisy's audio output from Digital to Analogue Convertion (DAC). 
<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/driver_board_1.png" width="432" height="241" alt="Driver Board"></p>

The major components of the Driver Board are, as shown in the figure above:

- a quadruple operation amplifier LM324 for signal conditioning and amplification
- an analogue compressor and expander NE570
- 3 hadware switches to bypass the compressor, expander and Daisy Seed board
- 6 trimpot potentiometers to adjust the input level, compressor output level, ADC input level, DAC output level, expander output level, and GSP output level

The Driver Board has a quadruple operation amplifier LM324 to amplify the input signal from approximately 1V guitar amplitude up to 7V for compressor 
