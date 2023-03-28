# Nugget Doom

Nugget Doom (formerly known as IRamm, originally a [Crispy Doom](https://www.chocolate-doom.org/wiki/index.php/Crispy_Doom) fork) is a fork of [Woof!](https://github.com/fabiangreffrath/woof), simply intended to implement additional features.

### DISCLAIMER
Although the new code has been written with the intention of not breaking demo compatibility, it has not been properly tested yet.
**RECORD DEMOS AT YOUR OWN RISK!**

## Features

Most of Nugget Doom's features come from other sources, like source ports and mods; the initial implementations for some are **ported from (p.f.)** said sources, while others are just **inspired by (i.b.)** them. These acknowledgements are included in the feature lists below; be aware that some might be inaccurate or outright missing.

A few settings are labeled as _**CFG-Only**_: they can only be toggled by editing `nugget-doom.cfg`. For these settings, their internal variable names are provided alongside the label as guidance.

### General

- **Selection of widescreen ratios** in the setup menu itself [i.b. Crispy Doom]
- Allowed **usage of widescreen without aspect ratio correction**
- Toggle to **stretch game window to fit screen** (CFG-Only: `stretch-to-fit`) [i.b. and partially p.f. Crispy Doom; i.b. ZDoom]
- **Gamma Correction slider ranging from 0.50 to 2.0 in steps of 0.05**
- Toggle for **Gamma Correction key cycling**, through either original or extended gamma levels (CFG-Only: `gammacycle`)
- **Screen Wipe Style** selection (**Warning**: _"Seizure"_ might legitimately be seizure-inducing, use with caution)
- _**Disable palette tint in menus**_ setting [i.b. Crispy Doom]
- _**Field of View**_ setting [p.f. Doom Retro]
- _**Things move over/under things**_ setting [p.f. Crispy Doom]
- **Jumping** (default key: <kbd>Alt</kbd>, must be enabled first) [p.f. Crispy Doom]
- **Crouching/ducking** (default key: <kbd>C</kbd>, must be enabled first) [i.b. ZDoom]
- _**Damage Tint Cap**_ and _**Bonus Tint Cap**_, to attenuate or disable the red and yellow screen tint
- _**Disable Berserk Tint**_ setting
- _**View Bobbing Percentage**_ setting [i.b. Crispy Doom, ZDoom]
- _**View Height**_ setting, which allows to enter a custom height value between 32 and 56 for the player's POV [i.b. Brutal Doom]
- _**Sound Clipping Distance**_ selection, to optionally double the distance at which SFX become audible
- _**One-Key Quick Save/Load**_ setting, to skip the confirmation prompt
- Setting of condition to _**Advance Internal Demos**_
- _**Quick "Quit Game"**_ setting, to skip the confirmation prompt [p.f. Crispy Doom]
- Toggling of _**Weapon Flash Lighting**_ [p.f. Crispy Doom]
- Toggling of _**Weapon Flash Sprite**_ [p.f. Crispy Doom]
- Toggling of _**Invulnerability Colormap**_ [p.f. Crispy Doom]

### Weapons

- Restored _**Weapon Recoil**_ menu item
- _**Disable Horizontal Autoaim**_ setting
- **Freeaim** (Vertical Aiming) [p.f. Crispy Doom]
- _**Weapon Bobbing Percentage**_ setting [i.b. Crispy Doom, ZDoom]
- _**Bobbing Styles**_ selection [p.f. Zandronum]
- Setting to **Draw Berserk in place of Ammo count when the Fist is equipped** [p.f. Crispy Doom]
- _**Squat Weapon Down On Impact**_ setting [p.f. Crispy Doom]
- **"Correct" first person sprite centering** setting, to toggle the 1px first person sprite misalignment (CFG-Only: `sx_fix`)

### Status Bar/HUD

- **NUGHUD lump support**, making the Crispy HUD (now called Nugget HUD) customizable (see `docs/nughud.txt`)
- **SSG is accounted for on the Arms widget** [p.f. Crispy Doom]
- _**Alternative Arms Display**_ setting, to show the Chainsaw or SSG's availability on the Arms widget in place of the trivial Pistol
- _**Smart Totals**_ setting [p.f. So Doom]
- _**Smooth Health/Armor Count**_ setting, to progressively increase and decrease said displayed values [i.b. Heretic]
- **Crosshair:**
  - 5 new types;
  - Shaded variants for all types;
  - Vertical-only target lock-on;
  - Horizontal autoaim indicators;
  - Option to account for fuzzy targets [i.b. From Doom With Love];
  - Dedicated toggle key.

### Automap

- **Automap color for unrevealed secret sectors**
- _**Blink [Automap] Marks**_ key (default: <kbd>B</kbd>)

### Enemies

- _**Extra Gibbing**_ setting, to force Berserk Fist, Chainsaw and SSG gibbing [i.b. Smooth Doom]
- _**Bloodier Gibbing**_ setting [i.b. Smooth Doom]

### Doom Compatibility

- Fix Lost Soul colliding with items
- Lost Soul forgets target upon impact
- Fuzzy things bleed fuzzy blood [i.b. Crispy Doom]
- Non-bleeders don't bleed when crushed [i.b. Crispy Doom]
- Prevent Pain state with 0 damage attacks
- Bruiser attack (A_BruisAttack) doesn't face target
- Fix lopsided Icon of Sin explosions
- Permanent IDCHOPPERS invulnerability
- Blazing doors reopen with wrong sound [p.f. Crispy Doom]
- Manually reactivated moving doors are silent [p.f. Crispy Doom]
- Corrected switch sound source [p.f. Crispy Doom]
- Chaingun makes two sounds with one bullet
- Chaingunner uses pistol/chaingun sound
- Arch-Vile fire plays flame start sound [p.f. Crispy Doom]
- Dead players can still play "oof" sound [p.f. Crispy Doom]
- Key pickup resets palette

### Cheats

- _**'FULLCLIP'**_ for infinite ammo
- _**'VALIANT'**_ for fast weapons [i.b. ZDoom]
  - _**'BOBBERS'**_ serves as a shortcut to toggle the two cheats mentioned above, plus IDFA
- _**'GIBBERS'**_ to force gibbing on dying enemies, independently of damage dealt
- _**'IDFLY'**_ to fly [i.b. PrBoom+, ZDoom]
- _**'FREEZE'**_ to freeze gameplay [i.b. ZDoom]
- _**'SUMMON'**_ to spawn a hostile or friendly actor based on its mobjtype index [i.b. ZDoom]
- _**'RESURRECT' / 'IDRES'**_ to resurrect the player without toggling IDDQD [i.b. ZDoom]
- _**'LINETARGET'**_ to give some info on the player's linetarget [i.b. ZDoom]
- _**'MDK'**_ [i.b. ZDoom]
- _**'SAITAMA'**_ to enable the MDK Fist (replaces A_Punch's melee attack with the MDK attack, also featuring an alternate attack when holding down Strafe On)
- _**'NEXTMAP'**_ to exit the level [i.b. ZDoom]
- _**'NEXTSECRET'**_ to exit the level as if using a secret exit [i.b. ZDoom]
- _**'TURBO'**_ to change the player speed in-game
- **IDCLEV00 restarts current map** [p.f. Crispy Doom]
- **Mid-air control while on noclipping mode** [p.f. Crispy Doom]
- Reenabled _**'NOMOMENTUM'**_ cheat [p.f. Crispy Doom]

### Miscellaneous

- **Interactive character cast** (Turn keys to rotate enemy, Run key to gib, Strafe keys to skip) [p.f. Crispy Doom]
- Restored _**Message Listing Scrolls Upwards**_ setting, and enabled it by default
- Toggle to **allow chat in singleplayer** (CFG-Only: `sp_chat`)
- **Sound Links check for a dedicated lump**, therefore allowing usage of a custom sound effect

## Versioning

Nugget Doom follows a fairly simple **X.Y.Z** versioning system:
  - **X** is increased in the event of at least one major implementation, such as a new spec like _UMAPINFO_;
  - **Y** is increased in the event of at least one minor implementation, such as a new cheat;
  - **Z** is increased in the event of at least one bug fix, text/code reformatting, or merging of _Woof!_ updates, even if the changes to the latter are considered minor or major.

Incrementing any of the first values will reset the latter (i.e. a major change to 1.1.2 would shift it to 2.0.0).

# Building

As a Woof! fork, its build instructions should also apply here:

The source code is available at GitHub: https://github.com/MrAlaux/Nugget-Doom

## Linux, and Windows with MSYS2

On Linux, you will need to install the SDL2, SDL2_mixer and SDL2_net libraries.  Usually your distribution should have the corresponding packages in its repositories, and if your distribution has "dev" versions of those libraries, those are the ones you'll need.

Once installed, compilation should be as simple as:

```
 cd nugget
 mkdir build; cd build
 cmake ..
 make
```

After successful compilation the resulting binary can be found in the `src/` directory.

## Windows

Visual Studio 2019 comes with built-in support for CMake by opening the source tree as a folder.  Otherwise, you should probably use the GUI tool included with CMake to set up the project and generate build files for your tool or IDE of choice.

It's worth noting that you do not need to download any dependencies.  The build system will automatically download them for you.

## Cross-compiling

You may want to cross-compile from Linux to Windows.  First, make sure you have a reasonably recent version of the [Mingw-w64](https://www.mingw-w64.org/) package installed.  From there, cross-compiling should be as easy as:

```
 cd nugget
 mkdir build; cd build
 cmake -DCMAKE_TOOLCHAIN_FILE=../CrossToWin64.cmake ..
 make
```

Much like a native Windows build, you do not need to download any dependencies.

# Contact

The homepage for Nugget Doom is https://github.com/MrAlaux/Nugget-Doom

Please report any bugs, glitches or crashes that you encounter to the GitHub [Issue Tracker](https://github.com/MrAlaux/Nugget-Doom/issues).

## Acknowledgement

Help was provided by _kraflab_ (responsible for [dsda-doom](https://github.com/kraflab/dsda-doom)), _melak47_, _atsb_ (a.k.a. _Gibbon_) and _Fabian Greffrath_ himself.

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
 © 2020-2022 Fabian Greffrath;  
 © 2020 Alex Mayfield;  
 © 2020-2022 Roman Fomin;  
 © 2022 ceski;  
 © 2021-2023 Alaux.  
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

Files: `cmake/FindFluidSynth.cmake`
Copyright: © 2018 Christophe Giboudeaux.
License: [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)

Files: `cmake/FindSDL2.cmake, cmake/FindSDL2_mixer.cmake, cmake/FindSDL2_net.cmake`
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
 © 1999 Thomas A. Fine;
 © 2002-2004 The DOSBox Team;
 © 2005-2017 Simon Howard.
License: [GPL-2.0+](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)

Files: `win32/win_opendir.*`
License: public-domain
