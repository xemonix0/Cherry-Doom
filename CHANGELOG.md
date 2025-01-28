- **_Save_ version has been changed to `Cherry 2.1.0`**

## New Features

- _Less Blinding Tints_ setting by [@Spaicrab](https://github.com/Spaicrab)

### Level table

- Press `Del` to _erase selected map stats_
- The "Reset to defaults" button is repurposed for erasing current WAD stats

## Nugget Doom merges

Merged changes from the following Nugget Doom releases:
- [3.3.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-3.3.0):
	- Disabled stats tracking when _Duplicate Monsters_ is enabled in the _Custom Skill_ settings
- [4.0.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-4.0.0):
	- Removed the _Movement widget_, favoring Woof!'s _speedometer_ and _command history widget_
	- Removed _Low/Ok/Good/Extra value color customization_ and the _intermission screen widgets_ due to the removal of Health, Armor, Ammo, and Weapons widgets
- [4.1.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-4.1.0)

## Changes

- _Blood amount scaling with the amount of damage dealt_ is now toggleable through a setting and is disabled by default
- Introduced minor improvements to rocket smoke physics
- _Detection of Targets in Darkness_ now accounts for fullbright frames
- WAD stats are now written to the file every time a level is beaten
- `TNT31.WAD` is no longer considered a separate WAD by the stats tracking system when loaded with `TNT.WAD`
- _Rocket Trails_ now work even when DSDHacked patches are loaded
- Removed menu items for the _Rocket Trails Interval_ and _Smoke Translucency_ settings
- Times on the level table now display hours when exceeding 60 minutes