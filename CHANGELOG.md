**_Save_ version has been changed to `Cherry 2.0.0`**
**_WAD stats_ version has been changed back to `1`**

## New Features

- _Mute Inactive Window_ setting
- _Floating Powerups_ setting

## Changes

- Changes from the following _Nugget Doom_ releases have been merged:
	- [2.2.1](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-2.2.1)
	- [2.3.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-2.3.0)
	- [2.3.1](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-2.3.1)
	- [3.0.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-3.0.0)
	- [3.1.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-3.1.0)
- Some features added in previous versions have been removed:
	- Cherry's _Extended HUD colors_, except colors for _Low/Ok/Good/Extra values_
	- _Motion Blur_
	- _Damage Shake Effect_, in favor of Nugget Doom's _Flinch Upon Damage_
	- _View Shake Intensity_ setting
- Separate options for _Health + Armor_ and _Weapons_ widgets on the intermission screen have been introduced
- Minor menu color changes:
	- _Selected setup menu tabs_: Yellow > Green
	- _Setup menu titles_: Yellow > Red
- The _Movement Widget_ is now disabled in _strict mode_
- Hints for disabled menu items, explaining the reason certain items are disabled, have been added

## Bug Fixes

- Widgets disappearing from the intermission screen when exiting setup menus
- _Backdrop Darkening_ and _Overlay Darkening_ items not being toggled properly
- Widgets overlapping on the intermission screen when _Nugget HUD_ is enabled

## Stats Tracking & Level Table

This release introduces many changes, fixes and some additions to the _Level Table_ and the _stats tracking system_, all of which are listed below.

### New Features

- CVAR to toggle _stats tracking_
- Command line parameter to _disable stats tracking_ (`-notracking`)
- _Level table stats format_ customization
- CVAR to toggle _tracking kills and time for maps beaten not from pistol start_

### Changes

- Removed _Attempt tracking_
- Some color and formatting changes have been made to the _Level Table_:
	- Skill names are displayed instead of numbers
	- All valid times are displayed in white
	- Various changes to the Summary screen
- _Stats tracking_ now ignores WADs without maps when creating data folders for _stats files_[^1]
- The _Level Table_ is now inaccessible in multiplayer
- Invalid _stats files_ are now non-fatal
	- The game doesn't crash if stats can't be parsed; instead, the _Level Table_ becomes inaccessible and _stats tracking_ is disabled)
- The _Level Table_ now displays all loaded maps, grouped by WAD filename
	- Just like before, stats are only tracked for the last WAD in the load order, but now the _Level Table_ can be used to warp to other available maps as well

### Bug Fixes

- Doom 1 maps from PWADs being listed on the _Level Table_ when loaded with the Doom 2 IWAD and vice versa (see Maps of Chaos)
- _Level Table_'s Summary screen counting unavailable maps towards the totals

[^1]: This means that existing `stats.txt` files may require to be moved between `cherry_doom_data` subfolders for Cherry Doom to detect them