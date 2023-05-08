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

In practice, **there are two types of elements in the Nugget HUD**. Both types use the following variables for each element:

- `_x`: **An integer for X position**. Can be any number between 0 and 320 (inclusive).
- `_y`: **An integer for Y position**. Can be any number between 0 and 200 (inclusive).
- `_wide`: **An integer to shift the element's X position when using the widescreen Nugget HUD**. Can be any number between -1 and 1, where a value of -1 will shift the element left, a value of 1 will shift it right, and a value of 0 won't shift it at all.

### Widgets

The following widgets are available:

```
nughud_ammo ----- Ammo count for the currently-equipped weapon
nughud_health --- Health count
nughud_arms# ---- Arms number, where # is a number between 2 and 9 (inclusive)
nughud_frags ---- Frags count, only shown during Deathmatch games
nughud_face ----- Face (Mugshot)
nughud_armor ---- Armor count
nughud_key# ----- Key display, where # is a number between 0 and 2 (in order: Blue Key; Yellow Key; Red Key)
nughud_ammo# ---- Ammo count for each type, where # is a number between 0 and 3 (in order: Bullets; Shells; Cells; Rockets)
nughud_maxammo# - Same as the above, but for Max Ammo
nughud_time ----- Time display, only shown if enabled by the user
nughud_sts ------ Stats (Kills/Items/Secrets) display, only shown if enabled by the user
nughud_title ---- Level Name display, only shown on the Automap
nughud_coord ---- Coordinates display, only shown if enabled by the user
nughud_fps ------ FPS display, only shown when the FPS cheat is activated
```

Widgets just use the variables listed above, with some exceptions:

- A boolean (value of 0 or 1) is used to toggle the _Face_ background, whose position is linked to that of the _Face_ itself;
- Another boolean is used to make the _Time_ display be relocated to the position of the _Stats_ display, in case the latter is inactive.

Additionally, **widgets also support an X position value of -1**.
For most widgets, said value will disable the widget. However, for the _Time_, _Stats_, _Level Name_, _Coordinates_ and _FPS_ displays, said value will instead make them be forcefully drawn at their default positions. Most of these widgets can still be disabled by in-game means.

**The _Coordinates_ and _FPS_ displays specifically are drawn aligned to the right** (i.e. they'll be drawn to the left of the specified X position value).

**Widgets example:**

```
; This is a comment!
; Loading a NUGHUD lump with these contents will restore the Face widget.

nughud_face_x 143

; Move the Frags widget elsewhere, since in the default NUGHUD distribution,
; it is drawn right where the Face is drawn in the traditional Status Bar.
nughud_frags_x 314
nughud_frags_y 155
nughud_frags_wide 1
```

### Patches

**Patches are static graphics that can be drawn anywhere on the screen**, behind the widgets.
Up to 8 patches can be drawn. They are drawn in increasing order (i.e. `patch1` is drawn below `patch2`, which is drawn below `patch3`, and so on).

**They make use of an additional string variable, `_name`, that determines the name of the graphic lump to be used**, which can either be a sprite (i.e. a lump between `S_START` and `S_END` markers, like `MEDIA0`) or a graphic (like `STBAR`). The name used in the `NUGHUD` lump MUST be enclosed between quotation marks.

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