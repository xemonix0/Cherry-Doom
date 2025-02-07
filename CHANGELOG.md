## New Features

- **_Thing Lighting Mode_** setting
- **_Smart Autoaim_** setting
- **_Improved Weapon Toggles_** setting

## Changes

- **Merged changes from the following Woof! releases:**
  - **[Woof! 15.2.0](https://github.com/fabiangreffrath/woof/releases/tag/woof_15.2.0)**, note:
    - Removed `vertical_layout` property of SBARDEF widgets in favor of Woof's `vertical` [1]
  - Also merged reporting of SDL release/platform from Woof! post-15.2.0
- **Vertical weapon inertia now takes the sprite's height into account**
- **Made the following CVARs settable in `OPTIONS`:**
  - `always_bob`
  - `announce_milestones`
  - `bonuscount_cap`
  - `chasecam_crosshair`
  - `chasecam_distance`
  - `chasecam_height`
  - `chasecam_mode`
  - `damagecount_cap`
  - `menu_background_all`
  - `message_fadeout`
  - `message_flash`
  - `no_berserk_tint`
  - `no_killough_face`
  - `no_menu_tint`
  - `no_radsuit_tint`
  - `s_clipping_dist_x2`
  - `use_nughud`
  - `weapswitch_interruption`
- **Made `vertical_lockon` not settable in `OPTIONS`**
- **Renamed `diminished_lighting` to `diminishing_lighting`** [2]

## Bug Fixes

- **Weapon voxel models being incorrectly affected by view pitch**

**[1].** This may affect existing NUGHUDs.  
**[2].** This may affect existing config files.  
