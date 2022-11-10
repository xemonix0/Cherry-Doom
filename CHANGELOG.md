## New Features
* Implemented Crispy Doom's "Squat Weapon Down On Impact"
* Implemented Crispy Doom's drawing of Berserk in place of the Ammo count when the Fist is equipped
* Implemented crosshair lock-on
* Implemented horizontal autoaim indicators for crosshair
* Implemented toggle to force the default crosshair color when coloring based on target health without a target
* Implemented "Advance Internal Demos" setting
* 'IDNLEV' / 'IDNEXT' cheat, to end the level
* Implemented "Nugget HUD" support
* Implemented toggle to allow usage of chat in singleplayer

## Changes
* Merged Woof 10.4.0's changes, therefore:
   * Removed Nugget's "Disable [Menu] Background" toggle
   * Renamed 'a11y_palette_changes' to 'palette_changes'
   * Renamed 'a11y_extra_lighting' to 'extra_level_brightness'
* Restored "Weapon Recoil" menu item
* Restored "Message Listing Scrolls Upwards" toggle, and enabled it by default
* Offer selection of widescreen ratios in the setup menu itself
* 'rfov' is now calculated in R_ExecuteSetViewSize(), therefore:
   * Made it no longer necessary to reset the whole screen to change the FOV
   * Fixed a bug where 'rfov' was set before 'casual_play', which caused projection issues
* Fixed randomly mirrored deaths in finale cast
* Fixed 'MDK' and 'SPAWN' cheats crashes
* Added a check for 'crosshair.cr' being set before drawing the crosshair, fixing an occassional crash when toggling the crosshair on for the first time with the toggle key
* Added a check for 'psp->state' being set to the condition to call P_NuggetBobbing(), fixing a crash when the player becomes a "zombie"
* D_NuggetUpdateCasual() is now called in G_InitNew() and G_DoPlayDemo(), and it now checks for 'timingdemo'
* Due to implementation of Nugget HUD, removed hardcoded Crispy HUD variants
* Set default value of 'comp_iosdeath' to 0
* Added basic Linux install script (thanks @Zse00)
* Replaced placeholder built-in setup icon with actual setup icon
