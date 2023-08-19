# Nugget Doom's NUGHUD guide

**The `NUGHUD` lump** is a variant of MBF's `OPTIONS` lump, **used specifically by Nugget Doom to customize the Crispy HUD**, which we will refer to as _Nugget HUD_ from now on.

As implied, `NUGHUD` uses the same format as `OPTIONS`. Excerpt from `mbfedit.txt`:

```
The OPTIONS lump has the same format as mbf.cfg: A text file listing option
names and values, optionally separated by blank or comment lines.
```

## Loading NUGHUD lumps

**`NUGHUD` lumps can be loaded just like `OPTIONS` lumps**:

- By loading WADs which contain a lump explicitly named `NUGHUD`;
- By including a file explicitly named `nughud.lmp` (case-insensitive) in the corresponding autoload folder;
- By using the `-file` command-line parameter to load a file explicitly named `nughud.#`, where `#` can be any extension name (`.lmp` is recommended).

## NUGHUD widgets

**The Nugget HUD is composed of widgets**, whose behavior is determined by a set of properties.

The following properties are shared across all widgets:

- `_x`: **X position**, which can be any number between 0 and 320 (inclusive).
- `_y`: **Y position**, which can be any number between 0 and 200 (inclusive).
- `_wide`: **Widescreen shift**, with the following possible values:
  - -2 to shift the element left forcefully;
  - -1 to shift the element left only when using the widescreen Nugget HUD;
  - 0 to keep the element in place regardless of widescreen;
  - 1 to shift the element right only when using the widescreen Nugget HUD;
  - 2 to shift the element right forcefully.

The following types of widgets support special behavior:

- **Disableables**: Can be disabled by setting `_x` to -1.
- **Alignables**: Can be aligned by means of the `_align` property, with the following possible values:
  - -1 for left alignment;
  - 0 for centered alignment;
  - 1 for right alignment.

The following widgets are available:

```
nughud_ammo ------ Ammo count for the currently-equipped weapon
nughud_health ---- Health count
nughud_arms# ----- Arms (weapon) number, where # is a number between 1 and 9 (inclusive)
nughud_frags ----- Frags count, only shown during Deathmatch games
nughud_face ------ Face (Mugshot)
nughud_armor ----- Armor count
nughud_armoricon - Armor icon, which changes depending on armor type (requires NHARMOR font - see below)
nughud_key# ------ Key display, where # is a number between 0 and 2 (in order: Blue Key; Yellow Key; Red Key)
nughud_ammo# ----- Ammo count for each type, where # is a number between 0 and 3 (in order: Bullets; Shells; Cells; Rockets)
nughud_maxammo# -- Same as the above, but for Max Ammo
nughud_time ------ Time display, only shown if enabled by the user
nughud_sts ------- Stats (Kills/Items/Secrets) display, only shown if enabled by the user
nughud_title ----- Level Name display, only shown on the Automap
nughud_powers ---- Powerup timers, only shown if enabled by the user
nughud_coord ----- Coordinates display, only shown if enabled by the user
nughud_fps ------- FPS display, only shown when the FPS cheat is activated
nughud_message --- Message display
nughud_secret ---- Secret Message display
```

Their properties are as follows:

| Widget           | Disableable | Alignable |
| :--------------: | :---------: | :-------: |
| nughud_ammo      | Yes         | Yes       |
| nughud_health    | Yes         | Yes       |
| nughud_arms#     | Yes         | No        |
| nughud_frags     | Yes         | Yes       |
| nughud_face      | Yes         | No        |
| nughud_armor     | Yes         | Yes       |
| nughud_armoricon | Yes         | No        |
| nughud_key#      | Yes         | No        |
| nughud_ammo#     | Yes         | Yes       |
| nughud_maxammo#  | Yes         | Yes       |
| nughud_time      | No          | Yes       |
| nughud_sts       | No          | Yes       |
| nughud_title     | No          | Yes       |
| nughud_powers    | No          | Yes       |
| nughud_coord     | No          | Yes       |
| nughud_fps       | No          | Yes       |
| nughud_message   | No          | Yes       |
| nughud_secret    | No          | Yes       |

There are some additional boolean properties (value of 0 or 1) for some specific widgets:

