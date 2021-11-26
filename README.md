# Nugget Doom

Nugget Doom (formerly known as IRamm, originally a [Crispy Doom](https://www.chocolate-doom.org/wiki/index.php/Crispy_Doom) fork) is a fork of [Woof!](https://github.com/fabiangreffrath/woof), intended to implement features I, Alaux, would like to have. This includes some Crispy Doom features.

### DISCLAIMER
Although the new code has been written with the intention of not breaking demo compatibility, it has not been properly tested yet.
**RECORD DEMOS AT YOUR OWN RISK!**

## Features

- From Crispy Doom (inspired or ported code):
  - **Minimalistic HUD** (partially implemented);
  - **Resurrect from savegame** (Run key + Use key);
  - _**'RESURRECT'**_ cheat adapted from Crispy's resurrection with IDDQD (**'IDRES'** as an alternative combination);
  - _**'NOTARGET'**_ cheat;
  - _**Things move over/under things**_ setting;
  - **Jumping** (default key: <kbd>Alt</kbd>, must be enabled first);
  - _**Quick "Quit Game"**_ setting, to skip the confirmation prompt;
  - _**Disable palette tint in menus**_ setting;
  - _**Use Armor Type Color**_ setting, for armor count coloring based on armor type;
  - **Extended IDCLEV functionality** (IDCLEV00 restarts current map, IDCLEV beyond 32 in Doom 2 warps to the corresponding map if present);
  - **Mid-air control** while on noclipping mode;
  - **Interactive character cast** (Turn keys to rotate enemy, Run key to gib, Strafe keys to skip).
- _**Disable Background**_ setting, to disable the background on setup screens and dynamic Help screen
- _**Damage Tint Cap**_ and _**Bonus Tint Cap**_, to attenuate or disable the red and yellow screen tint
- _**Extra Gibbing**_ setting, to force Berserk Fist, Chainsaw and SSG gibbing
- **Crouching/ducking** (default key: <kbd>C</kbd>, must be enabled first)
- _**One-Key Quick Save/Load**_ setting, to skip the confirmation prompt
- _**Disable Horizontal Autoaim**_ setting
- _**View Height**_ setting, which allows to enter a custom height value between 8 and 56 for the player's POV (default is 41, the original)
- New **cheats**:
  - _**'FULLCLIP'**_ for infinite ammo;
  - _**'VALIANT'**_ for fast weapons;
    - _**'BOBBERS'**_ serves as a shortcut to toggle the two cheats mentioned above, plus IDFA;
  - _**'GIBBERS'**_ to force gibbing on dying enemies, independently of damage dealt;
  - _**'BUDDHA'**_ to prevent the player's health from going below 1%, despite still taking damage.
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
  - **Z** is increased in the event of at least one bug fix, or merging of _Woof!_ updates, even if the changes to the latter are considered major or minor;

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
 cmake -DCMAKE_BUILD_TYPE=Debug ..
 make
```

If you want a release build, use `Release` for the build type instead of `Debug`.  Also, if you happen to have [Ninja](https://ninja-build.org/) installed, you can add `-G Ninja` to the `cmake` invocation for faster build times.

After successful compilation the resulting binary can be found in the `Source/` directory.

### Windows

Visual Studio 2019 comes with built-in support for CMake by opening the source tree as a folder.  Otherwise, you should probably use the GUI tool included with CMake to set up the project and generate build files for your tool or IDE of choice.

It's worth noting that you do not need to download any dependencies.  The build system will automatically download them for you.

### Cross-compiling

You can cross-compile from Linux to Windows.  First, make sure you have a reasonably recent version of mingw-w64 installed.  From there, cross-compiling should be as easy as:

```
 cd nugget
 mkdir build; cd build
 cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=../CrossToWin64.cmake ..
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
Copyright: © 1993-1996 Id Software, Inc.;
 © 1993-2008 Raven Software;
 © 1999 by id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman;
 © 2004 James Haley;
 © 2005-2014 Simon Howard;
 © 2020-2021 Fabian Greffrath;
 © 2020 Alex Mayfield;
 © 2020-2021 Roman Fomin.
License: [GPL-2.0+](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)

Files: `opl/*`
Copyright: © 2002-2003 Marcel Telka;
 © 2005-2014 Simon Howard;
 © 2013-2018 Alexey Khokholov (Nuke.YKT).
License: [GPL-2.0+](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)

Files: `Source/beta.h`
Copyright: © 2001-2019 Contributors to the Freedoom project.
License: [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)

Files: `Source/dogs.h`
Copyright: © 2017 Nash Muhandes;
 © apolloaiello;
 © TobiasKosmos.
License: [CC-BY-3.0](https://creativecommons.org/licenses/by/3.0/) and [CC0-1.0](https://creativecommons.org/publicdomain/zero/1.0/)

Files: `data/woof.ico,
 data/woof.png,
 data/woof8.ico`
Copyright: © 2021 Korp.
License: [CC BY-NC-SA 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/)

Files: `Source/icon.c` (to be replaced)
Copyright: © 2020 Julia Nechaevskaya.
License: [CC-BY-3.0](https://creativecommons.org/licenses/by/3.0/)

Files: `cmake/FindSDL2.cmake,
 cmake/FindSDL2_image.cmake,
 cmake/FindSDL2_mixer.cmake,
 cmake/FindSDL2_net.cmake`
Copyright: © 2018 Alex Mayfield.
License: [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)
