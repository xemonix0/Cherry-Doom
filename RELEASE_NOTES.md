This version is up to date with [_Nugget Doom 6.0.1_](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-6.0.1) and [_Woof! 16.0.0_](https://github.com/fabiangreffrath/woof/releases/tag/woof_16.0.0).

## Added

- _Pulsating Message Display_ setting
- _[Weapon] Switch Speed_ setting

## Changed

- **Merged changes from Nugget Doom releases [6.0.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-6.0.0) and [6.0.1](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-6.0.1)**, note:
  - Removed `mute_inactive` in favor of Woof!'s `mute_unfocused` [^1]
- Reverted all rearrangements made to Nugget Doom's menu items and moved Cherry Doom's display options into a separate tab
- The level table now fits 16 rows on the screen instead of 15
- The level table now displays partial totals on the summary page
- _Detection of Targets in Darkness_ now accounts for weapon flashes, the light amplification visor, the invulnerability colormap and the _Extra Lighting_ setting
- Added a scrollbar to the level table, replacing the barely noticeable arrows

## Fixed

- Multiple issues with the dark backdrop fade-in/out animations
- Demo/coop desyncs related to intermission screen fixes introduced in 2.0.0

[^1]: Affects existing config files.