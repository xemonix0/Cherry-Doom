# Changelog

This file contains the full list of changes for every version of Cherry Doom.

## Unreleased

### Added

- _Dithered Lighting_ setting
- _Pulsating Message Display_ setting
- _[Weapon] Switch Speed_ setting

### Changed

- Reverted all rearrangements made to Nugget Doom's menu items and moved Cherry Doom's display options into a separate tab
- The level table now fits 16 rows on the screen instead of 15
- The level table now displays partial totals on the summary page
- _Detection of Targets in Darkness_ now accounts for weapon flashes, the light amplification visor, the invulnerability colormap and the _Extra Lighting_ setting
- Added a scrollbar to the level table, replacing the barely noticeable arrows

### Fixed

- Multiple issues with the dark backdrop fade-in/out animations
- Demo/coop desyncs related to intermission screen fixes introduced in 2.0.0

## 2.1.0

**Cherry Doom 2.1.0**, released June 30th, 2026. This version is up to date with [_Nugget Doom 5.1.0_](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-5.1.0) and [_Woof! 15.3.0_](https://github.com/fabiangreffrath/woof/releases/tag/woof_15.3.0).

### Added

- _Less Blinding Tints_ setting by [@Spaicrab](https://github.com/Spaicrab)
- Ability to erase stats for the selected map (via the <kbd>Del</kbd> key) and the whole WAD (via the reset button) from the level table screen

### Changed

- **_Save_ version has been changed to `Cherry 2.1.0`**
- **Merged changes from Nugget Doom releases [3.3.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-3.3.0) through [5.1.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-5.1.0)**, note:
  - Stats tracking is disabled when _Duplicate Monsters_ is active
- _Blood amount scaling with the amount of damage dealt_ is now toggleable through a setting and is disabled by default
- Introduced minor improvements to rocket smoke physics
- _Detection of Targets in Darkness_ now accounts for fullbright frames
- WAD stats are now written to the file every time a level is beaten
- `TNT31.WAD` is no longer considered a separate WAD by the stats tracking system when loaded with `TNT.WAD`
- _Rocket Trails_ now work even when DSDHacked patches are loaded
- Removed menu items for the _Rocket Trails Interval_ and _Smoke Translucency_ settings
- Times on the level table now display hours when exceeding 60 minutes
- Renamed the `rocket_trails_tran` CVAR to `rocket_trails_tran_pct` for consistency [^1]
- The dark backdrop in menus and in the automap now fades in and out

### Fixed

- `stats.txt` still being created when tracking is globally disabled

### Removed

- The _Movement widget_, favoring Woof!'s _speedometer_ and _command history widget_
- _Low/Ok/Good/Extra value color customization_ and the _intermission screen widgets_ due to the removal of Health, Armor, Ammo, and Weapons widgets
- Support for Woof! and Nugget Doom savefiles

## 2.0.0

**Cherry Doom 2.0.0**, released August 5th, 2024. This version is up to date with [_Nugget Doom 3.2.0_](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-3.2.0) and [_Woof! 14.5.0_](https://github.com/fabiangreffrath/woof/releases/tag/woof_14.5.0).

### Added

- _Mute Inactive Window_ setting
- _Floating Powerups_ setting
- _Rocket Trails_ setting (customizable via the _Rocket Trails Interval_ and _Smoke Translucency_ settings)
- Crosshair > _Disable On Slot 1_ setting
- Crosshair > _Detection of Targets in Darkness_ setting
- _Mouselook_ option for the _Stretch Short Skies_ setting
- Setting to _adjust intermission kill percentage to follow UV max speedrun requirements_ (CFG-only: `inter_accurate_kill_count`)
- Hints for some disabled menu items

### Changed

- **_Save_ version has been changed to `Cherry 2.0.0`**
- **_WAD stats_ version has been changed back to `1`**
- Merged changes from Nugget Doom releases [2.2.1](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-2.2.1) through [3.2.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-3.2.0), note:
  - Maintained support for Woof! 14.0.0 saves
  - _Stats tracking_ can be disabled for _custom skill_ games using the new _Disable Stats Tracking_ setting
  - If not disabled, the _stats tracking system_ converts the _custom skill level_ to one of the vanilla difficulties by checking chosen _Thing Spawns_ (+ ITYTD and NM specific modifiers)
- New icon!
- Separate options for _Health & Armor_ and _Weapons_ widgets on the intermission screen have been introduced
- Minor menu color changes:
  - _Selected setup menu tabs_: Yellow > Green
  - _Setup menu titles_: Yellow > Red
- The _Movement Widget_ is now disabled in _strict mode_
- Most setup menus have been rearranged (utilizing the new _scrollable subpages_ feature)
- Items percentage is now 100% on maps without items
- Blood amount now scales with the amount of damage dealt

### Fixed

- Widgets disappearing from the intermission screen when exiting setup menus
- _Backdrop Darkening_ and _Overlay Darkening_ items not being disabled properly
- Widgets overlapping on the intermission screen when _Nugget HUD_ is enabled

### Removed
- Cherry's _extended HUD color customization_, except for colors for _Low/Ok/Good/Extra values_, to streamline the user experience
- _Motion Blur_ setting, due to the performance hit it causes
- _Damage Shake Effect_ setting, in favor of Nugget Doom's _Flinch Upon Damage_
- _View Shake Intensity_ setting, in favor of Nugget Doom's _Explosion shake intensity percent_ setting

### Stats Tracking & Level Table

This release introduces many changes, fixes and additions to the _Level Table_ and the _stats tracking system_, all of which are listed below, separate from other changes for clarity.

#### Added

- Setting to toggle _stats tracking_ (CFG-only: `lt_enable_tracking`)
- Command line parameter to _disable stats tracking_ (`-notracking`)
- Setting to toggle _tracking kills and time for maps not beaten from a pistol start_ (CFG-only: `lt_track_continuous`)
- Setting to toggle _resetting stats for the current level upon beating the level on a new best skill (except Nightmare)_ (CFG-only: `lt_reset_on_higher_skill`)
- _Level Table Stats Format_ setting (CFG-only: `lt_stats_format`)

#### Changed

- Some color and formatting changes have been made to the _Level Table_:
    - Skill names are displayed instead of numbers
    - All valid times are displayed in white
    - Various changes to the Summary screen
- _Stats tracking_ now ignores WADs without maps when creating data folders for _stats files_
    - This means that existing `stats.txt` files may require to be moved between `cherry_doom_data` subfolders for Cherry Doom to detect them
- The _Level Table_ is now inaccessible in multiplayer
- Invalid _stats files_ are now non-fatal
- The _Level Table_ now displays all loaded maps

#### Fixed

- Doom 1 maps from PWADs being listed on the _Level Table_ when loaded with the Doom 2 IWAD and vice versa (see Maps of Chaos)
- _Level Table_'s Summary screen counting unavailable maps towards the totals

#### Removed

- **_Attempt tracking_**

## 1.0.1

**Cherry Doom 1.0.1**, released October 14th, 2023. This version is up to date with [_Nugget Doom 2.2.0_](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-2.2.0) and [_Woof! 12.0.0_](https://github.com/fabiangreffrath/woof/releases/tag/woof_12.0.0).

### Changed
- **_Save_ version has been changed to `Cherry 1.0.1`**
- **Merged changes from [Nugget Doom 2.2.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-2.2.0)**, note:
  - Removed Cherry's shake effect implementation in favor of Nugget's
  - Removed the "Boss Death Shake Effect" setting
  - Replaced the "Screen Shake Percentage" setting with "Max Shake Intensity"
  - Made the view shake effect wear off faster

### Removed
- _Consistent Widget Name Color_ setting
- The `TIME` label from the time widget

## 1.0.0

**Cherry Doom 1.0.0**, released September 1st, 2023. This version is up to date with [_Nugget Doom 2.1.0_](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-2.1.0) and [_Woof! 11.3.0_](https://github.com/fabiangreffrath/woof/releases/tag/woof_11.3.0).

### Added

- _Last Used [Weapon]_ key
- **The _Level Table_ from DSDA-Doom**
- _Damage, Explosion & Boss Death Screen Shake_ settings
- _Motion Blur_ setting
- _Draw Bars [In Widgets]_ setting
- _More Widgets On Intermission Screen_ setting (displays Health, Armor, Weapons and Attempts)
- Woof! savegame compatibility
- _Attempt counter_ widget and an _Attempts column_ in the Level Table
- _Movement_ widget (shows the current movement inputs (e.g. "MF40 SR50"))
- Tag Finder color customization
- Color settings for the following HUD components:
  - Widget names
  - Plain text
  - Total level time
  - Low/Ok/Good/Extra thresholds for Health, Armor and Ammo widgets

### Changed

- **_Save_ version has been changed to `Cherry 1.0.0`.** Versions `Woof 6.0.0` and `Nugget 2.1.0` are supported
- New icon
- Menu background can now be enabled in all menus

[^1]: Affects existing config files.