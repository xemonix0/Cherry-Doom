# Cherry Doom
[![Cherry Doom Icon](https://raw.githubusercontent.com/xemonix0/Cherry-Doom/master/data/cherry-doom.png)](https://github.com/xemonix0/Cherry-Doom)

[![Release](https://img.shields.io/github/release/xemonix0/Cherry-Doom.svg)](https://github.com/xemonix0/Cherry-Doom/releases/latest)
[![Release Date](https://img.shields.io/github/release-date/xemonix0/Cherry-Doom.svg)](https://github.com/xemonix0/Cherry-Doom/releases/latest)
[![Downloads (total)](https://img.shields.io/github/downloads/xemonix0/Cherry-Doom/total)](https://github.com/xemonix0/Cherry-Doom/releases/latest)
[![Downloads (latest)](https://img.shields.io/github/downloads/xemonix0/Cherry-Doom/latest/total.svg)](https://github.com/xemonix0/Cherry-Doom/releases/latest)
[![Commits Since Latest Release](https://img.shields.io/github/commits-since/xemonix0/Cherry-Doom/latest.svg)](https://github.com/xemonix0/Cherry-Doom/commits/master)
[![Last Commit](https://img.shields.io/github/last-commit/xemonix0/Cherry-Doom.svg)](https://github.com/xemonix0/Cherry-Doom/commits/master)

Cherry Doom is a fork of [Nugget Doom](https://github.com/MrAlaux/Nugget-Doom) intended to add even more features.

> **Disclaimer:** Although the new code has been written with the intention of not breaking demo compatibility, it has not been properly tested yet. **RECORD DEMOS AT YOUR OWN RISK!**

## Features

Cherry Doom builds on top of Nugget Doom with numerous enhancements, most being cosmetic and quality-of-life additions, such as:
- Floating powerups _(from International Doom)_
- Rocket trails _(from Doom Retro)_
- Less blinding tints
- Dithered lighting _(from Doom Retro)_

The main highlight of this port is the _Level Table_, inspired by DSDA-Doom. It is used for tracking individual level progress, has a summary screen with total WAD statistics, and allows you to warp to any map without using cheats.

_For a full list of features and more details on the Level Table, check out [FEATURES.md](FEATURES.md)._

## Compiling

The Cherry Doom source code is available at GitHub: <https://github.com/xemonix0/Cherry-Doom>.

It can be cloned via

```shell
git clone https://github.com/xemonix0/Cherry-Doom.git
```

### Linux, and Windows with MSYS2

The following build system and libraries need to be installed:
 
 * [CMake](https://cmake.org) (>= 3.15)
 * [SDL2](https://github.com/libsdl-org/SDL/tree/SDL2) (>= 2.0.18)
 * [SDL2_net](https://github.com/libsdl-org/SDL_net)
 * [openal-soft](https://github.com/kcat/openal-soft) (>= 1.22.0 for PC Speaker emulation)
 * [libsndfile](https://github.com/libsndfile/libsndfile) (>= 1.1.0 for MPEG support)
 * [libebur128](https://github.com/jiixyj/libebur128) (>= 1.2.0)
 * [yyjson](https://github.com/ibireme/yyjson) (>= 0.10.0, optional)
 * [fluidsynth](https://github.com/FluidSynth/fluidsynth) (>= 2.2.0, optional)
 * [libxmp](https://github.com/libxmp/libxmp) (optional)
 
Usually your distribution should have the corresponding packages in its repositories,
and if your distribution has "dev" versions of those libraries, those are the ones you'll need.

Once installed, compilation should be as simple as:

```shell
cd cherry-doom
mkdir build; cd build
cmake ..
make
```

After successful compilation the resulting binary can be found in the `src/` directory.

## Windows with Visual Studio

[Visual Studio 2022](https://visualstudio.microsoft.com/) comes with built-in support for CMake by opening the source tree as a folder.

Install vcpkg <https://github.com/Microsoft/vcpkg#quick-start-windows>. Integrate it into CMake or use toolchain file:
```shell
cd cherry-doom
cmake -B build -DCMAKE_TOOLCHAIN_FILE="[path to vcpkg]/scripts/buildsystems/vcpkg.cmake"
cmake --build build
```
CMake will automatically download and build all dependencies for you.

# Legalese

Files: `*`  
Copyright:  
 © 1993-1996 Id Software, Inc.;  
 © 1993-2008 Raven Software;  
 © 1999 by id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman;  
 © 1999-2004 by Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian Schulze;  
 © 2004 James Haley;  
 © 2005-2006 by Florian Schulze, Colin Phipps, Neil Stevens, Andrey Budko;  
 © 2005-2018 Simon Howard;  
 © 2006 Ben Ryves;  
 © 2007-2011 Moritz "Ripper" Kroll;  
 © 2008-2019 Simon Judd;  
 © 2017 Christoph Oelckers;  
 © 2020 Alex Mayfield;  
 © 2020 JadingTsunami;  
 © 2020-2024 Fabian Greffrath;  
 © 2020-2024 Roman Fomin;  
 © 2021-2022 Ryan Krafnick;  
 © 2021-2026 Alaux;  
 © 2022 Julia Nechaevskaya;  
 © 2022-2024 ceski;  
 © 2023 Andrew Apted;  
 © 2023 liPillON;  
 © 2024 pvictress;  
 © 2025 Guilherme Miranda;  
 © 2023-2026 Xemonix.   
License: [GPL-2.0+](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)

Files: `src/i_flickstick.*, src/i_gyro.*`  
Copyright:  
 © 2018-2021 Julian "Jibb" Smart;  
 © 2021-2024 Nicolas Lessard;  
 © 2024 ceski.  
License: [MIT](https://opensource.org/licenses/MIT)

Files: `src/nano_bsp.*`  
Copyright:  
 © 2023 Andrew Apted.  
License: [MIT](https://opensource.org/licenses/MIT)

Files: `src/m_scanner.*`  
Copyright:  
 © 2015 Braden "Blzut3" Obrzut.  
License: [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)

Files: `src/v_flextran.*`  
Copyright:  
 © 2013 James Haley et al.;  
 © 1998-2012 Marisa Heit.  
License: [GPL-3.0+](https://www.gnu.org/licenses/gpl-3.0)

Files: `src/v_video.*`  
Copyright:  
 © 1999 by id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman;  
 © 2013 James Haley et al.  
License: [GPL-3.0+](https://www.gnu.org/licenses/gpl-3.0)

Files: `src/wad_stats.*`  
Copyright: © 2021-2023 Ryan Krafnick;  
License: [GPL-2.0+](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)

Files: `base/all-all/sprites/pls*, man/simplecpp`  
Copyright:  
 © 2001-2019 Contributors to the Freedoom project.  
License: [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)

Files: `base/all-all/sprites/ngcha0.png, base/all-all/sprites/ngcla0.png, base/all-all/sprites/ngtr*0.png`  
Copyright:  
 © 2023-2024 Korp.  
License: [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/)

Files: `base/all-all/sprites/rsmk*0.png`  
Copyright:  
 © 2018 Talon1024;  
 © 2022 Brad Harding.  
License: public-domain

Files: `base/all-all/dsdg*, base/all-all/sprites/dog*`  
Copyright:  
 © 2017 Nash Muhandes;  
 © apolloaiello;  
 © TobiasKosmos.  
License: [CC-BY-3.0](https://creativecommons.org/licenses/by/3.0/) and [CC0-1.0](https://creativecommons.org/publicdomain/zero/1.0/)

Files: `base/all-all/sm*.png`  
Copyright:  
 © 2024 Julia Nechaevskaya.  
License: [CC-BY-3.0](https://creativecommons.org/licenses/by/3.0/)

Files: `base/all-all/sbardef.lmp`  
Copyright:  
 © 2024 Ethan Watson.  
License: [CC0-1.0](https://creativecommons.org/publicdomain/zero/1.0/)

Files: `base/all-all/dmxopl.op2`  
Copyright:  
 © 2017 Shannon Freeman.  
License: [MIT](https://github.com/sneakernets/DMXOPL/blob/DMXOPL3/LICENSE)

Files: `cmake/FindSDL2.cmake, cmake/FindSDL2_net.cmake`  
Copyright:  
 © 2018 Alex Mayfield.  
License: [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)

Files: `data/io.github.xemonix0.Cherry-Doom.metainfo.*`  
Copyright:  
 © 2023-2024 Fabian Greffrath.  
License: [CC0-1.0](https://creativecommons.org/publicdomain/zero/1.0/)

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

Files: `third-party/md5/*`  
License: public-domain

Files: `third-party/miniz/*`  
Copyright:  
 © 2010-2014 Rich Geldreich and Tenacious Software LLC;  
 © 2013-2014 RAD Game Tools and Valve Software.  
License: [MIT](https://opensource.org/licenses/MIT)

Files: `third-party/pffft/*`  
Copyright:  
 © 2004 The University Corporation for Atmospheric Research ("UCAR");  
 © 2013 Julien Pommier.  
License: [FFTPACK License](https://bitbucket.org/jpommier/pffft/src/master/pffft.h)

Files: `third-party/sha1/*`  
Copyright:  
 © 1998-2001 Free Software Foundation, Inc.;  
 © 2005-2014 Simon Howard.  
License: [GPL-2.0+](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)

Files: `third-party/spng/*`  
Copyright:  
 © 2018-2023 Randy.  
License: [BSD-2-Clause](https://opensource.org/license/bsd-2-clause)

Files: `third-party/yyjson/*`  
Copyright:  
 © 2020 YaoYuan.  
License: [MIT](https://opensource.org/licenses/MIT)

Files: `win32/win_opendir.*`  
Copyright:  
 © 2019 win32ports.  
License: [MIT](https://opensource.org/licenses/MIT)
