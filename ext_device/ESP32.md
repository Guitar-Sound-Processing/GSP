# External Device

Although any microprocessor module can be used to bypass the effect commands to Daisy Seed, a ESP32 WROVER module was chosen by its capability to provide wifi 
and bluetooth support. Therefore GSP can be potentially configured by any smartphone or computer connected to ESP32. There are several ESP32 WROVER modules available on market. Among them the LILYGO or TTGO was selected, since it offers also SMD support for data storage.

<p align="center">
  **LILYGO ESP32 WROVER Module**
  <img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/esp32_ttgo.png" width="588" height="459" alt="ESP32 LILYGO">
</p>

## ESP32 tasks

ESP32 WROVER has the following tasks:

- To receive, interpret and execute the [Expression Pedal](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/ExprPedal.md) commands (P, A, S and C) coming from DS through UART_1 Serial line.
- To receive the [Effect Commands](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/Commands.md)) from end users, and route then to Daisy Seed through UART_1 Serial line.
- To receive reply from Daisy Seed and to transmit them to GSP application in Android phones. 

From low to high level commands, they are:

- [Effect Commands](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/Commands.md) (interpreted by Daisy Seed)
- Presets (interpreted by GSP Android)
- Chain Profiles (interpreted by by GSP Android)
- Songs (interpreted by by GSP Android)
- Playlists (interpreted by by GSP Android)

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/hl_com.png" width="512" height="185" alt="High and Low Level commands"></p>

