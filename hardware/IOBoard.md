# Input and Output Board

The Input and Output Board offers some small funcionalities to GSP, although they are not fully necessary. The IO Board hosts three 1/4 inch jacks, for guitar input, signal output and Expression Pedal. For input and output signals only the sleeve and tip contacts are needed. Of course the Expression Pedal jack uses stereo contacts. The board also holds contact for nRF24L01 wireless module. Conection to the [ESP32 Shield](https://github.com/Guitar-Sound-Processing/GSP/blob/main/hardware/ESP32_shield.md) is provided by two JST PH 4 pin headers, but it is only necessary if wireless Expression Pedals are going to be attached to GSP. Finally there is a IC female socket for ADUM1201 magnetic isolator module, and two 4 pin JST PH headers to connect this module to [ESP32 Shield](https://github.com/Guitar-Sound-Processing/GSP/blob/main/hardware/ESP32_shield.md). However, this isolator isn't really required, since UART of both Daisy Seed and ESP32 is based on 3.3V. If you feel that you need more electrical protection, attach the 10 Ohm resistors on [ESP32 Shield](https://github.com/Guitar-Sound-Processing/GSP/blob/main/hardware/ESP32_shield.md) serial lines.

Therefore the IO Board has the following connectors:
- A JST PH 2 pin header for guitar input connection to the GSP [Driver Board](https://github.com/Guitar-Sound-Processing/GSP/blob/main/hardware/DriverBoard.md).
- A JST PH 2 pin header for guitar output connection from the GSP [Driver Board](https://github.com/Guitar-Sound-Processing/GSP/blob/main/hardware/DriverBoard.md).
- Two JST PH 4 pin headers for magnetic isolator module connection with [ESP32 Shield](https://github.com/Guitar-Sound-Processing/GSP/blob/main/hardware/ESP32_shield.md).
- Two JST PH 4 pin headers nRF24L01 module connection to [ESP32 Shield](https://github.com/Guitar-Sound-Processing/GSP/blob/main/hardware/ESP32_shield.md).

Expression Pedals comes in two polarities, depending the manufacturer. Moog, Roland, M-audio and Hammond, among others, adopt the RTS (Ring, Tip, Sleeve) configuration, and Yamaha, Korg and Behringer use the TRS (Tip, Ring and Sleeve). The order of RTS or TRS corresponds to the Wiper-CW-CCW of the potentiometer. So the Wiper can be attached to the Ring or the Tip, depending on the pedal. On the other hand, the power line (in this case, the 3.3V line can be connected to the Tip or the Ring. Sleeve always get the Ground line. The IO Board can be configured to support both arquitectures with jumpers in the R-P-T and T-VC-R headers. When R-P and T-VC are jumped (RTS), Wiper goes to Ring and 3V3 goes to Tip. If P-T and VC-R are jumped (TRS) then Wiper will be attached to Tip and 3V3 to Ring. 

The Input and Output Board was designed with Autodesk Eagle, but new versions will be based on Kicad. The Eagle files for schematics and boards are available [here](https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/IO_board.zip).

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/IO_board.png" width="343" height="526" alt="Input and Output Board"></p>

Do not attempt arrangement other than these two, otherwise you expression pedal will not work. Also note that the TRS configuration can cause an electrical short circuit if a mono plug was inserted in the jack.

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/IO_sheet.png" width="772" height="533" alt="Input and Output Board schematics"></p>
