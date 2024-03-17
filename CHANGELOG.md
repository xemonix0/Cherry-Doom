## New Features

- **_Night-Vision Visor Effect_** setting
- **_Alternative Intermission Background_** setting
- **Rewinding** [thanks @rfomin]
- **_'IDDF'_ cheat**, to find a key on the Automap
- **Support for optional health-based player pain sounds**
- **Support for Stats icons**
- **_SUMMON_ cheat spawning mobjs at position of Automap pointer**
- **_Higher god-mode face priority_** setting

## Changes

- **Merged changes from the following Woof! releases:**
  - **[Woof! 14.0.0](https://github.com/fabiangreffrath/woof/releases/tag/woof_14.0.0)**, note:
    - Replaced Nugget's _Organize Saves by IWAD_ feature with Woof's _Organize Save Files_, which also organizes by PWAD [1]
    - Temporarily removed _Selective Fuzz Darkening_
    - Adopted Woof's screen sizes, and adapted the Nugget HUD accordingly:
      The Nugget HUD is now accessed by setting the "HUD Type" to "Nugget" (which is the default),
      and the widescreen arrangement depends on the "HUD Mode" setting
    - Replaced Nugget's crosshair-coloring-by-target-health logic with Woof's
    - Replaced Nugget's use-button timer with Woof's, changed the CVAR names of Nugget's other event timers accordingly [2]
      and removed the "only in demos" option
    - Replaced Nugget's unrevealed-secret-sector Automap color with Woof's revealed-secret-sector color
    - Removed _Smart Totals_ setting
    - Removed extended gamma levels and `gammacycle`
    - Changed `menu_background_darkening` to `menu_backdrop_darkening` [2]
    - Rearranged menus
    - Maintained minimum window size of 200p/240p
  - **[Woof! 14.1.0](https://github.com/fabiangreffrath/woof/releases/tag/woof_14.1.0)**, note:
    - Maintained SDL render driver setting (`sdl_renderdriver`)
  - **[Woof! 14.2.0](https://github.com/fabiangreffrath/woof/releases/tag/woof_14.2.0)**
  - **[Woof! 14.3.0](https://github.com/fabiangreffrath/woof/releases/tag/woof_14.3.0)**, note:
    - Restored `screen_melt`, replacing `wipe_type` [2]
    - Replaced "Seizure" wipe with Woof's "Crossfade"
    - Renamed "Fade" wipe to "Black Fade", and changed its value to `4` to make room for the "Fizzle" wipe
- **Reimplemented _Move Over/Under Things_ feature** [3], making it much less bug-prone
- **Renamed _Impact Pitch_ (`impact_pitch`) to _Flinching_ (`flinching`)** [2]
- **Tweaked zooming effect**
- **Changed internal values of Nugget's internal mobj flags** [4]
- **Screenshot directory doesn't default to `savegame_dir` if set anymore**
- **Rebranded "Crispy HUD" as "Nugget HUD"**

**[1]\.** This means that existing save files may require to be moved between folders for Nugget to detect them.

**[2]\.** This will affect existing config files.

**[3]\.** Among other changes, the setting itself has been extended: a value of `1` enables the feature only for players,
while a value of `2` enables it for all Things. This differs from the previous implementation, wherein `1` would enable
the feature for all Things.

**[4]\.** This may affect existing saves.

## Bug Fixes

- **FOV effects not being cleared thoroughly upon loading levels**
- **Turbo cheat setting incorrect walk-strafing speed**
- **Crash when toggling Alt. Arms Display without entering a map**
- **_No Melee Snapping_ toggle not being forcefully disabled during non-casual play**
