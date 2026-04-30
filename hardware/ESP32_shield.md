# ESP32 Shield

The current version of GSP uses a ESP32 Wrover as External Device. To integrate the Wrover board to GSP, a dedicated pcb shield was designed to provide standard interfaces to Expression Pedals and Serial UART lines to Daisy Seed. Moreover, some jumpers are provided to interface the ESP32 Shield to a nRF24L01 wireless module for remote Expression Pedals. The Shield board does not contain any direct wiring to peripherics. All connections shall be manually provided by jumpers. This allow flexibility to change the Wrover (or any other compatible ESP32 board) pins to the desired data and power lines. The main characteristics of the board are:

- Two pin headers for DS with two additional headers already connected for air wiring.
- A socket for step down 2A DC/DC converter module with small footprint, based on LM2586. The output must be adjusted to 5V.
<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/dcdc_module.png" width="216" height="162" alt="DCDC Module"></p>

- An operation amplifier LM358 to generate 9V/2 (4.5V) to biase input audio signal.
- - 3 JST PH headers for any GPIO peripheral with pin headers for air wiring connection. It can be used to Expression Pedal interface, but the current GSP version of DS sofware does not have support to them.
- A JST PH headers for 9V-Gnd power input, 9V-Gnd output and 9V-4.5V-Gnd Power output to Audio Driver Board.
- 3 JST PH headers with additional pin headers for air wiring connection with DS for UART Serial lines.
- 2 JST PH headers with additional pin headers for air wiring connection with DS for ADC and DAC signals coming from and going to the Audio Driver Board.
- A JST PH header to provide analogue Vref and AGND from DS to the ADB connected by jumper.
- 5 jumpers to direct connect:
    - 5V to DS Vin
    - Grounding to DS DGND
    - Vref to JST header
    - AGND to JST header
    - DGND to AGND

In normal situation all these jumpers shall be wired.
<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/DS_board_1_2.jpg" width="303" height="533" alt="DCDC Board1"></p>

The PCB was designed with Autodesk Eagle, but next version will be based on Kicad. The Eagle files for schematics and boards are available [here](https://github.com/Guitar-Sound-Processing/GSP/blob/fb6eaf6ac311b30f773d6f07a96264511f1aebff/resources/gsp_ds_v3.zip).

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/DS_board_sch.png" width="772" height="532" alt="DCDC Board1"></p>


