# GSP Main Loop

GSP main code is responsible to provide all interfaces to libDaisy, as well as to call the process methods for all the audio effects in chain ([Effect Sofware Directives](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/SWDirectives.md)). The main loop provides also the Effect Command decoding and execution, besides Expression Pedal assignments. It runs exclusively in Daisy Seed, and does not store any configuration in flash memory, in order to avoid wasting time.

Main loop interfaces to the External Device (ED) by UART Serial or to any computer by virtual COM port through USB. Presentely the UART Serial shares both Effect Commands and Expression Pedal data coming from ED in the same serial line. They differentiate by a preceeding 
opening brace '''{''' (Effect Command) and closing brace '}' for Expression Pedal.
## Standard headers
