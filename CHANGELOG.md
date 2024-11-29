## New Features

- **_Slow Motion_ button**
- **_'TRAILS'_ cheat**, to show hitscan trails
- **Color settings** from International Doom

## Changes

- **Merged changes from [Woof! 15.0.0](https://github.com/fabiangreffrath/woof/releases/tag/woof_15.0.0)**, note:
  - NUGHUD now partially uses SBARDEF as its backend, which may cause some rendering differences
  - Integrated periodic auto saves into the save/load menus
  - Maintained key blink, message list, message duration, and chat-message duration settings [1]
  - Removed `show_ssg` (now default behavior), `alt_arms` and `hud_highlight_weapon` settings
  - Renamed `#_bobbing_percentage` to `#_bobbing_pct` [2]
  - Renamed `show_berserk` to `sts_show_berserk` [2]
  - Moved `sts_show_berserk` menu item to _Status Bar/HUD_ setup menu
  - Revised the descriptions of many of Nugget's new CVARs
- **Messages in the message list now have individual durations**
- **Made the minimap customizable through SBARDEF and NUGHUD**
- **Improved FOV-based sky stretching**
- **Smoother FOV effects**
- **Allowed orbiting around freecam mobj**
- **Replaced `translucent_pspr(_pct)` with `pspr_translucency_pct`** [2]
- **Made _Screen Wipe Speed Percentage_ setting affect the _Fizzle_ fade**
- **Raised maximum _Rewind Depth_ to 3000**
- **Gave shadow to the _Pause_ graphic when using _HUD/Menu Shadows_**
- **Removed _Physical [Weapon] Recoil_ menu item**

## Bug Fixes

- **Desync involving lost-soul charge attack**
- **_Double autoaim range_ setting doubling range of BFG tracers**
- **FOV effects disabling interpolation of weapon sprites**
- **Crash when loading WADs with empty lumps between `C_#` markers** (fixes `nt_rc1.wad`)
- **Shadows not being drawn for HUD icons when using Boom font**

**[1].** Not necessarily with the same CVARs; existing config files may be affected.

**[2].** This will affect existing config files.
