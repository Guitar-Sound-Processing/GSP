# Daisy Seed Shield

This board was designed so as to provide interface between the Daisy Seed module to several periphericals like potentiometer for Expression Pedals, Serial UART lines and Power Supply (9V, 5V and 4.5V). The design does not make any wiring to DS, except the power lines and grounding through jumpers. All the wirings shall manually be made by air jumpers. This procedure assures flexibility to choose any port or GPIO to be used anywhere it is required, and to change the port in case of localized malfunction. The board has:

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

The PCB was designed with Autodesk Eagle, but next version will be based on Kicad. The Eagle files for schematics and boards are available here.

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/DS_board_sch.png" width="772" height="532" alt="DCDC Board1"></p>

