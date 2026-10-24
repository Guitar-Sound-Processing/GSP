# Expression Pedal Linkage

GSP can handle up to 8 expression pedals, to drive effects like Volume, Wah-Wah and several others. The External Device reads the potentiometer voltage, scale it and send to Daisy Seed by serial line at 10 Hz frequency.

## Important Note

GSP was designed to communicate to External Device through 2 serial lines:

- a serial line dedicated to receive effect commands and to send replies, and
- a second serial line to send Expression Pedal commands and to receive potentiometer data

However, although some old versions of libDaisy UART driver can still handle multiple serial lines, there has been a lot of effort from Daisy's team to provide a powerful and reliable driver. The most recent driver (8.1 at the time of this writing) still shows some issues when more than one serial line needs to be configured. Unfortunately, there was no other way than combining both activities (effect and expression pedal commands) in a single serial line. So, any message sent or received by both Daisy Seed and External Device shall initiate with a disambiguation character. It was chosen the character "{" (123) to indicate an effect command or effect reply, and "}" (125) for expression pedal commands and potentiometer data. Of course this may change in future versions of GSP when, and if, two serial lines could be independently configured in libDaisy. Please note that the description below does not show the disambiguation character, since it is planned to be removed in future versions.

## Parameter format

On the description of the commands, the following rules apply to the parameters:
 
- {var} indicates a numeric value in ASCII
- \<var\> indicates a numeric value sent by serial line in binary format
- /var\ indicates that the string stored in the variable var is sent in ASCII code.
- *efc* indicates an effect name like ```phr```, ```tml```, ```wah``` and others.

## Expression Pedal Command

The Expression Pedal can be selected by the External profile as 8 in the LFFG (LFO_EXTERNAL). The effects that requires the LFFG are Chorus (chs), Phaser (phr), Tremolo (tml), Vibrato (vbt), Volume (vol) and Wahwah (wah). Note, however, that the output level of any GSP effect can be controlled by the Volume effect on the chain's output (```>Inp->(LVD)->CMP->OVD->EQZ->VOL->Out```) which allows controling the output level of the chain, provided the Volume has been appropriated configured with External profile.

## Potentiometer data 

There are three potentiometer commands to control the data sent to Daisy Seed: the ```A```, ```S``` and ```C``` commands. These are internal commands between Daisy Seed and ESP32, without direct user interference.

### Command A

After receiving the Expression Pedal ```pot``` command, GSP running in Daisy Seed sends an assign command to the External Device (ED) with the sequence of required potentiometer data that ED shall send to Daisy Seed. This command has the following format

	A{r1}{r2}…{rm}\n

It starts with ASCII A, with one or more ```rid``` identifiers in sequence. The ```rn``` (n = 1, 2, ..., m) is not a true ASCII number, but, instead, it is the potentiometer identifier number plus 48, which means that the sequence matches the ASCII table characters for identifiers from 0 to 9, but identifier 10 is, therefore, 58 (“:”) and so on. This strategy allows GSP to have more than 9 identifiers (up to 207) with only one byte and avoids having problems with characters in the non-printing range of ASCII table. The carriage return terminator ```(\n)``` indicates the command has finished. The number of potentiometer data to be sent to DS is, therefore, given by the number of identifiers present in this command. Two or more ```rn``` may refer to the same potentiometer identifier, since two or more effects can share the same potentiometer, like ```A0323```, for instance.

### Command S

The ```S``` command states that the External Device shall start sending potentiometer data, at a rate of 10 Hz, with two bytes (one word) each, in the format specified bellow. 

After reading the potentiometer voltages and receiving the ```S``` command, ED sends the data to DS by serial line in following binary format:

	P<m><P1_lsb><P1_msb><P2_lsb><P2_msb><P3_lsb><P3_msb>…<Pm_lsb><Pm_msb>\n

in which 
- ```m``` is  the number of expression pedals data contained in the message, in binary format, one byte,  
- ```P1…Pm``` are the potentiometer data, scaled from 0 up to 16383 (14 bits).
- ```_lsb``` and ```_msb``` are the least and most significant byte of potentiometer data.

GSP on Daisy Seed operates with the potentiometer data in 16 bits. However, sending data in 16 bits could cause message truncations since the least or most significant byte could lie in the non printing range of ASCII table. Data is then converted to two bytes with 7 bits each (ESP32 ADC has 12 bits) by ESP32 and both most significant bit are set to 1, assuring the bytes are on printing range. For instance, let's say that data read in 14 bits was 12557 (0x310D), which corresponds to decimal 13 (Carriage Return, binary = 0000 1101) for the least significant byte and 49 (binary = 0011 0001) for the most significant byte. These bytes are then converted to 7 bit bytes, by left shifting one digit the most significant byte, and placing the most significant bit of the least significant byte (0, in this case) at right of it. The next figure explains how convertion of a 14 bit data into two bytes is done.

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/pot_data.png" width = "413", alt="Potentiometer data convertion"></p>

The inverse procedure is then carried on from DS side in order to restitute the original potentiometer data. As an example, let us assume 3 expression pedals connected to pot identifiers 0, 2, and 5, where ID 0 is shared between two effects. Daisy Seed then sends to ED the command ```A0205\n```, followed by the ```S``` command. ED then starts sending the expression pedal data at 10 Hz with messages in the format ```P<4><P0_lsb><P0_msb><P2_lsb><P2_msb><P0_lsb><P0_msb><P5_lsb><P5_msb>\n```. One may note that all bytes are sent in ASCII. Of course ```m``` defines the number of data received from the DS side.

Nor Daisy Seed nor the External Device can identify if there really is an expression pedal attached to a given pedal input. So the user has to take care in order to select the appropriate identifier when linking it to a given effect.

For instance, if two Expression pedals are selected with commands:

 	pot tml 5
	pot wah 3

then DS will send this request command to ED 

	A<51><53>\n	

in which 51 = 3 + 48 and 53 = 5 + 48. After receiving the enable command ```S``` to send data, ED answers with the potentiometer data sent at 10 Hz frequency:

	P<2><P3_lsb><P3_msb><P5_lsb><P5_msb>\n

### Command C

The ```C``` command orders the ED to stop sending data and to clear the potentiometers field (number of pot data is zero).
