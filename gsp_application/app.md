
# GSP Application Software

## Description 

The Guitar Sound Processing can be remotely operated using an Android application software freely available. It runs on Android 10 and above, but it is expected that it can also work on older versions. The software was generated with help from Gemini's AI, to provide basic kotlin coding. The software is still in progress but version 1.0 is already working with minor bugs. Some new improvements may be soon available, mainly in the Expression Pedal algorithm. Currently the app software is available for download only at github's GSP page, although in future versions it can be also available in Google Play Store. Wifi support for both Windows and Android is still being considered as future improvements.

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/gsp_arquit.png" alt="GSP Architecture"></p>

The application software is organized in 4 levels (see figure below):
- Playlist Editor and on stage performance (Play)
- Song Editor (Song)
- Chain (Rig) Editor (Chain)
- Effect and Preset Editor (Preset)

Each level is selected by buttons on the top bar menu, as shown in picture. Just below these buttons are the Bluetooth state "led", that indicates that the GSP application succesfully stablished contact with Daisy Seed, if green, or red, if has not. Although the application still can be used without connection with Bluetooth, the effect parameters shown in the Preset Editor are random. At right side of the screen, a debug switch can be enabled to print on screen the commands sent to and received from Daisy Seed.

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/topbar.png" width="270" height="109" alt="GSP Main Menu"></p>

## Effect and Preset Editor

The Effect and Preset Editor can modify any effect of GSP, including the Level Detector. As can be seen in picture below, the parameters can be adjusted through slider rulers, buttons and dropdown lists. The right and left arrows change the selected effect in alphabetic order, in spite of the effects ordering in current Chain. Note that the bypass switch is off (no effect on output) by default. In order to have the effect on output the bypass switch must be turned on. Of course this is a bit confusing and probably this will be changed in future GSP versions.

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/presets.png" width="270" alt="Preset Editor"></p>

