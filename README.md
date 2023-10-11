# PS1 Sokoban

Sokoban game implementation for PS1 using MIPS toolchain + Nugget + Psy-Q Dev Kit.


## Binary CD BIN/CUE ##

* You can download the latest binary CD BIN/CUE here: https://github.com/danx2016/ps1_sokoban/releases/tag/v0.0.4


## How to Build ##

* Install Visual Studio Code + PSX.DEV extension (https://www.youtube.com/watch?v=KbAv-Ao7lzU)
* Clone this repository using:
```
git clone --recurse-submodules https://github.com/danx2016/ps1_sokoban.git
```
<b>note:</b> psyq-iwyu and nugget folders inside third_party directory are git submodules (independent projects referenced in another locations), so you need to use '--recurse-submodules' flag to include them 
* Open the folder in vscode, then 'Ctrl + Shift + P -> PSX.Dev: Show panel > WELCOME > Restore Psy-Q'. This will restore the Psy-Q SDK files inside 'third_party/psyq' folder
* Then, to build ps-exe, just type inside project directory:
```
make
```
* Alternatively, you can also build ps-exe within vscode itself using "Ctrl + Shift + B -> Build Release".
* Finally, to generate the CD BIN/CUE:
```
mkpsxiso.exe -y isoconfig.xml  
```


## Special Thanks To ##

* @Nicolas Noble for modplayer, sjis info and lots of other explanations...
* @spicyjpeg for pal/ntsc and lots of other explanations...
* @Stenzek for memory card sjis encoding info and lots of other explanations...
* @Kamil & @rama for kindly trying to run this game on a real console...
* and all people from PSX.Dev
