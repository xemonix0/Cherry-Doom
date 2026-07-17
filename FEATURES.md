## Features

The build corresponding to this feature list is based on [Nugget Doom 5.1.0](https://github.com/MrAlaux/Nugget-Doom/releases/tag/nugget-doom-5.1.0).

Some of Cherry Doom's features originate from other sources; acknowledgements are provided _(ported from [p.f.] or inspired by [i.b.])_.

Config variables marked as "_CFG-only_" don't have a corresponding menu item and can only be changed by editing `cherry-doom.cfg`.

### General

- _Mute Inactive Window_ setting [p.f. International Doom]
- _Floating Powerups_ setting [p.f. International Doom]
- _Rocket Trails_ setting [partially p.f. Doom Retro]
    - Customize smoke particle spawn rate and translucency via CFG-only CVARs: `rocket_trails_interval` and `rocket_trails_tran_pct` respectively
- _Mouselook_ option for the _Stretch Short Skies_ setting, enabling sky stretching only with mouselook active
- _Less Blinding Tints_ setting (by [@Spaicrab](https://github.com/Spaicrab))
- _Dithered Lighting_ setting [i.b. Doom Retro]

#### Intermission screen

- Setting to adjust intermission kill percentage to meet UV max speedrun requirements (CFG-only: `inter_accurate_kill_count`)
    - Ensures that resurrected and Icon of Sin-spawned monsters do not increase the kill count. Maps without monsters display 100% kill completion
- Maps without items display 100% item completion

### Status Bar/HUD

#### Crosshair

- _Disable On Slot 1_ setting [i.b. [_Precise Crosshair_ mod](https://forum.zdoom.org/viewtopic.php?t=64788)]
- _Detection of Targets in Darkness_ setting, with required light level customizable via the CFG-only CVAR `hud_crosshair_dark_level` [i.b. [_Target Spy_ mod](https://forum.zdoom.org/viewtopic.php?t=60784)]

#### Messages

- _Pulsating Message Display_ setting [i.b. UZDoom]

### Weapons

- _Switch Speed_ setting [i.b. Nyan Doom]

### Enemies

- _Blood Amount Scales With Damage_ setting [i.b. Doom Retro]

### Miscellaneous

- Menu items, disabled due to active compatibility options, now include hints, explaining the reason for their unavailability
- _Disable Stats Tracking_ setting for the _Custom Skill_

### _Level Table_

The _Level Table_, inspired by DSDA-Doom, tracks individual level progress, including statistics like skill level, kills, items, secrets, and time, has a summary screen with total WAD statistics, and allows you to warp to any map without using cheats.

Compared to DSDA-Doom's implementation, there are a few notable additions and changes:
- Toggleable _stats tracking_ (CFG-only: `lt_enable_tracking`)
- Command line parameter to temporarily disable stats tracking (`-notracking`)
- Option to track kills and time for maps not beaten from a pistol start (CFG-only: `lt_track_continuous`)
    - Enabled by default; disabling replicates older (pre-2.0.0) behavior.
- Option to reset stats for the current level upon achieving a new best skill, excluding Nightmare (CFG-only: `lt_reset_on_higher_skill`)
    - Enabled by default; disabling replicates older behavior.
- Ability to view and warp to all loaded maps, not just those from the last-loaded WAD, grouped by WAD filename
- Stats tracking excludes WADs without maps when creating data folders for stats files
- _Level Table Stats Format_ setting (CFG-only: `lt_stats_format`)
- Press `Del` to erase selected map stats
- "Reset to defaults" button repurposed for erasing current WAD stats
- Various visual enhancements