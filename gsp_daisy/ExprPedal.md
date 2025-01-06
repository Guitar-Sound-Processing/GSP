# Expression Pedal Linkage

GSP can handle up to 8 expression pedals, to drive effects like Volume, Wah-Wah and several others. The External Device reads the potentiometer voltage, scale it and send to Daisy Seed by serial line at 10 Hz frequency.

## Parameter format

On the description of the commands, the following rules apply to the parameters:
 
- {var} indicates a numeric value in ASCII
- \<var\> indicates a numeric value sent by serial line in binary format
- /var\ indicates that the string stored in the variable var is sent in ASCII code.
- *efc* indicates a command or effect name like ovd, phr, wah, pot, and others.

## Expression Pedal Command

The command ```pot``` links a given effect configured with LFO_EXTERNAL profile in the LFFG to an expression pedal.

	pot /efc\ {rid}
		efc 	– Effect name (wah, phr, tml, chs or vbt)
		rid 	– Potentiometer identifier number

The GSP software on DS exchanges data with the External Device processor (ESP32, for instance) through serial interface using predefined commands to request and to deliver potentiometer data. Each potentiometer attached to the external processor will receive a numeric identifier based on its connection. The interface identifiers are predefined by software and can’t be changed. Up to 8 identifiers can be used, numbered from 0 to 7. The link between the potentiometer identifier and the associated effect is done by the Expression Pedal command. 

If ```rid``` is negative, disregarding its value, the effect will be unassigned from its previously attached pot. However, one has to take care to avoid sending something like -0, since negative zero has no useful meaning. if ```rid``` is absent the command is disregarded.  A given potentiometer identifier can be assigned to more than one effect on the chain, although the inverse isn’t valid.

When receiving a ```pot``` request, GSP answers with 

> ->POT: Effect: *efc* | Pot ID: *k*

where *efc* is the requested effect and *k* is the associated potenciometer ID. If there is no previously Expression Pedals configured with the ```pot``` command, GSP answers with \<none>. The standard reply for a ```pot``` command without parameters is a list of all Expression Pedals attached on GSP so far.

To attach Volume in channel 5 and Wah-Wah on channel 3, the following commands must be sent:

```pot```
> ->POT: \<none>

```pot vol 5```
> ->POT: Effect: VOL | Pot ID: 5

```pot```
> ->POT: Effect: VOL | Pot ID: 5

```pot wah 3```
> ->POT: Effect: WAH | Pot ID: 3

```pot```
> ->POT: Effect: WAH | Pot ID: 3 <br>
> ->POT: Effect: VOL | Pot ID: 5 </br>

After receiving the Expression Pedal command, the DS sends an assign command to the External Device (ED) with the sequence of potentiometer data that ED is required to send to Seed. This command has the following format

	A{r1}{r2}…{rm}\n

It starts with ASCII A, with one or more ```rid``` identifiers in sequence. The ```rn``` (n = 1, 2, ..., m) is not a true ASCII number, but, instead, it is the potentiometer identifier number plus 48, which means that the sequence matches the ASCII table characters for identifiers from 0 to 9, but identifier 10 is, therefore, 58 (“:”) and so on. This strategy allows GSP to have more than 9 identifiers (up to 207) with only one byte and avoids having problems with characters in the non-printing range of ASCII table. The carriage return terminator ```(\n)``` indicates the command has finished. The number of potentiometer data to be sent to DS is, therefore, given by the number of identifiers present in this command. Two or more ```rn``` may refer to the same potentiometer identifier, since two or more effects can share the same potentiometer, like ```A0323```, for instance.

Besides the ```A``` command, DS can send also two other commands: ```S``` and ```C```. Both are internal commands, without direct user interference.

The ```S``` command states that the External Device shall start sending potentiometer data, at a rate of 10 Hz, with two bytes (one word) each, in the format specified bellow. The ```C``` command orders the ED to stop sending data and to clear the potentiometers field (number of pot data is zero).

After reading the potentiometer voltages, ED sends the data to DS by serial line in following binary format:

	P<m><P1><P2><P3>…<Pm>\n

in which 
- ```m``` is  the number of expression pedals data contained in the message, in binary format, one byte, and 
- ```P1…Pm``` are the potentiometer data, scaled from 0 up to 65535 (two bytes), less significant byte first.

As an example, let us assume 3 expression pedals connected to pot identifiers 0, 2, and 5, where ID 0 is shared between two effects. Daisy Seed then sends to ED the command ```A0205\n```, followed by the ```S``` command. ED then starts sending the expression pedals data at 10 Hz with messages in the format ```P<4><P0><P2><P0><P5>\n```. One may note that only the characters ```P``` and the terminator ```\n``` are mapped to ASCII table. ```m``` and ```Pi``` (```i``` = 1, …, ```m```) are all sent in binary format. Of course ```m``` defines the number of binary data received from the DS side.

Nor Daisy Seed or the ED can identify if there really is an expression pedal attached to a given pedal input. So the user has to take care in order to select the appropriate identifier when linking it to a given effect.

For instance, if two Expression pedals are selected with commands:

 	pot tml 5
	pot wah 3

then DS will send this request command to ED 

	A<51><53>\n	

in which 51 = 3 + 48 and 53 = 5 + 48. After receiving the enable sending data command, ```S```, ED answers with the potentiometer data sent at 10 Hz frequency:

	P<2><P3><P5>\n


