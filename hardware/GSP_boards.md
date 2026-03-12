# GSP Boards

Current GSP version comprises four small pcb boards:

- Daisy Seed Shield
- Driver Board
- ESP32 Shield
- Input and Output Board

All boards have the same footprint, of 50 x 90 mm. They can be easily stacked, although they don't have stacking headers; only JST PH 2.0 Plug 2.0mm pitch headers to provide electrical interface between boards.

Daisy Seed Shield (DSS) provides a double header to stack the Daisy Seed board, besides a small DC/DC converter for 5V output to DS and ESP32 boards, together with JST headers for input and output audio signal.

The Audio Driver Board (ADB) amplifies the analogue input audio signal and compress it before delivering the signal to DSS. This board also filters and expand the analogue DSS output audio from DSS.

ESP32 Shield (ESPS) has a double header for ESP32 Wrover Lilygo TTGO board with SD Card support. It's main task is to provide high level user interface with DSS together with Expression Pedal interface. The board also provides a socket to a nRF24L01 board connection and JST headers to up to four Expression Pedals.

Finally the Input and Output Board (IOB) has 1/4 inch input and output guitar jacks, and one stereo jack for Expression Pedal.

