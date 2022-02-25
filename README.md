# Nugget Doom

Nugget Doom (formerly known as IRamm, originally a [Crispy Doom](https://www.chocolate-doom.org/wiki/index.php/Crispy_Doom) fork) is a fork of [Woof!](https://github.com/fabiangreffrath/woof), intended to implement features I, Alaux, would like to have. This includes some Crispy Doom features.

### DISCLAIMER
Although the new code has been written with the intention of not breaking demo compatibility, it has not been properly tested yet.
**RECORD DEMOS AT YOUR OWN RISK!**

## Features

- From Crispy Doom (inspired or ported code):
  - **Minimalistic HUD** (without translucent variants);
  - **Resurrect from savegame** (Run key + Use key);
  - _**'RESURRECT'**_ cheat adapted from Crispy's resurrection with IDDQD (**'IDRES'** as an alternative combination);
  - _**'NOTARGET'**_ cheat;
  - _**Things move over/under things**_ setting;
  - **Static crosshair**, without highlight on target;
  - **Jumping** (default key: <kbd>Alt</kbd>, must be enabled first);
  - _**Quick "Quit Game"**_ setting, to skip the confirmation prompt;
  - _**Disable palette tint in menus**_ setting;
  - _**Use Armor Type Color**_ setting, for armor count coloring based on armor type;
  - **IDCLEV00 restarts current map**;
  - **Mid-air control** while on noclipping mode;
  - **Interactive character cast** (Turn keys to rotate enemy, Run key to gib, Strafe keys to skip);
  - Reenabled _**'NOMOMENTUM'**_ cheat.
- _**Disable Background**_ setting, to disable the background on setup screens and dynamic Help screen
- _**Damage Tint Cap**_ and _**Bonus Tint Cap**_, to attenuate or disable the red and yellow screen tint
- _**Extra Gibbing**_ setting, to force Berserk Fist, Chainsaw and SSG gibbing
- _**Bobbing Percentage**_ setting
- **Crouching/ducking** (default key: <kbd>C</kbd>, must be enabled first)
- _**One-Key Quick Save/Load**_ setting, to skip the confirmation prompt
- _**Disable Horizontal Autoaim**_ setting
- _**View Height**_ setting, which allows to enter a custom height value between 8 and 56 for the player's POV (default is 41, the original)
- New **cheats**:
  - _**'FULLCLIP'**_ for infinite ammo;
  - _**'VALIANT'**_ for fast weapons;
    - _**'BOBBERS'**_ serves as a shortcut to toggle the two cheats mentioned above, plus IDFA;
  - _**'GIBBERS'**_ to force gibbing on dying enemies, independently of damage dealt;
  - _**'IDFLY'**_ to fly;
  - _**'BUDDHA'**_ to prevent the player's health from going below 1%, despite still taking damage.
- **Sound Links check for a dedicated lump**, therefore allowing usage of a custom sound effect
- New **"Doom compatibility"** settings (some adapted from Crispy Doom fixes):
  - Blazing doors reopen with wrong sound;
  - Manually reactivated moving doors are silent;
  - Corrected switch sound source;
  - Chaingun makes two sounds with one bullet;
  - Fix Lost Soul colliding with items;
  - Lost Soul forgets target upon impact;
  - Fuzzy things bleed fuzzy blood;
  - Non-bleeders don't bleed when crushed;
  - Prevent Pain state with 0 damage attacks;
  - Bruiser attack (A_BruisAttack) doesn't face target;
  - Chaingunner uses pistol/chaingun sound;
  - Arch-Vile fire plays flame start sound;
  - Dead players can still play "oof" sound;
  - Fix lopsided Icon of Sin explosions;
  - Key pickup resets palette;
  - Fix IDCHOPPERS invulnerability.

## Versioning
Nugget Doom follows a fairly simple **X.Y.Z** versioning system:
  - **X** is increased in the event of at least one major implementation, such as a new spec like _UMAPINFO_;
  - **Y** is increased in the event of at least one minor implementation, such as a new cheat;
  - **Z** is increased in the event of at least one bug fix, text/code reformatting, or merging of _Woof!_ updates, even if the changes to the latter are considered minor or major;

Incrementing any of the first values will reset the latter (i.e. a major change to 1.1.2 would shift it to 2.0.0).

## Building

As a Woof! fork, its build instructions should also apply here:

The source code is available at GitHub: https://github.com/MrAlaux/Nugget-Doom

### Linux

You will need to install the SDL2, SDL2_image, SDL2_mixer and SDL2_net libraries.  Usually your distribution has these libraries in its repositories, and if your distribution has "dev" versions of those libraries, those are the ones you'll need.

Once installed, compilation should be as simple as:

```
 cd nugget
 mkdir build; cd build
 cmake ..
 make
```

After successful compilation the resulting binary can be found in the `Source/` directory.

### Windows

Visual Studio 2019 comes with built-in support for CMake by opening the source tree as a folder.  Otherwise, you should probably use the GUI tool included with CMake to set up the project and generate build files for your tool or IDE of choice.

It's worth noting that you do not need to download any dependencies.  The build system will automatically download them for you.

### Cross-compiling

You may want to cross-compile from Linux to Windows.  First, make sure you have a reasonably recent version of the [Mingw-w64](https://www.mingw-w64.org/) package installed.  From there, cross-compiling should be as easy as:

```
 cd nugget
 mkdir build; cd build
 cmake -DCMAKE_TOOLCHAIN_FILE=../CrossToWin64.cmake ..
 make
```

Much like a native Windows build, you do not need to download any dependencies.

## Contact

The homepage for Nugget Doom is https://github.com/MrAlaux/Nugget-Doom

Please report any bugs, glitches or crashes that you encounter to the GitHub [Issue Tracker](https://github.com/MrAlaux/Nugget-Doom/issues).

## Acknowledgement

Help was provided by _kraflab_ (responsible for [dsda-doom](https://github.com/kraflab/dsda-doom)), _melak47_ and _Fabian Greffrath_ himself.

## Legalese

Files: `*`
Copyright:
 © 1993-1996 Id Software, Inc.;
 © 1993-2008 Raven Software;
 © 1999 by id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman;
 © 2004 James Haley;
 © 2005-2018 Simon Howard;
 © 2006 Ben Ryves;
 © 2017 Christoph Oelckers;
 © 2019 Fernando Carmona Varo;
 © 2019 Jonathan Dowland;
 © 2020-2021 Fabian Greffrath;
 © 2020 Alex Mayfield;
 © 2020-2021 Roman Fomin.
License: [GPL-2.0+](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)

Files: `Source/beta.h`
Copyright: © 2001-2019 Contributors to the Freedoom project.
License: [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)

Files: `Source/dogs.h`
Copyright:
 © 2017 Nash Muhandes;
 © apolloaiello;
 © TobiasKosmos.
License: [CC-BY-3.0](https://creativecommons.org/licenses/by/3.0/) and [CC0-1.0](https://creativecommons.org/publicdomain/zero/1.0/)

Files: `Source/u_scanner.*`
Copyright:
 © 2010 Braden "Blzut3" Obrzut;
 © 2019 Fernando Carmona Varo.
License: [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)

Files: `cmake/FindFluidSynth.cmake`
Copyright: © 2018 Christophe Giboudeaux.
License: [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)

Files: `cmake/FindSDL2.cmake, cmake/FindSDL2_mixer.cmake, cmake/FindSDL2_net.cmake`
Copyright: © 2018 Alex Mayfield.
License: [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)

Files: `data/woof.ico,
 data/woof.png,
 data/woof8.ico,
 Source/icon.c`
Copyright: © 2022 Korp.
License: [CC BY-NC-SA 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/)

Files: `miniz/*`
Copyright:
 © 2010-2014 Rich Geldreich and Tenacious Software LLC;
 © 2013-2014 RAD Game Tools and Valve Software.
License: [MIT](https://opensource.org/licenses/MIT)

Files: `opl/*`
Copyright:
 © 2005-2014 Simon Howard;
 © 2013-2018 Alexey Khokholov (Nuke.YKT).
License: [GPL-2.0+](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)

Files: `soundfonts/TimGM6mb.sf2`
Copyright:
 © 2004 Tim Brechbill;
 © 2010 David Bolton.
License: [GPL-2.0](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)

Files: `textscreen/*`
Copyright:
 © 1993-1996 Id Software, Inc.;
 © 1999 Thomas A. Fine;
 © 2002-2004 The DOSBox Team;
 © 2005-2017 Simon Howard.
License: [GPL-2.0+](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)

Files: `win32/stdint.h`
Copyright: © 2006-2008 Alexander Chemeris.
License: [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)

Files: `win32/win_opendir.*`
License: Public Domain
