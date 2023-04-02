## New Features

- **Stretch-to-fit viewport behavior** (partially ported from Crispy Doom)
- **Separated View Bobbing from Weapon Bobbing**; each now has its own value
- _**Selective fuzz darkening**_ (thanks @ceski-1)
- **Zoom key**
- **Secret count in "secret revealed" message** (ported from Crispy Doom)
- _**ZDoom-like item drops**_ setting
- _**Imitate player's breathing**_ setting (ported from International Doom)
- **Toggle for Berserk display** in place of the Ammo count
- **Toggle for weapon sprite centering correction**
- _**'FREEZE'**_ cheat (partially ported from DSDA-Doom)

## Changes

- **Allowed widescreen without aspect ratio correction**
- **Extended FOV range**: now goes from 20 to 160
- **Allowed some freelook while dead**
- **Rearranged _Doom Compatibility_ setup menu's pages 4 and 5**
- **Changed maximum NUGHUD drawing position values**: from `X = 320` and `Y = 200` to `X = 319` and `Y = 199` respectively; this might affect some existing NUGHUDs
- **Mentioned flight keys** in Key Bindings setup menu
- **Changed `gammacycle`'s description**
- **Specified `CPACK_PACKAGE_NAME`** as "Nugget-Doom" (thanks @Mariiibo)

## Bug Fixes

- **Fixed Berserk being drawn in fullscreen HUD**
- **Fixed weapon switching with bobbing disabled and forced weapon sprite coordinates**
- **Prevented weapon bobbing when forcing weapon sprite coordinates**
- **Corrected position of Time/STS widgets in NUGHUD**; this might change the position in which they're displayed in some existing NUGHUDs
