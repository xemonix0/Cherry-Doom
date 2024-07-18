## New Features

- _Mute Inactive Window_ setting
- _Floating Powerups_ setting

## Changes

- Merged changes from the following _Nugget Doom_ releases:
	- [2.2.1](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-2.2.1)
	- [2.3.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-2.3.0)
	- [2.3.1](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-2.3.1)
	- [3.0.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-3.0.0)
	- [3.1.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-3.1.0)
- Removed some features added in previous versions:
	- Cherry's _Extended HUD colors_, except colors for _Low/Ok/Good/Extra values_
	- _Motion Blur_
	- _Damage Shake Effect_, in favor of Nugget Doom's _Flinch Upon Damage_
	- _View Shake Intensity_ setting
- Introduced separate options for _Health + Armor_ and _Weapons_ widgets on the intermission screen
- Minor menu color changes:
	- _Selected setup menu tabs_: Yellow > Green
	- _Setup menu titles_: Yellow > Red
- Disable _Movement Widget_ in _strict mode_
- Added hints for disabled menu items, which explain why the item is disabled
- Made _stats tracking_ ignore WADs without maps when creating data folders for `stats.txt`[^1]

## Bug Fixes

- Attempts incrementing on demo playback
- Widgets disappearing from the intermission screen when exiting setup menus
- _Backdrop Darkening_ and _Overlay Darkening_ items not being toggled properly
- Attempts showing up as "0/0" on the level table for beaten levels with no attempt data (i.e. in data files from DSDA-Doom)
- Widgets overlapping on the intermission screen when _Nugget HUD_ is enabled
- Doom 1 maps from PWADs being listed on the level table when loaded with the Doom 2 IWAD and vice versa (e.g. Maps of Chaos)

[^1]: This means that existing `stats.txt` files may require to be moved between `cherry_doom_data` subfolders folders for Cherry Doom to detect them