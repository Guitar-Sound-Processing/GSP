# External Device

Although any microprocessor module may be used to translate high level to low level effect commands in GSP, a ESP32 WROVER module was chosen by its capability to provide wifi and bluetooth support. Therefore GSP can be potentially configured by any smartphone or computer connected to ESP32.

## Current ESP32 tasks

ESP32 WROVER has the following tasks:

- To receive, interpret and execute the [Expression Pedal](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/ExprPedal.md) commands (P, A, S and C) coming from DS through UART_1 Serial line.
- To receive, interpret and translate the high level commands from Playlist, Chain Profiles and Songs to low level commands (Setup and Effect Commands) from end users, and send then to Daisy Seed through UART_1 Serial line.

