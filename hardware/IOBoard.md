# Input and Output Board

The Input and Output Board offers some small funcionalities to GSP, although they are not fully necessary. So the IO Board hosts three 1/4 inch jacks, for guitar input, signal output and Expression Pedal. For input and output signals only the sleeve and tip contacts are needed. Of course the Expression Pedal jack uses stereo contacts. The board also holds contact for nRF24L01 wireless module. Conection to the [ESP32 Shield](https://github.com/Guitar-Sound-Processing/GSP/blob/main/hardware/ESP32_shield.md) is provided by two JST PH 4 pin headers, but it is only necessary if wireless Expression Pedals are going to be attached to GSP. Finally there is a IC female socket for ADUM1201 magnetic isolator module, and two 4 pin JST PH headers to connect this module to [ESP32 Shield](https://github.com/Guitar-Sound-Processing/GSP/blob/main/hardware/ESP32_shield.md).

Therefore the IO Board has the following connectors:
- A JST PH 2 pin header for guitar input connection to the GSP [Driver Board](https://github.com/Guitar-Sound-Processing/GSP/blob/main/hardware/DriverBoard.md).
- A JST PH 2 pin header for guitar output connection from the GSP [Driver Board](https://github.com/Guitar-Sound-Processing/GSP/blob/main/hardware/DriverBoard.md).
- Two JST PH 4 pin header for magnetic isolator module connection with [ESP32 Shield](https://github.com/Guitar-Sound-Processing/GSP/blob/main/hardware/ESP32_shield.md).
- - Two JST PH 4 pin header nRF24L01 module connection to [ESP32 Shield](https://github.com/Guitar-Sound-Processing/GSP/blob/main/hardware/ESP32_shield.md).

Expression Pedals comes in two polarities, depending the manufacturer. Moog, Roland, M-audio and Hammond, among others, adopt the RTS (Ring, Tip, Sleeve) configuration, and Yamaha, Korg and Behringer use the TRS (Tip, Ring and Sleeve). The order of RTS or TRS corresponds to the CCW-Wiper-CW of the potentiometer. So the Wiper can be attached to the Ring or the Tip, depending on the pedal. On the other hand, the power line (in this case, the 3.3V line can be connected to the Tip or the Ring. Sleeve always get the Ground line. The IO Board can be configured to support both arquitectures with jumpers in the R-P-T and T-VC-R headers. When R-P and T-VC are jumped (RTS), Wiper goes to Ring and 3V3 goes to Tip. If P-T and VC-R are jumped (TRS) then Wiper will be attached to Tip and 3V3 to Ring. 

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/IO_board.png" width="343" height="526" alt="DCDC Module"></p>

Some of these connectors are already connected to the ESP32 pins, although they need jumpers to enable connection. They are:

 - Vin jumper connects the 5Vin (input power) to ESP32 VIN pin.
 - Gnd jumper joins the input ground from 5Vin input power to ESP32 Wrover ground pins, besides grounding both Expression Pedal headers and the ground pin of 3V3 output header.
 - 3V3 jumper enables the connection between the 3V3 output from ESP32 to the Expression Pedal headers and the 3V3 output connector for magnetic isolator module.
 - Tx1, Rx1, Tx2 and Rx2 jumpers to connect the JST headers for Serial lines to ESP32 pins: Tx1-GPIO23, Rx1-GPIO22, Tx2-GPIO19, Rx2-GPIO18.

In normal situation all these jumpers shall be wired. The ESP32 Wrover TIGO module has 34 pins (2x17). However the board has 36 (2x18) pins, as can be seen in figure above, since it is difficult to find female headers with 17 pins. So care must be taken in order to avoid placing the Wrover module in the wrong pins. 

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/IO_sheet.png" width="772" height="533" alt="DCDC Board1"></p>
