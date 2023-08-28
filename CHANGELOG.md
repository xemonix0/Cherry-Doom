**WARNING:** Saves from previous versions are incompatible with this version.

## New Features

- **Horizontal Weapon Centering** setting
- **_Switch [Weapon] on Pickup_** setting
- **Last weapon key**
- **_Blink Missing Keys_** setting
- **Support for optional sounds** (Crispy Doom's and more)
- **NUGHUD:**
  - Made patches and some Status Bar widgets alignable;
  - Armor icon;
  - Infinity icon;
  - Ammo count font;
  - Toggle to draw percentage signs.
- **_Announce Milestone Completion_** setting
- **(In)Complete Milestone Color** choices
- **Customizable dark menu background/dark Automap overlay darkening**
- **Woof savegame compatibility**

## Changes

- **Powerup Timers now display `"` instead of `S`**
- **Extended Mouselook range**; pitch effects are now applied even when looking all the way up and down
- **Disabled crosshair when using Chasecam**; now configurable through the `chasecam_crosshair` CVAR
- **Permanent Weapon Bobbing can now be toggled** through the `always_bob` CVAR

## Bug Fixes

- **FOV-related sky stretching issues**
- **Weapon lowering under certain conditions**
- **Caching of certain NUGHUD patches** (fixed some crashes when using e.g. Status Bar graphics)
- **Vertical weapon inertia reset** when disabling mouselook/padlook
- **NUGHUD forced widescreen shifting being applied in all screen sizes**
- **Death Camera health check**; it wouldn't work if the player's health were exactly 0
- **Delayed Chasecam speed effect** to mitigate stutter
