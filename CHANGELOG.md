## New Features

- **_Alternative Intermission Background_** setting
- **Rewinding** [thanks @rfomin]
- **Support for optional health-based player pain sounds**
- **_Higher god-mode face priority_** setting

## Changes

- **Merged changes from [Woof! post-12.0.2](link)**, note:
  - Replaced Nugget's _Organize Saves by IWAD_ feature with Woof's _Organize Save Files_, which also organizes by PWAD¹;
  - Replaced Nugget's unrevealed-secret-sector Automap color with Woof's revealed-secret-sector color;
  - Removed _Smart Totals_ setting;
  - Removed extended gamma levels and `gammacycle`.
- **Reimplemented _Move Over/Under Things_ feature**², making it much less bug-prone
- **Improved Automap line thickening when the window is downscaled**
- **Tweaked zooming effect**
- **Changed internal values of Nugget's internal mobj flags**³

**1\.** This means that existing save files may require to be moved between folders for Nugget to detect them.

**2\.** Among other changes, the setting itself has been extended: a value of `1` enables the feature only for players,
while a value of `2` enables it for all Things. This differs from the previous implementation, wherein `1` would enable
the feature for all Things.

**3\.** This change may affect existing saves.

## Bug Fixes

- **Garbage lines (black outline) at the top of weapon sprites** [fix taken from Woof!]
- **FOV difference between widescreen modes**
- **FOV effects not being cleared thoroughly upon loading levels**
- **_"Use" Button Timer_ being disallowed in Strict Mode**
- **Turbo cheat setting incorrect walk-strafing speed**
