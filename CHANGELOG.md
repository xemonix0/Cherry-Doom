## New Features

* Allowed **widescreen without aspect ratio correction**
* Implemented **stretch-to-fit viewport behavior**
* **Separated View Bobbing from Weapon Bobbing**; each now has its own value
* Implemented **selective fuzz darkening** (thanks @ceski-1)
* Implemented **toggle for Berserk display** in place of the Ammo count
* Implemented **toggle for weapon sprite centering correction**
* _**'FREEZE'**_ cheat (partially ported from DSDA-Doom)

## Changes

* **Specified `CPACK_PACKAGE_NAME`** as "Nugget-Doom" (thanks @Mariiibo)
* **Mentioned flight keys** in Key Bindings setup menu
* **Rearranged _Doom Compatibility_ setup menu's pages 4 and 5**
* **Changed maximum NUGHUD drawing position values**: from `X = 320` and `Y = 200` to `X = 319` and `Y = 199` respectively
* **Changed `gammacycle`'s description**

## Bug Fixes

* **Fixed Berserk being drawn in fullscreen HUD**
* **Fixed weapon switching with bobbing disabled and forced weapon sprite coordinates**
* **Prevented weapon bobbing when forcing weapon sprite coordinates**
* **Corrected position of Time/STS widgets in NUGHUD**; this might change the position in which they're displayed in some existing NUGHUDs
