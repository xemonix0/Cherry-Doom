## New Features

- **_Thing Lighting Mode_** setting
- **_Smart Autoaim_** setting

## Changes

- **Merged changes from the following Woof! releases:**
  - **[Woof! 15.2.0](https://github.com/fabiangreffrath/woof/releases/tag/woof_15.2.0)**, note:
    - Removed `vertical_layout` property of SBARDEF widgets in favor of Woof's `vertical` [1]
  - Also merged reporting of SDL release/platform from Woof! post-15.2.0
- **Vertical weapon inertia now takes the sprite's height into account**
- **Renamed `diminished_lighting` to `diminishing_lighting`** [2]

## Bug Fixes

- **Weapon voxel models being incorrectly affected by view pitch**

**[1].** This may affect existing NUGHUDs.
**[2].** This may affect existing config files.
