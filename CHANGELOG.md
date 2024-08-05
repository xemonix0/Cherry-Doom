**_Save_ version has been changed to `Cherry 2.0.0`**  
**_WAD stats_ version has been changed back to `1`**

## New Features

- _Mute Inactive Window_ setting
- _Floating Powerups_ setting
- _Rocket Trails_ setting (customizable via the _Rocket Trails Interval_ and _Smoke Translucency_ settings)
- Crosshair > _Disable On Slot 1_ setting
- Crosshair > _Detection of Targets in Darkness_ setting
- _Mouselook_ option for the _Stretch Short Skies_ setting
- Setting to _adjust intermission kill percentage to follow UV max speedrun requirements_ (CFG-only: `inter_accurate_kill_count`)
- Hints for some disabled menu items

## Nugget Doom merges

Changes from the following _Nugget Doom_ releases have been merged:
- [2.2.1](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-2.2.1)
- [2.3.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-2.3.0)
- [2.3.1](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-2.3.1)
- [3.0.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-3.0.0), note:
	- Support for Woof! 14.0.0 saves missing in Nugget Doom 3.0.0 and up has been added
- [3.1.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-3.1.0)
- [3.2.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-3.2.0), note:
	- _Stats tracking_ can be disabled for _custom skill_ games using the new _Disable Stats Tracking_ setting
	- If not disabled, the _stats tracking system_ converts the _custom skill level_ to one of the vanilla difficulties by checking chosen _Thing Spawns_ (+ ITYTD and NM specific modifiers)

## Changes

- New icon!
- Some features added in previous versions have been removed:
	- Cherry's _extended HUD color customization_, except for colors for _Low/Ok/Good/Extra values_, to streamline the user experience
	- _Motion Blur_ setting, due to the performance hit it causes
	- _Damage Shake Effect_ setting, in favor of Nugget Doom's _Flinch Upon Damage_
	- _View Shake Intensity_ setting, in favor of Nugget Doom's _Explosion shake intensity percent_ setting
- Separate options for _Health & Armor_ and _Weapons_ widgets on the intermission screen have been introduced
- Minor menu color changes:
	- _Selected setup menu tabs_: Yellow > Green
	- _Setup menu titles_: Yellow > Red
- The _Movement Widget_ is now disabled in _strict mode_
- Most setup menus have been rearranged (utilizing the new _scrollable subpages_ feature)
- Items percentage is now 100% on maps without items
- Blood amount now scales with the amount of damage dealt

## Bug Fixes

- Widgets disappearing from the intermission screen when exiting setup menus
- _Backdrop Darkening_ and _Overlay Darkening_ items not being disabled properly
- Widgets overlapping on the intermission screen when _Nugget HUD_ is enabled

## Stats Tracking & Level Table

This release introduces many changes, fixes and additions to the _Level Table_ and the _stats tracking system_, all of which are listed below, separate from other changes for clarity.

### New Features

- Setting to toggle _stats tracking_ (CFG-only: `lt_enable_tracking`)
- Command line parameter to _disable stats tracking_ (`-notracking`)
- Setting to toggle _tracking kills and time for maps not beaten from a pistol start_ (CFG-only: `lt_track_continuous`)
- Setting to toggle _resetting stats for the current level upon beating the level on a new best skill (except Nightmare)_ (CFG-only: `lt_reset_on_higher_skill`)
- _Level Table Stats Format_ setting (CFG-only: `lt_stats_format`)

### Changes

- Removed _attempt tracking_
- Some color and formatting changes have been made to the _Level Table_:
	- Skill names are displayed instead of numbers
	- All valid times are displayed in white
	- Various changes to the Summary screen
- _Stats tracking_ now ignores WADs without maps when creating data folders for _stats files_
	- This means that existing `stats.txt` files may require to be moved between `cherry_doom_data` subfolders for Cherry Doom to detect them
- The _Level Table_ is now inaccessible in multiplayer
- Invalid _stats files_ are now non-fatal
- The _Level Table_ now displays all loaded maps

### Bug Fixes

- Doom 1 maps from PWADs being listed on the _Level Table_ when loaded with the Doom 2 IWAD and vice versa (see Maps of Chaos)
- _Level Table_'s Summary screen counting unavailable maps towards the totals