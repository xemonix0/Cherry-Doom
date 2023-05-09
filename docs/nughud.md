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

## NUGHUD elements

In practice, **there are three types of elements in the Nugget HUD**. The following variables are shared across all types:

- `_x`: **An integer for X position**. Can be any number between 0 and 320 (inclusive), and also -1 in some cases.
- `_y`: **An integer for Y position**. Can be any number between 0 and 200 (inclusive).
- `_wide`: **An integer to shift the element's X position when using the widescreen Nugget HUD**. Can be any number between -1 and 1, where a value of -1 will shift the element left, a value of 1 will shift it right, and a value of 0 won't shift it at all.

### Widgets

The following widgets are available:

```
nughud_ammo ----- Ammo count for the currently-equipped weapon
nughud_health --- Health count
nughud_arms# ---- Arms (weapon) number, where # is a number between 2 and 9 (inclusive)
nughud_frags ---- Frags count, only shown during Deathmatch games
nughud_face ----- Face (Mugshot)
nughud_armor ---- Armor count
nughud_key# ----- Key display, where # is a number between 0 and 2 (in order: Blue Key; Yellow Key; Red Key)
nughud_ammo# ---- Ammo count for each type, where # is a number between 0 and 3 (in order: Bullets; Shells; Cells; Rockets)
nughud_maxammo# - Same as the above, but for Max Ammo
```

**Widgets support an X position value of -1 to disable the widget.**

Aside from the shared variables, there is an additional boolean variable (value of 0 or 1), `nughud_face_bg`, that toggles the _Face_ background, whose position is linked to that of the _Face_ itself.

**Widgets example:**

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

**`NUGHUD` supports custom fonts for all numbered widgets.**
Graphics for all characters of a given font must be provided for the font to be used, else the Vanilla font will be used.

The following fonts are available:

```
Tall Numbers, used for the Health, Armor and current-weapon Ammo counts:

  NHTNUM# -- Number, where # is a number between 0 and 9 (inclusive)
  NHTMINUS - Minus sign
  NHTPRCNT - Percent sign

Ammo Numbers, used for the Ammo and Max Ammo counts:

  NHAMNUM# - Number, where # is a number between 0 and 9 (inclusive)

Arms Numbers, used for the weapon numbers:

  NHW0NUM# - Weapon unavailable, where # is a number between 1 and 9 (inclusive)
  NHW1NUM# - Weapon available, where # is a number between 1 and 9 (inclusive)
```

### Text Lines

The following text lines are available:

```
nughud_time -- Time display, only shown if enabled by the user
nughud_sts --- Stats (Kills/Items/Secrets) display, only shown if enabled by the user
nughud_title - Level Name display, only shown on the Automap
nughud_coord - Coordinates display, only shown if enabled by the user
nughud_fps --- FPS display, only shown when the FPS cheat is activated
```

**Text lines support an X position value of -1 to forcefully draw the text line at its default X position.** Most of these widgets can be disabled by in-game means.

Aside from the shared variables, **text lines have an additional integer variable, `_align`, that sets the text line's alignment**: -1 will align it left, 0 will center it and 1 will align it right.
There is also an additional boolean variable, `nughud_time_sts`, that makes the _Time_ display be relocated to the position of the _Stats_ display, in case the latter is inactive.

**Text lines example:**

```
; Loading a NUGHUD lump with these contents will draw the Level Name display centered.

nughud_title_x     160
nughud_title_wide  0
nughud_title_align 0
```

### Patches

**Patches are static graphics that can be drawn anywhere on the screen**, behind the widgets.
Up to 8 patches can be drawn. They are drawn in increasing order (i.e. `patch1` is drawn below `patch2`, which is drawn below `patch3`, and so on).

Aside from the shared variables, **patches have an additional string variable, `_name`, that determines the name of the graphic lump to be used**, which can either be a sprite (i.e. a lump between `S_START` and `S_END` markers, like `MEDIA0`) or a graphic (like `STBAR`).
The names used in the `NUGHUD` lump MUST be enclosed between quotation marks.

Patches do NOT support an X position value of -1. They can be disabled simply by not providing any graphic.

**Patches example:**

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

**Custom lumps CAN be used** (for example, a graphic called `NEWPATCH`).

### Miscellaneous

**There's an additional fixed-point variable, `nughud_weapheight`, to increase the height at which weapon sprites are drawn**.
It can be any value between 0 and 200 (inclusive).

---

By default and compared to the original Crispy HUD, the Nugget HUD hides the face widget and shows 8 Arms numbers instead of 6.

**The default distribution for the Nugget HUD**, as defined in the executable, **is available in the form of `nughud.lmp`**, found in the `docs/` folder. Comments were added to it for clarity. Feel free to use it as a base to make new distributions.

It is advised that you do not include values for variables that you do not wish to modify, as to avoid issues if the handling of any of them is altered in the future.