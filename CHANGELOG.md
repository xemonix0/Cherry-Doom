## New Features

- **Experimental 4X (800p) and 8X (1600p) resolutions** (by @ceski-1)
- **_Chasecam_**
- **_Death Camera_ toggle**
- **_Impact Pitch_ setting**
- **_Fake Contrast_ toggle**
- **_Screen Wipe speed percentage_ setting**
- **_Powerup Timers_**
- **_Weapon Inertia_ setting** (by @ceski-1)
- **_Translucent [Weapon] Flashes_ toggle**
- **_'BOOMCAN'_ cheat, for explosive hitscan attacks**
- **Arms number 1 is now highlighted only when the player has Berserk**

## Changes

- **Merged changes from the following Woof releases:**
  - [11.2.0](https://github.com/fabiangreffrath/woof/releases/tag/woof_11.2.0);
  - [11.3.0](https://github.com/fabiangreffrath/woof/releases/tag/woof_11.3.0), therefore:
    - Replaced `freeaim` with `vertical_aiming`;
    - Crosshair lock-on now works with direct vertical aiming.
- **FOV changes are now interpolated**
- **Zoom is now reset upon teleporting**
- **_'SUMMON'_ now reports the last summoned mobj's type**
- **_'SUMMONE'_ and _'SUMMONF'_ now print a message requesting the mobj index**
- **Mobjs summoned with _'SUMMON'_ now inherit the player's angle**
- **Alt. Arms Display now accounts for SSG in Doom 1**
- **Bloodier Gibbing now spawns a minimum of 20 blood splats** (previous minimum was 21)
- **Renamed _Advance Internal Demos_ to _Play Internal Demos_**
- **Changed `gamma2` default to 10** (matches Woof)
- **Changed `sx_fix` default to 0**
- **Rearranged Nugget's General settings pages, improved wording**

## Bug Fixes

- **Fixed Pistol being marked as unavailable in the Status Bar**
- **Fixed wall/sprite lighting being affected by FOV**
- **Fixed flickering of the Ammo count when changing screen sizes with weapons with no ammo**
- **Fixed screen melt inconsistency across resolutions**
- **Fixed top-aligned widgets being unnecessarily shifted with Centered Messages enabled**
- **Fixed potential crash when changing FOV while zoomed in**
