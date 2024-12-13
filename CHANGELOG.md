- **_Save_ version has been changed to `Cherry 2.1.0`**

## New Features

- _Less Blinding Tints_ setting [thanks to @Spaicrab]

### Level table

- Press `Del` to _erase selected map stats_
- "Reset to defaults" button repurposed for _erasing current WAD stats_

## Nugget Doom merges

Merged changes from the following Nugget Doom releases:
- [Nugget Doom 3.3.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-3.3.0), note:
	- Disabled stats tracking when _Duplicate Monsters_ is enabled in the _Custom Skill_ settings
- [Nugget Doom 4.0.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-4.0.0), note:
	- Removed the _Movement widget_, in favor of Woof!'s speedometer and command history widget
	- Removed _Low/Ok/Good/Extra value color customization_ and the _intermission screen widgets_ settings, since Health, Armor, Ammo and Weapons widgets have been removed

## Changes

- _Blood amount scaling with the amount of damage dealt_ is now toggleable through a setting, and is disabled by default
- Minor improvements to rocket smoke physics have been introduced
- _Detection of Targets in Darkness_ now accounts for fullbright frames
- WAD stats are now saved every time a level is beaten
- `TNT31.WAD` is now not considered a separate WAD by the _stats tracking_ system when loaded with `TNT.WAD`
- _Rocket Trails_ now work even when there are DSDHacked patches loaded
- Menu items for the _Rocket Trails Interval_ and _Smoke Translucency_ settings have been removed
- Times on the level table now display hours when over 60 minutes

## Bug Fixes

None.