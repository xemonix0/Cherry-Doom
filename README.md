﻿# Cherry Doom
[![Cherry Doom Icon](https://raw.githubusercontent.com/xemonix0/Cherry-Doom/master/data/cherry-doom.png)](https://github.com/xemonix0/Cherry-Doom)

[![Code Size](https://img.shields.io/github/languages/code-size/xemonix0/Cherry-Doom.svg?style=for-the-badge)](https://github.com/xemonix0/Cherry-Doom)
[![License](https://img.shields.io/github/license/xemonix0/Cherry-Doom.svg?style=for-the-badge&logo=gnu)](https://github.com/xemonix0/Cherry-Doom/blob/master/COPYING)
[![Release](https://img.shields.io/github/release/xemonix0/Cherry-Doom.svg?style=for-the-badge)](https://github.com/xemonix0/Cherry-Doom/releases/latest)
[![Release Date](https://img.shields.io/github/release-date/xemonix0/Cherry-Doom.svg?style=for-the-badge)](https://github.com/xemonix0/Cherry-Doom/releases/latest)
[![Downloads (total)](https://img.shields.io/github/downloads/xemonix0/Cherry-Doom/total?style=for-the-badge)](https://github.com/fabiangreffrath/woof/releases/latest)
[![Downloads (latest)](https://img.shields.io/github/downloads/xemonix0/Cherry-Doom/latest/total.svg?style=for-the-badge)](https://github.com/fabiangreffrath/woof/releases/latest)
[![Commits Since Latest Release](https://img.shields.io/github/commits-since/xemonix0/Cherry-Doom/latest.svg?style=for-the-badge)](https://github.com/xemonix0/Cherry-Doom/commits/master)
[![Last Commit](https://img.shields.io/github/last-commit/xemonix0/Cherry-Doom.svg?style=for-the-badge)](https://github.com/xemonix0/Cherry-Doom/commits/master)
[![Continuous Integration](https://img.shields.io/github/check-runs/xemonix0/Cherry-Doom/master?style=for-the-badge&label=Continuous%20Integration)](https://github.com/xemonix0/Cherry-Doom/actions/workflows/main.yml)

Cherry Doom is a fork of [Nugget Doom](https://github.com/MrAlaux/Nugget-Doom) intended to add even more features.

**Note**: this README and the rest of documentation are updated on a per-commit basis, meaning that they may not correspond to the latest release.

If you're seeking information on the version you're using, please refer to the documentation included with it.

## Demo Compatibility Notice
Although the new code has been written with the intention of not breaking demo compatibility, it has not been properly tested yet.
**RECORD DEMOS AT YOUR OWN RISK!**

## Features

**Important notes**:
- This feature list is relative to [Nugget Doom's](https://github.com/MrAlaux/Nugget-Doom/blob/master/README.md#features)
- Some of Cherry Doom's features originate from other sources; acknowledgements are provided _(ported from [p.f.] or inspired by [i.b.])_
- Some config variables (CVARs) don't have a corresponding menu item and can only be changed by editing `cherry-doom.cfg`. These are marked as "_CFG-only_" and the CVAR name is included for guidance

### General

- _Mute Inactive Window_ setting [p.f. [International Doom](https://jnechaevsky.github.io/inter-doom/)]
- _Floating Powerups_ setting [p.f. International Doom]
- _Rocket Trails_ setting [partially p.f. [Doom Retro](https://www.doomretro.com/)]
	- Customize smoke particle spawn rate and translucency via CFG-only CVARs: `rocket_trails_interval` and `rocket_trails_tran_pct` respectively
- _Mouselook_ option for the _Stretch Short Skies_ setting, enabling sky stretching only with mouselook active
- _Less Blinding Tints_ setting (by [@Spaicrab](https://github.com/Spaicrab))

#### Intermission screen

- Setting to adjust intermission kill percentage to meet UV max speedrun requirements (CFG-only: `inter_accurate_kill_count`)
	- Ensures that resurrected and Icon of Sin-spawned monsters do not increase the kill count. Maps without monsters display 100% kill completion
- Maps without items display 100% item completion

### Status Bar/HUD

#### Crosshair

- _Disable On Slot 1_ setting [i.b. [_Precise Crosshair_ mod](https://forum.zdoom.org/viewtopic.php?t=64788)]
- _Detection of Targets in Darkness_ setting, with required light level customizable via the CFG-only CVAR `hud_crosshair_dark_level` [i.b. [_Target Spy_ mod](https://forum.zdoom.org/viewtopic.php?t=60784)]

### Enemies

- _Blood Amount Scales With Damage_ setting [i.b. Doom Retro]

### Miscellaneous

- Menu items, disabled due to acive compatibility options, now include hints, explaining the reason for their unavailability
- Most setup menus have been reorganized using a new _scrollable subpages_ feature for easier navigation
- _Disable Stats Tracking_ setting for the _Custom Skill_

### _Level Table_

The _Level Table_, inspired by and initially ported from DSDA-Doom, tracks WAD level progress, including statistics like skill level, kills, items, secrets, and time, has a summary screen with overall statistics for the current WAD, and allows you to warp to any map.

Compared to DSDA-Doom's implementation, there are a few notable additions and changes:
- Toggleable _stats tracking_ (CFG-only: `lt_enable_tracking`)
- Command line parameter to temporarily disable stats tracking (`-notracking`)
- Option to track kills and time for maps not beaten from a pistol start (CFG-only: `lt_track_continuous`)
	- Enabled by default; disabling replicates older (pre-2.0.0) behavior.
- Option to reset stats for the current level upon achieving a new best skill, excluding Nightmare (CFG-only: `lt_reset_on_higher_skill`)
	- Enabled by default; disabling replicates older behavior.
- Ability to view and warp to all loaded maps, not just those from the last-loaded WAD, grouped by WAD filename
- Stats tracking excludes WADs without maps when creating data folders for stats files
- _Level Table Stats Format_ setting (CFG-only: `lt_stats_format`)
- Press `Del` to erase selected map stats
- "Reset to defaults" button repurposed for erasing current WAD stats
- Various visual enhancements

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
 
Usually your distribution should have the corresponding packages in its repositories, and if your distribution has "dev" versions of those libraries, those are the ones you'll need.

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
 © 2021-2025 Alaux;  
 © 2022 Julia Nechaevskaya;  
 © 2022-2024 ceski;  
 © 2023 Andrew Apted;  
 © 2023 liPillON;  
 © 2024 pvictress;  
 © 2025 Guilherme Miranda;  
 © 2023-2025 Xemonix.   
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
