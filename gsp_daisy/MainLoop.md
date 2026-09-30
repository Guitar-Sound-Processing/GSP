# GSP Main Loop

GSP main code is responsible to provide all interfaces to libDaisy, as well as to call the process methods for all the audio effects in chain ([Effect Sofware Directives](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/SWDirectives.md)). The main loop provides also the Effect Command decoding and execution, besides Expression Pedal assignments. It runs exclusively in Daisy Seed, and does not store any configuration in flash memory, in order to avoid wasting time. 

Table below shows the processing time for all effects in default configuration, in percentage of the available duty cycle, provided by the ```out``` command in [Interfaces Commands](https://github.com/Guitar-Sound-Processing/GSP/blob/main/gsp_daisy/Interfaces.md). First row shows the idle time, which is composed by the LVD (Level Detector) duty cycle, which is allways active, together the necessary time to libDaisy to provide audio samples, which takes around 5.2% of the processing time. The following rows of the second column are the measured duty cycle of isolated effects, added to the idle time. Third column presents the difference between the isolated effect and the idle times, i.e., the duty cycle of each effect alone. Last row presents the total time of all effects together in default configuration as measured (second colunm) and computed by adding all the rows in third column. The available processing time to include new effects are still large, around 70%.

<div align="center">
  
| Effect | efc + LVD (%) | efc (%) |
| --- | :---: | :---: |
| LVD | 5.14 | 5.14 |
| CHS | 6.40 | 1.26 |
| CMP | 6.56 | 1.42 |
| DFB | 5.86 | 0.72 |
| DFF | 6.53 | 1.39 |
| DTN | 6.12 | 0.98 |
| EFB | 5.93 | 0.79 |
| EFF  | 6.85 | 1.71 |
| EQZ | 6.39 | 1.25 |
| LIM | 6.35 | 1.21 |
| NGT | 6.33 | 1.19 |
| OCT  | 6.16 | 1.02 |
| OVD  | 7.80 | 2.66 |
| PHR  | 7.94 | 2.80 |
| SFT | 5.99 | 0.85 |
| RVB  | 7.65 | 2.51 |
| TML | 5.95 | 0.81 |
| VBT | 6.49 | 1.35 |
| VOL | 6.03 | 0.89 |
| WAH | 6.72 | 1.58 |
| Total  | 28.70 | 31.53 |

<\div>

Main loop interfaces to the External Device (ED) by UART Serial or to any computer by virtual COM port through USB. Presentely the UART Serial shares both Effect Commands and Expression Pedal data coming from ED in the same serial line. They differentiate by a preceeding 
opening brace (```{```) for Effect Command and a closing brace (```}```) for Expression Pedal. They shall utilize two serial lines in future GSP versions.

Since GSP is a large program, it can't fit in the internal flash memory of the STM32H750IB processor. So it is necessary to store the program in the SDRAM external memory of Daisy Seed. The provided Makefile is already configured to do this, by using Visual Studio Code.
However, it is also required to change the normal bootloader of DS with the Daisy Bootloader, explained in the [Daisy Seed](https://daisy.audio/tutorials/_a7_Getting-Started-Daisy-Bootloader/) page.

