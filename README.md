# Cherry Doom
[![Cherry Doom Icon](https://raw.githubusercontent.com/xemonix0/Cherry-Doom/master/data/cherry-doom.png)](https://github.com/xemonix0/Cherry-Doom)

[![Code Size](https://img.shields.io/github/languages/code-size/xemonix0/Cherry-Doom.svg)](https://github.com/xemonix0/Cherry-Doom)
[![License](https://img.shields.io/github/license/xemonix0/Cherry-Doom.svg?logo=gnu)](https://github.com/xemonix0/Cherry-Doom/blob/master/COPYING)
[![Release](https://img.shields.io/github/release/xemonix0/Cherry-Doom.svg)](https://github.com/xemonix0/Cherry-Doom/releases/latest)
[![Release Date](https://img.shields.io/github/release-date/xemonix0/Cherry-Doom.svg)](https://github.com/xemonix0/Cherry-Doom/releases/latest)
[![Downloads](https://img.shields.io/github/downloads/xemonix0/Cherry-Doom/latest/total.svg)](https://github.com/xemonix0/Cherry-Doom/releases/latest)
[![Commits](https://img.shields.io/github/commits-since/xemonix0/Cherry-Doom/latest.svg)](https://github.com/xemonix0/Cherry-Doom/commits/master)
[![Last Commit](https://img.shields.io/github/last-commit/xemonix0/Cherry-Doom.svg)](https://github.com/xemonix0/Cherry-Doom/commits/master)
[![Build Status](https://github.com/xemonix0/Cherry-Doom/actions/workflows/main.yml/badge.svg)](https://github.com/xemonix0/Cherry-Doom/actions/workflows/main.yml)

Cherry Doom is a fork of [Nugget Doom](https://github.com/MrAlaux/Nugget-Doom) intended to add even more features.

**Note**: this README and the rest of documentation are updated on a per-commit basis, meaning that they may not correspond to the latest release.

If you're seeking information on the version you're using, please refer to the documentation included with it.

## DISCLAIMER
Although the new code has been written with the intention of not breaking demo compatibility, it has not been properly tested yet.
**RECORD DEMOS AT YOUR OWN RISK!**

## Features

Note that this feature list is relative to [Nugget Doom's](https://github.com/MrAlaux/Nugget-Doom/blob/master/README.md#features); read the latter for more details.
Some features were first introduced in Cherry Doom and later merged into Nugget Doom, and so they're not listed here anymore.

Most of Cherry Doom's features come from other sources, like other source ports, mods or games. The initial implementations for some are **ported from (p.f.)** or **inspired by (i.b.)** said sources. These acknowledgements are included in the feature lists below.

Some settings are labeled as _CFG-only_: they can only be changed by editing `cherry-doom.cfg`. For these settings, their CVAR names are provided alongside the _CFG-only_ label as guidance.

### General

- _Mute Inactive Window_ setting [p.f. International Doom]
- _Floating Powerups_ setting [p.f. International Doom]
- _Rocket Trails_ setting (with additional _Rocket Trails Interval_ and _Smoke Translucency_ settings for extended customization) [p.f. Doom Retro]

### Status Bar/HUD

- _More Widgets on the Intermission Screen_ setting: shows smaller versions of _Health_, _Armor_ and _Weapons_ widgets on the intermission screen, customizable through options
- _Movement widget_: shows the current player movement and strafing speeds
- _Disable Crosshair On Slot 1_ setting

### Miscellaneous

- Hints for disabled menu items, explaining the reason certain items are disabled
- Most setup menus have been rearranged (utilizing the new _scrollable subpages_ feature) to improve user experience and make navigation easier
- _Disable Stats Tracking_ setting for the _custom skill_

### _Level Table_

The _Level Table_, inspired by DSDA-Doom, provides a way to track your progress across the levels of a WAD.

This feature tracks statistics, such as the _skill level_, _kills_, _items_, _secrets_ and _time_, and allows you to see all that information in one place (including a _Summary_ screen, with overall statistics for the current WAD), also giving you the ability to warp to any map conveniently from the same screen.

Compared to DSDA-Doom's implementation, there are a few notable additions and changes:
- CVAR to toggle _stats tracking_ altogether (CFG-only: `lt_enable_tracking`)
- Command line parameter to _disable stats tracking_ (`-notracking`)
- _Level table stats format_ customization (CFG-only: `lt_stats_format`)
- CVAR to _toggle tracking kills and time for maps beaten not from pistol start_ (CFG-only: `lt_track_continuous`)
	- The old behavior (before 2.0.0 and in DSDA-Doom) is equivalent to this CVAR being set to `0`
- The ability to see (and warp to) all loaded maps, not just maps from the last loaded WAD
- Various visual changes

# Releases

Source code, Windows binaries (MSVC builds for Windows 7 and newer) and Linux AppImages for the latest release can be found on the [Release](https://github.com/xemonix0/Cherry-Doom/releases/latest) page.

The most recent list of changes can be found in the [Changelog](https://github.com/xemonix0/Cherry-Doom/blob/master/CHANGELOG.md).

A complete history of changes and releases can be found on the [Releases](https://github.com/xemonix0/Cherry-Doom/releases) page.

## Versioning

Cherry Doom follows the same versioning system as Nugget Doom:

- **X** is increased in the event of major implementations, as were arbitrary/dynamic resolution and voxel support;
- **Y** is increased in the event of minor implementations, such as a new cheat;
- **Z** is increased in the event of bug fixes or text/code reformatting.

Incrementing any of the first values will reset the latter (i.e. a major change to 1.1.2 would shift it to 2.0.0).

The merging of changes from Nugget Doom's releases may affect any of the version values, but not necessarily in the same way as Nugget Doom's own version (i.e. `Nugget Doom 2.Y.Z -> 3.Y.Z` doesn't necessarily mean `Cherry Doom 1.Y.Z -> 2.Y.Z`).

# Compiling

The Cherry Doom source code is available at GitHub: <https://github.com/xemonix0/Cherry-Doom>.

It can be cloned via

```shell
git clone https://github.com/xemonix0/Cherry-Doom.git
```

## Linux, and Windows with MSYS2

The following build system and libraries need to be installed:
 
 * [CMake](https://cmake.org) (>= 3.9)
 * [SDL2](https://github.com/libsdl-org/SDL/tree/SDL2) (>= 2.0.18)
 * [SDL2_net](https://github.com/libsdl-org/SDL_net)
 * [openal-soft](https://github.com/kcat/openal-soft) (>= 1.22.0 for PC Speaker emulation)
 * [libsndfile](https://github.com/libsndfile/libsndfile) (>= 1.1.0 for MPEG support)
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

[Visual Studio 2019](https://visualstudio.microsoft.com/) and [Visual Studio Code](https://code.visualstudio.com/) come with built-in support for CMake by opening the source tree as a folder.

Install vcpkg <https://github.com/Microsoft/vcpkg#quick-start-windows>. Integrate it into CMake or use toolchain file:
```shell
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
 © 1999-2004 by Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian Schulze;  
 © 2004 James Haley;  
 © 2005-2006 by Florian Schulze, Colin Phipps, Neil Stevens, Andrey Budko;  
 © 2005-2018 Simon Howard;  
 © 2006 Ben Ryves;  
 © 2007-2011 Moritz "Ripper" Kroll;  
 © 2008-2019 Simon Judd;  
 © 2017 Christoph Oelckers;  
 © 2019 Fernando Carmona Varo;  
 © 2020 Alex Mayfield;  
 © 2020 JadingTsunami;  
 © 2020-2024 Fabian Greffrath;  
 © 2020-2024 Roman Fomin;  
 © 2021 Ryan Krafnick;  
 © 2021-2024 Alaux;  
 © 2022 Julia Nechaevskaya;  
 © 2022-2024 ceski;  
 © 2023 Andrew Apted;  
 © 2023 liPillON;  
 © 2023-2024 Xemonix.   
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

Files: `src/nano_bsp.*`  
Copyright: © 2023 Andrew Apted.  
License: [MIT](https://opensource.org/licenses/MIT)

Files: `src/u_scanner.*`  
Copyright:  
 © 2010 Braden "Blzut3" Obrzut;  
 © 2019 Fernando Carmona Varo.  
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

Files: `src/wadstats.*`  
Copyright: © 2021-2023 Ryan Krafnick.  
License: [GPL-2.0+](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)

Files: `cmake/FindSDL2.cmake, cmake/FindSDL2_net.cmake`  
Copyright: © 2018 Alex Mayfield.  
License: [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)

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
