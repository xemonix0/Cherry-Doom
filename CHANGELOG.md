## New Features

- **Implemented experimental 4X (800p) and 8X (1600p) resolutions**
- **Implemented chasecam**
- **Arms number 1 can now be colored red if Berserk is available**

## Changes

- **Merged changes from Woof 11.2.0**
- **Changed mouselook range to [-100, 100]**
- **FOV changes are now interpolated**
- **Zoom is now reset upon teleporting**
- **_'SUMMON'_ now reports the last summoned mobj's type**
- **_'SUMMONE'_ and _'SUMMONF'_ now print a message requesting the mobj index**
- **Mobjs summoned with _'SUMMON'_ now inherit the player's angle**
- **Alt. Arms Display now accounts for SSG in Doom 1**
- **Changed `gamma2` default to 10 (matches Woof)**
- **Changed `sx_fix` default to 0**

## Bug Fixes

- **Fixed Pistol being marked as unavailable in the Status Bar**
- **Fixed flickering of the Ammo count when changing screen sizes with weapons with no ammo**
- **Fixed screen melt inconsistency across resolutions**
- **Fixed potential crash when changing FOV while zoomed in**
