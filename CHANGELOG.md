## New Features

- **Support for weapon voxel models**
- **_'RIOTMODE'_** cheat, to make enemies attack all sentient entities
- **_Vertical Target Lock-on_** setting
- **_Message Fadeout_** setting
- **_Weapon Bob Speed_** setting
- **_Bob [Weapon] While Switching_** setting
- **Low-resolution pixel width/height** settings
- **_[Color] Contrast_** setting [by @pvictress]

## Changes

- **Merged changes from [Woof! 15.1.0](https://github.com/fabiangreffrath/woof/releases/tag/woof_15.1.0)**, note:
  - Replaced `fullscreen_hud_type` with `use_nughud`, with the NUGHUD now replacing the second-to-last HUD
  - Removed `fuzzdark_mode` in favor of Woof!'s `fuzzmode` (_Refraction_ is equivalent to _Selective Fuzz Darkening_)
  - Removed `comp_blazing2`; its functionality has been integrated into `comp_blazing`
- **Applied weapon inertia while firing** (added `weapon_inertia_fire` CVAR to disable it)
- **Lowered lower limit of `weapon_inertia_scale_pct` to -200**
- Nugget's translucency features now use translucency maps from a shared pool,
  potentially improving program startup time in exchange for stutters
  when enabling said features for the first time since launch

## Bug Fixes

- **Broken movement in systems assuming `char`s to be unsigned**
- **`comp_lsamnesia` inversely affecting collision with entities** (i.e. lost souls would remember their target)
- **Weapon sprites being raised/lowered excessively when using switch interruption**
  with certain bobbing styles and/or reduced weapon bobbing amplitude
- **Voxels rendering incorrectly near the top and bottom of the screen**
- **_Flip Levels_ setting not working for side screens in three-screen mode**
- **Message grouping only checking as many characters as the last message had**,
  causing incorrect grouping (e.g. new message "TEST" being grouped with last message "TES")
- **Fixed freecam angle being reset to face east when letting go of a mobj**
- **Fixed chasecam being forcefully enabled when locked onto a mobj while the player were dead and the _Death Camera_ were enabled**
