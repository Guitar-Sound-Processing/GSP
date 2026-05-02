# External Device

Although any microprocessor module can be used to bypass the effect commands to Daisy Seed, a ESP32 WROVER module was chosen by its capability to provide wifi and bluetooth support. Therefore GSP can be potentially configured by any smartphone or computer connected to ESP32.

## ESP32 tasks

ESP32 WROVER has the following tasks:

- To receive, interpret and execute the [Expression Pedal](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/ExprPedal.md) commands (P, A, S and C) coming from DS through UART_1 Serial line.
- To receive the [Effect Commands](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/Commands.md)) from end users, and route then to Daisy Seed through UART_1 Serial line.
- To receive reply from Daisy Seed and to transmit them to GSP application in Android phones. 

From low to high level commands, they are:

- [Effect Commands](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/Commands.md) (interpreted by Daisy Seed)
- [Effect Presets](https://github.com/Guitar-Sound-Processing/GSP/blob/main/ext_device/Presets.md) (interpreted by GSP Android)
- [Chain Profiles](https://github.com/Guitar-Sound-Processing/GSP/blob/main/ext_device/ChainProfiles.md) (interpreted by by GSP Android)
- [Songs](https://github.com/Guitar-Sound-Processing/GSP/blob/main/ext_device/Songs.md) (interpreted by by GSP Android)
- [Playlists](https://github.com/Guitar-Sound-Processing/GSP/blob/main/ext_device/Playlists.md) (interpreted by by GSP Android)

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/hl_com.png" width="512" height="185" alt="High and Low Level commands"></p>

