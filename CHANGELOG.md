## New Features
* Implemented key to make the Automap marks blink (default key: <kbd>B</kbd>)
* Implemented an Automap color for unrevealed secret sectors
* Implemented a toggle to have the Gamma Correction key cycle through "new" gamma levels instead

## Changes
* Merged Woof 10.5.0 and Woof 10.5.1's changes, therefore:
   * The toggle to account for fuzzy targets in the crosshair now has its own variable
   * Removed "Force Default [Crosshair] Color" setting
   * Changed Level Brightness range from (-16, 16) to (-8, 8)
* Weapons no longer need ammo to be fired when the Infinite Ammo cheat is enabled
* Fixed a crash in Linux
* Cheats:
   * Replaced 'IDNLEV' / 'IDNEXT' with 'NEXTMAP'
   * Implemented 'NEXTSECRET' cheat
   * Removed 'SPAWN' alternative for 'SUMMON'
   * Removed 'SCANNER' and 'ANALYZE' alternatives for 'LINETARGET'
   * 'BOBBERS' now gives Full Ammo and Keys only when toggling on
* "New" Gamma Correction slider now ranges from 0.5 to 2.0 in steps of 0.05
* Changed "Horiz. Autoaim Indicators" description to "Horizontal Autoaim Indicators"