- `nughud_face_bg`: Toggle the _Face_ background, whose position is linked to that of the _Face_ itself.
- `nughud_percents`: Toggle drawing of percentage signs for the Health and Armor counts.
- `nughud_time_sts`: Toggle relocation of the _Time_ widget to the position of the _Stats_ widget when the latter is inactive.

Lastly, the _Message_ widget supports an X position value of -1 to forcefully draw it at its original X position, where it'll be affected by the Centered Messages setting.

#### Examples

```
; This is a comment!
; Loading a NUGHUD lump with these contents will restore the Face widget.

nughud_face_x 143

; Move the Frags widget elsewhere, since in the default NUGHUD distribution,
; it is drawn right where the Face is drawn in the traditional Status Bar.
nughud_frags_x    314
nughud_frags_y    155
nughud_frags_wide 1
```

```
; Loading a NUGHUD lump with these contents will draw the Level Name display centered.

nughud_title_x     160
nughud_title_wide  0
nughud_title_align 0
```

### Custom fonts

**`NUGHUD` supports custom fonts for certain widgets.**
Graphics for all characters of a given font must be provided for the font to be used, otherwise the Vanilla font will be used instead.

The following fonts are available:

```
Tall Numbers, used for the Health, Armor, current-weapon Ammo and Frags counts:

  NHTNUM# -- Number, where # is a number between 0 and 9 (inclusive)
  NHTMINUS - Minus sign
  NHTPRCNT - Percent sign

Current-weapon Ammo Numbers, used exclusively for the current-weapon Ammo count, taking precedence over the Tall Numbers:

  NHRNUM# -- Number, where # is a number between 0 and 9 (inclusive)
  NHRMINUS - Minus sign

Ammo Numbers, used for the Ammo and Max Ammo counts:

  NHAMNUM# - Number, where # is a number between 0 and 9 (inclusive)

Arms Numbers, used for the weapon numbers:

  NHW0NUM# - Weapon unavailable, where # is a number between 1 and 9 (inclusive)
  NHW1NUM# - Weapon available, where # is a number between 1 and 9 (inclusive)

Keys:

  NHKEYS# -- Key, where # is a number between 0 and 8 (inclusive)

Berserk, drawn in place of the Ammo count when using the Berserk Fist:

  NHBERSRK - Berserk graphic

Armor graphics, used for the Armor icon widget:

  NHARMOR# - Graphic, where # is either 0 (no armor), 1 (green armor) or 2 (blue armor)

Infinity, drawn in place of the Ammo count when using weapons with no ammo type (e.g. Fist/Chainsaw):

  NHINFNTY - Infinity graphic
```

### Patches

**Patches are static graphics that can be drawn anywhere on the screen**, behind the rest of widgets.
Up to 8 patches can be drawn. They are drawn in increasing order; `patch1` is drawn below `patch2`, which is drawn below `patch3`, and so on.

Aside from the shared properties, **patches make use of an additional property, `_name`, that determines the name of the graphic lump to be used**, which can either be a sprite (i.e. a lump between `S_START` and `S_END` markers, like `MEDIA0`) or a graphic (like `STBAR`).
**Custom lumps CAN be used** (for example, a graphic called `NEWPATCH`). The names used in the `NUGHUD` lump MUST be enclosed between quotation marks.

Patches are alignable, and can be disabled by simply not providing any graphic.

#### Example

```
; Loading a NUGHUD lump with these contents will draw
; the Status Bar graphic in its traditional position.
; It doesn't look right, but it's just an example.

nughud_patch1_x 0
nughud_patch1_y 168
nughud_patch1_wide 0
nughud_patch1_name "STBAR"

; Also draw the Arms box, which is actually a separate graphic.
nughud_patch2_x 104
nughud_patch2_y 168
nughud_patch2_wide 0
nughud_patch2_name "STARMS"
```

### Miscellaneous

**There is an additional fixed-point property, `nughud_weapheight`, to increase the height at which weapon sprites are drawn**.
It can be any value between 0 and 200 (inclusive).

---

By default and compared to the original Crispy HUD, the Nugget HUD hides the face widget and shows Arms numbers 2-9 instead of 2-7.

**The default distribution for the Nugget HUD**, as defined in the executable, **is available in text format as `nughud.lmp`**, found in the `docs/` folder. Comments were added to it for clarity. Feel free to use it as a base to make new distributions.

It is advised that you do not include values for variables that you do not wish to modify, as to avoid issues if the handling of any of them is altered in the future.
