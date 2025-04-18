- **_Save_ version has been changed to `Cherry 2.1.0`**

## New Features

- _Less Blinding Tints_ setting by [@Spaicrab](https://github.com/Spaicrab)
- Ways to erase stats for the selected map (via the `Del` key) and the whole WAD (via the reset button) from the level table screen

## Nugget Doom merges

Merged changes from the following Nugget Doom releases:
- [3.3.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-3.3.0):
	- Stats tracking is disabled when _Duplicate Monsters_ is active
- [4.0.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-4.0.0)
- [4.1.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-4.1.0)
- [4.2.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-4.2.0)

## Changes

- _Blood amount scaling with the amount of damage dealt_ is now toggleable through a setting and is disabled by default
- Introduced minor improvements to rocket smoke physics
- _Detection of Targets in Darkness_ now accounts for fullbright frames
- WAD stats are now written to the file every time a level is beaten
- `TNT31.WAD` is no longer considered a separate WAD by the stats tracking system when loaded with `TNT.WAD`
- _Rocket Trails_ now work even when DSDHacked patches are loaded
- Removed menu items for the _Rocket Trails Interval_ and _Smoke Translucency_ settings
- Times on the level table now display hours when exceeding 60 minutes
- Renamed the `rocket_trails_tran` CVAR to `rocket_trails_tran_pct` for consistency [^1]

### Regressions

- Removed the _Movement widget_, favoring Woof!'s _speedometer_ and _command history widget_
- Removed _Low/Ok/Good/Extra value color customization_ and the _intermission screen widgets_ due to the removal of Health, Armor, Ammo, and Weapons widgets
- Saves from Woof! 14.0.0 and later, as well as Nugget Doom 2.1.0 and later, are no longer supported

[^1]: This will affect existing config files.