- **_Save_ version has been changed to `Cherry 2.1.0`**

## New Features

- _Less Blinding Tints_ setting [thanks to @Spaicrab]

### Level table

- Press `Del` to _erase selected map stats_
- "Reset to defaults" button repurposed for _erasing current WAD stats_

## Nugget Doom merges

Merged changes from [Nugget Doom 3.3.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-3.3.0), note:
- Disabled stats tracking when _Duplicate Monsters_ is enabled in the _Custom Skill_ settings

## Changes

- _Blood amount scaling with the amount of damage dealt_ is now toggleable through a setting, and is disabled by default
- Minor improvements to rocket smoke physics have been introduced
- _Detection of Targets in Darkness_ now accounts for fullbright frames
- WAD stats are now saved every time a level is beaten
- `TNT31.WAD` is now not considered a separate WAD by the _stats tracking_ system when loaded with `TNT.WAD`

## Bug Fixes

- The _Disable Stats Tracking_ setting of the _Custom Skill_ not being preserved in savegames and through rewinds