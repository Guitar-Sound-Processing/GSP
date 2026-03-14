# External Device

Although any microprocessor module can be used to translate high level to low level effect commands in GSP, a ESP32 WROVER module was chosen by its capability to provide wifi and bluetooth support. Therefore GSP can be potentially configured by any smartphone or computer connected to ESP32.

## ESP32 tasks

ESP32 WROVER has the following tasks:

- To receive, interpret and execute the [Expression Pedal](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/ExprPedal.md) commands (P, A, S and C) coming from DS through UART_1 Serial line.
- To receive, interpret and translate the high level commands from Playlist, Chain Profiles and Songs to low level commands (Setup and [Effect Commands](https://github.com/Guitar-Sound-Processing/GSP/blob/905e93f35ec7b43535aa493525fcdbdda664c829/gsp_daisy/Commands.md)) from end users, and send then to Daisy Seed through UART_1 Serial line.
- To retrieve the high level commands from stored files and corresponding link to the low level configuration commands.

From low to high level commands, they are:

- [Effect Commands](https://github.com/Guitar-Sound-Processing/GSP/blob/905e93f35ec7b43535aa493525fcdbdda664c829/gsp_daisy/Commands.md) (interpreted by Daisy Seed)
- [Effect Setups](https://github.com/Guitar-Sound-Processing/GSP/blob/a6110c35568fb845425bcf964dfb7564135c35df/ext_device/Setup.md) (interpreted by ESP32)
- [Chain Profiles](https://github.com/Guitar-Sound-Processing/GSP/blob/07dee8d4ad24ca601d4f08727b1f1b7b17d918a4/ext_device/ChainProfiles.md) (interpreted by ESP32)
- [Songs](https://github.com/Guitar-Sound-Processing/GSP/blob/d68c116f49ef13aa1b3948ecab4c9a651261ede5/ext_device/Songs.md) (TBD)
- [Playlists](https://github.com/Guitar-Sound-Processing/GSP/blob/d68c116f49ef13aa1b3948ecab4c9a651261ede5/ext_device/Playlists.md) (TBD)

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/hl_com..png" width="512" height="185" alt="High and Low Level commands"></p>

