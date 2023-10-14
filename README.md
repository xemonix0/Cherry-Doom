# Cherry Doom
[![Cherry Doom Icon](https://raw.githubusercontent.com/xemonix0/Cherry-Doom/master/data/cherry-doom.png)](https://github.com/xemonix0/Cherry-Doom)

[![Top Language](https://img.shields.io/github/languages/top/xemonix0/Cherry-Doom.svg)](https://github.com/xemonix0/Cherry-Doom)
[![Code Size](https://img.shields.io/github/languages/code-size/xemonix0/Cherry-Doom.svg)](https://github.com/xemonix0/Cherry-Doom)
[![License](https://img.shields.io/github/license/xemonix0/Cherry-Doom.svg?logo=gnu)](https://github.com/xemonix0/Cherry-Doom/blob/master/COPYING)
[![Release](https://img.shields.io/github/release/xemonix0/Cherry-Doom.svg)](https://github.com/xemonix0/Cherry-Doom/releases/latest)
[![Release Date](https://img.shields.io/github/release-date/xemonix0/Cherry-Doom.svg)](https://github.com/xemonix0/Cherry-Doom/releases/latest)
[![Commits](https://img.shields.io/github/commits-since/xemonix0/Cherry-Doom/latest.svg)](https://github.com/xemonix0/Cherry-Doom/commits/master)
[![Last Commit](https://img.shields.io/github/last-commit/xemonix0/Cherry-Doom.svg)](https://github.com/xemonix0/Cherry-Doom/commits/master)
[![Build Status](https://github.com/xemonix0/Cherry-Doom/actions/workflows/main.yml/badge.svg)](https://github.com/xemonix0/Cherry-Doom/actions/workflows/main.yml)

Cherry Doom is a fork of [Nugget Doom](https://github.com/MrAlaux/Nugget-Doom) intended to add even more features.

**Note**: this README and the rest of documentation are updated on a per-commit basis,
meaning that they may not correspond to the latest release of Cherry Doom.
If you're seeking information on the version you're using, please refer to the documentation included with it.

### DISCLAIMER
Although the new code has been written with the intention of not breaking demo compatibility, it has not been properly tested yet.
**RECORD DEMOS AT YOUR OWN RISK!**

### SAVE FILE COMPATIBILITY NOTICE
In the course of development, changes to the save file structure, such as adding, moving, or removing values, might be introduced in some commits. **The save version is not updated after every single change,** rather only after the first one leading to the next release.

This means that if you saved your game before a breaking change to save file structure was introduced, updating to the latest commit might not trigger the incompatibility warning when trying to load the save, and as a result that might lead to unexpected behavior or a crash.

Saves made in older releases of Cherry Doom or in Nugget Doom or Woof! should all remain compatible with the newest Cherry Doom release, unless it is stated otherwise in the changelog.

## Features

Note that this feature list is relative to [Nugget Doom's](https://github.com/MrAlaux/Nugget-Doom/blob/master/README.md#features); read the latter for more details.
Some features were first introduced in Cherry Doom and later merged into Nugget Doom, and so they're not listed here anymore.

Some of Cherry Doom's features come from other sources, like other source ports, mods or games. The initial implementations for some are **(partially) ported from ((p.)p.f.)** or **inspired by (i.b.)** said sources. These acknowledgements are included in the feature lists below.

### General

- **Menu background can now be enabled in all menus**.
- **Damage view shake effect**.
- **Max shake intensity customization**.
- **Motion blur**. [p.f. Doom Retro]

### Status Bar/HUD

- **More widgets on intermission screen** (attempts, health, armor, weapons).
- **Attempts widget**: shows the current session attempts and the total attempts across all sessions. The session attempts reset when the map is beaten or when the game is quit, and they are preserved in saved games.
- **More HUD color customization**: pick colors for widget names, health/armor/ammo threshold colors and plain text.
- **Movement widget**: shows the current player movement speeds.

### Automap

- **Color customization for Tag Finder from PrBoomX**.

### Miscellaneous

- **Level table**, with the following additions: [p.f. DSDA-Doom]
	- **Attempts** level table entry.

# Releases

Source code, Windows binaries (MSVC builds for Windows 7 and newer) and Linux AppImages for the latest release can be found on the [Release](https://github.com/xemonix0/Cherry-Doom/releases/latest) page.

The most recent list of changes can be found in the [Changelog](https://github.com/xemonix0/Cherry-Doom/blob/master/CHANGELOG.md).

A complete history of changes and releases can be found on the [Releases](https://github.com/xemonix0/Cherry-Doom/releases) page.

## Versioning

Cherry Doom follows exactly the same versioning system as Nugget Doom, which is fairly simple:

- **X** is increased in the event of at least one major implementation, such as a new specification like _UMAPINFO_;
- **Y** is increased in the event of at least one minor implementation, such as a new cheat;
- **Z** is increased in the event of at least one bug fix, text/code reformatting, tiny tweak or merging of _Nugget Doom_ updates, even if the changes to the latter are considered minor or major.

Incrementing any of the first values will reset the latter (i.e. a major change to 1.1.2 would shift it to 2.0.0).

# Compiling

As a Nugget Doom fork, its build instructions should also apply here:

The Cherry Doom source code is available at GitHub: <https://github.com/xemonix0/Cherry-Doom>.

It can be cloned via

```
 git clone https://github.com/xemonix0/Cherry-Doom.git
```

## Linux, and Windows with MSYS2

The following libraries need to be installed:

- [SDL2](https://github.com/libsdl-org/SDL/tree/SDL2) (>= 2.0.18)
- [SDL2_net](https://github.com/libsdl-org/SDL_net)
- [openal-soft](https://github.com/kcat/openal-soft) (>= 1.22.0 for PC Speaker emulation)
- [libsndfile](https://github.com/libsndfile/libsndfile) (>= 1.1.0 for MPEG support)
- [fluidsynth](https://github.com/FluidSynth/fluidsynth) (>= 2.2.0, optional)
- [libxmp](https://github.com/libxmp/libxmp) (optional)
 
Usually your distribution should have the corresponding packages in its repositories, and if your distribution has "dev" versions of those libraries, those are the ones you'll need.

Once installed, compilation should be as simple as:

```
 cd cherry-doom
 mkdir build; cd build
 cmake ..
 make
```

After successful compilation the resulting binary can be found in the `src/` directory.

## Windows with Visual Studio

[Visual Studio 2019](https://visualstudio.microsoft.com/) and [Visual Studio Code](https://code.visualstudio.com/) come with built-in support for CMake by opening the source tree as a folder.

Install vcpkg <https://github.com/Microsoft/vcpkg#quick-start-windows>. Integrate it into CMake or use toolchain file:
```
 cd cherry-doom
 cmake -B build -DCMAKE_TOOLCHAIN_FILE="[path to vcpkg]/scripts/buildsystems/vcpkg.cmake"
 cmake --build build
```
CMake will automatically download and build all dependencies for you.

# Contact

The homepage for Cherry Doom is <https://github.com/xemonix0/Cherry-Doom>.

Please report any bugs, glitches or crashes that you encounter to the GitHub [Issue Tracker](https://github.com/xemonix0/Cherry-Doom/issues).

# Legalese

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
 © 2020 Alex Mayfield;  
 © 2021 Ryan Krafnick;  
 © 2022-2023 ceski;  
 © 2023 liPillON;  
 © 2020-2023 Fabian Greffrath;  
 © 2020-2023 Roman Fomin;  
 © 2021-2023 Alaux;  
 © 2023 Xemonix.   
License: [GPL-2.0+](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)

Files: `src/beta.h`  
Copyright: © 2001-2019 Contributors to the Freedoom project.  
License: [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)

Files: `src/dogs.h`  
Copyright:  
 © 2017 Nash Muhandes;  
 © apolloaiello;  
 © TobiasKosmos.  
License: [CC-BY-3.0](https://creativecommons.org/licenses/by/3.0/) and [CC0-1.0](https://creativecommons.org/publicdomain/zero/1.0/)

Files: `src/u_scanner.*`  
Copyright:  
 © 2010 Braden "Blzut3" Obrzut;  
 © 2019 Fernando Carmona Varo.  
License: [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)

Files: `src/ws_wadstats.*`
Copyright: © 2023 Ryan Krafnick.
License: [GPL-2.0+](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)

Files: `cmake/FindSDL2.cmake, cmake/FindSDL2_net.cmake`  
Copyright: © 2018 Alex Mayfield.  
License: [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)

Files: `src/icon.c, setup/setup_icon.c`  
Copyright: © 2022 Korp.  
License: [CC BY-NC-SA 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/)

Files: `src/thermo.h`  
Copyright: © 2020-2022 Julia Nechaevskaya.  
License: [CC-BY-3.0](https://creativecommons.org/licenses/by/3.0/)

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
 © 2002-2004 The DOSBox Team;  
 © 2005-2017 Simon Howard.  
License: [GPL-2.0+](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)

Files: `win32/win_opendir.*`  
License: public-domain
