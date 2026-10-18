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

The command ```pot``` links a given effect configured with LFO_EXTERNAL profile in the [LFFG](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/LFFG.md) to an expression pedal.

	pot /efc\ {rid}
		efc 	– Effect name (wah, phr, tml, chs, vbt or vol)
		rid 	– Potentiometer identifier number

The GSP software on DS exchanges data with the External Device processor (ESP32, for instance) through serial interface using predefined commands to request and to deliver potentiometer data. Each potentiometer attached to the external processor will receive a numeric identifier based on its connection interface (GPIO). The interface identifiers are predefined by software and can’t be changed. Up to 8 identifiers can be used, numbered from 0 to 7. The link between the potentiometer identifier and the associated effect is done by the Expression Pedal ```pot``` command. 

If ```rid``` is negative, disregarding its value, the effect will be unassigned from its previously attached pot. However, one has to take care to avoid sending something like -0, since negative zero has no useful meaning. If ```rid``` is absent the command is disregarded.  A given potentiometer identifier can be assigned to more than one effect on the chain, although the inverse isn’t valid.

When receiving a ```pot``` request, GSP lists all the effects using [LFFG](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/LFFG.md) and their associated potentiometer identifier in the format:

> ->POT Effect: *efc* | Pot ID: *k*

where *efc* is the effect acronym and *k* is the associated potenciometer ID. To attach Volume in channel 5 and Wah-Wah on channel 3, the following commands must be sent:

```pot vol 5```
> ->POT Effect: PHR | Potentiometer ID: -1 <br>
> ->POT Effect: WAH | Potentiometer ID: -1 <br>
> ->POT Effect: CHS | Potentiometer ID: -1 <br>
> ->POT Effect: VBT | Potentiometer ID: -1 <br>
> ->POT Effect: TML | Potentiometer ID: -1 <br>
> ->POT Effect: VOL | Potentiometer ID: 5 </br>

```pot wah 3```
> ->POT Effect: PHR | Potentiometer ID: -1 <br>
> ->POT Effect: WAH | Potentiometer ID: 3 <br>
> ->POT Effect: CHS | Potentiometer ID: -1 <br>
> ->POT Effect: VBT | Potentiometer ID: -1 <br>
> ->POT Effect: TML | Potentiometer ID: -1 <br>
> ->POT Effect: VOL | Potentiometer ID: 5 </br>

GSP answers to the ```pot``` command without parameters with the current list of all the effects which use LFFG with their associated potentiometer identifier:

```pot```
> ->POT Effect: PHR | Potentiometer ID: -1 <br>
> ->POT Effect: WAH | Potentiometer ID: 3 <br>
> ->POT Effect: CHS | Potentiometer ID: -1 <br>
> ->POT Effect: VBT | Potentiometer ID: -1 <br>
> ->POT Effect: TML | Potentiometer ID: -1 <br>
> ->POT Effect: VOL | Potentiometer ID: 5 </br>

The answer to the ```pot``` command is affected by the Standard Reply ```fmt``` command. If the short answer is selected, then GSP replies to any ```pot``` command by printing a single line with the current configuration

> ->POT PHR -1 WAH 3 CHS -1 VBT -1 TML -1 VOL 5 

## Potentiometer data 

There are three potentiometer commands to control the data sent to Daisy Seed: the ```A```, ```S``` and ```C``` commands. These are internal commands between Daisy Seed and ESP32, without direct user interference.

### Command A

After receiving the Expression Pedal command, GSP running in Daisy Seed sends an assign command to the External Device (ED) with the sequence of required potentiometer data that ED shall send to Daisy Seed. This command has the following format

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

GSP on Daisy Seed operates with the potentiometer data in 16 bits. However, sending data in 16 bits could cause message truncations since the least or most significant byte could lie in the non printing range of ASCII table. Data is then converted to two bytes with 7 bits each (ESP32 ADC has 12 bits) by ESP32 and both most significant bit are set to 1, assuring the bytes aren't on printing range. For instance, let's say that data read in 14 bits was 12557 (0x310D), which corresponds to decimal 13 (Carriage Return, binary = 0000 1101) for the least significant byte and 49 (binary = 0011 0001) for the most significant byte. These bytes are then converted to 7 bit bytes, by left shifting one digit the most significant byte, and placing the most significant bit of the least significant byte (0, in this case) at right of it. The next figure explains how convertion of a 14 bit data into two bytes is done.

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/pot_data.png" width = "413", alt="Potentiometer data convertion"></p>

As an example, let us assume 3 expression pedals connected to pot identifiers 0, 2, and 5, where ID 0 is shared between two effects. Daisy Seed then sends to ED the command ```A0205\n```, followed by the ```S``` command. ED then starts sending the expression pedal data at 10 Hz with messages in the format ```P<4><P0_lsb><P0_msb><P2_lsb><P2_msb><P0_lsb><P0_msb><P5_lsb><P5_msb>\n```. One may note that all bytes are sent in ASCII. Of course ```m``` defines the number of data received from the DS side.

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
