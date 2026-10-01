
# GSP Application Software

## Description 

The Guitar Sound Processing can be remotely operated using an Android application software freely available. It runs on Android 10 and above, but it is expected that it can also work on older versions. The software was generated with help from Gemini's AI, to provide basic kotlin coding. The software is still in progress, but version 1.0 is already working with minor bugs. Some new improvements may be soon available, mainly in the Expression Pedal algorithm. Currently the compiled apk is too large to be stored in github's page. If you need the apk file, please send an [email] (guitar.sound.processing@gmail.com) asking for it. Probably this app will be also available in the Google Play Store soon. Wifi support for both Windows and Android is still being considered as future improvements.

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/gsp_arquit.png" alt="GSP Architecture"></p>

The application software is organized in 4 levels (see figure below):
- Playlist Editor and on-stage performance (Play)
- Song Editor (Song)
- Chain (Rig) Editor (Chain)
- Effect and Preset Editor (Preset)

Each level is selected by buttons on the top bar menu, as shown in the picture. Just below these buttons is the Bluetooth state "LED", that indicates that the GSP application successfully established contact with Daisy Seed, if green, or not, if red. Although the application still can be used without a connection with Bluetooth, the effect parameters shown in the Preset Editor are random. At right side of the screen, a debug switch can be enabled to print on screen the commands sent to and received from Daisy Seed.

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/topbar.png" width="270" height="109" alt="GSP Main Menu"></p>

## Effect and Preset Editor

The Effect and Preset Editor can modify any effect of GSP, including the Level Detector. As can be seen in the picture below, the parameters can be adjusted through slider rulers, buttons, and dropdown lists. The right and left arrows change the selected Effect in alphabetic order, in spite of the Effects ordering in the current Chain. Note that the bypass switch is off (no effect on output) by default. In order to have the effect on output, the bypass switch must be turned on. Of course this is a bit confusing and probably will be changed in future GSP versions.

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/presets.png" width="270" alt="Preset Editor"></p>

All effects have a standard button menu, to create (Add), to Save, to Load and to delete (Del) a preset in permanent memory, stored in the phone or tablet's internal flash. These presets are individualized, i. e., they are stored separately for each effect.

## Chain Editor

GSP can store different chains with effects placed in any order. As shown in picture, the Chain Editor has several buttons. The gray central box shows one of the effects in chain, from input at leftmost to output at rightmost. The arrow buttons change the position to left or right and show the Effect at that position in central box. The New button creates a new chain with all the effects in their default position. Please note that the New button leaves the current Chain unsaved when pressed in favor of the new complete one. The Clear button removes all the Effects in current Chain leaving only the Level Detector as the first and unmovable effect. The left and right Plus (+) dropdown menus insert a selected effect at left or right position in Chain. If the selected effect is already in Chain, then it will be moved to the desired position. The Minus (-) button removes the central box effect from current Chain. By clicking in the central box, the parameters for this effect can be edited in the Preset Editor. 

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/chains.png" width="270" alt="Preset Editor"></p>

As in the Preset Editor, the buttons Add, Save, Load and Delete (Del) buttons respectively create, store in permanent memory, load a previously saved, or delete the current Chain. The Add button asks for the Chain name before saving the new chain. Just above these buttons, the GSP shows the current Chain name.

### Important note

Although GSP can store Presets for any effect, the Chain sequence to be sent to Daisy Seed does not use any stored Preset. That does not mean that the stored presets are useless. For instance, if a hard distortion is stored in the preset HardOverdrive, and this preset has to be included in chain MyChainA, then one must select the Distortion effect on the central box on the Chain Editor, and select (click over) the central box to go to the Preset Editor. After that, load the HardOverdrive preset, then return to Chain Editor and save it. The Chain Editor always saves the current configuration of any effect present in Chain.

## Song Editor

The Song Editor makes the required link between a given song and a previously created Chain in Chain Editor. It has only one button, to add a new song (the song's name or title, indeed). This button opens a new window and asks to input the song's title and to select the associated Chain from a dropdown menu. As can be seen in picture, there are buttons to change the title (pencil icon) and to remove (trash can icon) any stored song. When a given song is selected, its color changes, and GSP sends to Daisy Seed all the commands to change its configuration to the Chain linked to the song.

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/songs.png" width="270" alt="Preset Editor"></p>

## Playlist Editor

It is in the Playlist Editor that GSP shows its power. The Playlist Editor has two functions: to create and edit new playlists, and to select a given song to be "played", i. e., to change Daisy Seed configuration to the desired effects. The Plus (+) button aside the dropdown of playlists allows creating a new playlist, which is a collection of songs to be played on a performance show. Of course the trash can icon removes the selected playlist from stored playlist data. The songs to be performed are then selected by the dropdown menu "+ Add Song to Playlist". Daisy's configuration is changed by selecting one of the selected songs, or sequently by clicking in the "Next Song ->" button, as can be seen in picture. 

<p align="center"><img src="https://raw.githubusercontent.com/Guitar-Sound-Processing/GSP/master/resources/playlists.png" width="270" alt="Preset Editor"></p>

