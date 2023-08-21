# Cherry Doom

Cherry Doom is a fork of [Nugget Doom](https://github.com/MrAlaux/Nugget-Doom) intended to add even more features.

### DISCLAIMER
Although the new code has been written with the intention of not breaking demo compatibility, it has not been properly tested yet.
**RECORD DEMOS AT YOUR OWN RISK!**

# Features
Just like with Nugget Doom, most of Cherry Doom's features come from other sources, like source ports, mods or games. The initial implementations for some are **ported from (p.f.)** or **inspired by (i.b.)** said sources. These acknowledgements are included in the feature lists below.

### General

- **More menu background choices:** Solid, Dark, Darker; background can also be enabled for all menus
- **Screen shake effect on damage** [p.f. Doom Retro]
- **Motion blur** [p.f. Doom Retro]

### Weapons

- **Last used weapon** key [i.b. Half-Life]

### Status Bar/HUD

- Toggling of **extended HUD bars**

### Miscellaneous

- **Level table** and WAD stats tracking [p.f. DSDA-Doom]

# Releases

Source code and Windows binaries (MSVC builds for Windows 7 and newer) for the latest release can be found on the [Release](https://github.com/MrAlaux/Nugget-Doom/releases/latest) page.

The most recent list of changes can be found in the [Changelog](https://github.com/xemonix0/Cherry-Doom/blob/master/CHANGELOG.md).

A complete history of changes and releases can be found on the [Releases](https://github.com/xemonix0/Cherry-Doom/releases) page.

## Versioning

Cherry Doom follows exactly the same versioning system as Nugget Doom, which is fairly simple:

- **X** is increased in the event of at least one major implementation, such as a new spec like _UMAPINFO_;
- **Y** is increased in the event of at least one minor implementation, such as a new cheat;
- **Z** is increased in the event of at least one bug fix, text/code reformatting, or merging of _Nugget Doom_ updates, even if the changes to the latter are considered minor or major.

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

 * [SDL2](https://github.com/libsdl-org/SDL/tree/SDL2) (>= 2.0.18)
 * [SDL2_net](https://github.com/libsdl-org/SDL_net)
 * [openal-soft](https://github.com/kcat/openal-soft)
 * [libsndfile](https://github.com/libsndfile/libsndfile) (>= 1.1.0 for MPEG support)
 * [fluidsynth](https://github.com/FluidSynth/fluidsynth) (>= 2.2.0, optional)
 * [libxmp](https://github.com/libxmp/libxmp) (optional)
 
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

Visual Studio 2019 and [VSCode](https://code.visualstudio.com/) comes with built-in support for CMake by opening the source tree as a folder.

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
 © 2022 ceski;  
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

Files: `cmake/FindSDL2.cmake, cmake/FindSDL2_net.cmake`  
Copyright: © 2018 Alex Mayfield.  
License: [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)

Files: `data/nugget-doom.ico, data/nugget-doom.png, data/nugget-doom8.ico, src/icon.c, data/setup.ico, data/setup8.ico, setup/setup_icon.c, data/nugget-doom-setup.png`  
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
