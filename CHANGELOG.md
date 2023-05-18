## New Features

- **_"Fade"_ screen wipe**
- _**Teleporter Zoom**_ setting
- _**"Use" Button Timer**_ setting
- _**Teleport Timer**_ setting
- _**Key Pickup Timer**_ setting
- **NUGHUD:**
  - Now supports custom fonts for most Status Bar widgets
  - Now allows to relocate the _Level Name_, _Coordinates_, _FPS_, _Messages_ and _Secret Message_ text lines
  - Now allows to specify an alignment for supported text lines
  - Now supports widescreen shifting values to force said shift
  - Now supports Arms number 1
  - Reverted X and Y position maximums back to 320 and 200 respectively, and maximum weapon height back to 200

## Changes

- **Merged Woof 11.0.0 to 11.1.1's changes**, therefore:
  - _'FREEZE'_ cheat now has only one mode
  - Changed default positions for _Time_ and _Stats_ displays in NUGHUD
- **FOV is now changed gradually** in most cases
- **Reduced turning/freelook sensitivity when zoomed in**
- **Weapons are now lowered when zooming in**
- **Automap position isn't reset to player position when opening it with Follow Mode off**
- **Adjusted _"Seizure"_ screen wipe's speed** to match _"Melt"_ and _"Fade"_
- **FOV changes are now disabled in Strict Mode only**
- **Viewheight changes are now disabled in Strict Mode only**
- **Extended functionality for the _'SUMMON'_ cheat**
- **Removed menu item for _"Alternative Arms Display"_**
- **Alternative `A_FireCGun()` and `A_CPosAttack()` sounds are only looked up once**
- **The "Nugget Settings" category is now split** across pages 5 and 6
- **Renamed _"Imitate player's breathing"_ to _"Subtle Idle Bobbing/Breathing"_**
- **Forced `comp_choppers` OFF outside of Casual Play**
- **Strict Mode update:**
  - Disabled usage of `gamma2`
  - Forced `no_menu_tint` OFF
  - Forced `s_clipping_dist_x2` OFF
  - Forced `a11y_weapon_flash` ON
  - Forced `a11y_weapon_pspr` ON
  - Forced `a11y_invul_colormap` ON
  - Forced `bobbing_style` to 0 (Vanilla)
  - Forced `weaponsquat` OFF
  - Forced `sx_fix` OFF
  - Disabled usage of `nughud.weapheight`
  - Forced `smooth_counts` OFF
  - Disabled usage of `mapcolor_uscr`
  - Forced `fuzzdark_mode` OFF
  - Forced `comp_blazing2` ON
  - Forced `comp_manualdoor` ON
  - Forced `comp_switchsource` OFF
  - Forced `comp_cgundblsnd` ON
  - Forced `comp_cgunnersfx` OFF
  - Forced `comp_flamst` OFF
  - Forced `comp_deadoff` ON
  - Forced `comp_keypal` ON
  - Forced `sp_chat` OFF
  - Disabled usage of `DSCHGUN`

## Bug Fixes

- **Fixed some FOV-related sky rendering issues**
- **Made _"Seizure"_ screen wipe's speed consistent** in low and high resolution
- **Fixed some cheat activation messages**
- **Zoom FOV is now mostly unaffected by changes to normal FOV, and vice versa**
- **Fixed Face widget not being drawn in NUGHUD if `nughud.face.x = 0`**
- **Fixed not-found** (surely non-existent) **NUGHUD and Berserk/Medkit graphics being looked up constantly**
- **In Strict Mode:**
  - Fixed `damagecount`
  - Enabled `view_bobbing_percentage` menu item
  - Fixed `player->viewheight` capping
  - Enabled `center_weapon` menu item
