//
//  Copyright (C) 1999 by
//  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//
// DESCRIPTION:
//      Status bar code.
//      Does the face/direction indicator animatin.
//      Does palette indicators as well (red pain/berserk, bright pickup)
//
//-----------------------------------------------------------------------------

#include <stdlib.h>

#include "am_map.h"
#include "d_event.h"
#include "d_items.h"
#include "d_player.h"
#include "doomdef.h"
#include "doomstat.h"
#include "doomtype.h"
#include "hu_command.h"
#include "hu_obituary.h"
#include "i_system.h"
#include "i_video.h"
#include "info.h"
#include "m_array.h"
#include "m_cheat.h"
#include "m_config.h"
#include "m_misc.h"
#include "m_random.h"
#include "m_swap.h"
#include "p_mobj.h"
#include "p_user.h"
#include "hu_crosshair.h"
#include "r_data.h"
#include "r_defs.h"
#include "r_draw.h"
#include "r_main.h"
#include "r_state.h"
#include "s_sound.h"
#include "st_stuff.h"
#include "st_sbardef.h"
#include "st_widgets.h"
#include "tables.h"
#include "v_fmt.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"

// [Nugget]
#include "m_nughud.h"

// [Nugget] /=================================================================

static patch_t *stbersrk;
static int lu_berserk;

static patch_t *stinfnty;

static int NughudWideShift(const int wide);
static void LoadNuggetGraphics(void);

// CVARs ---------------------------------------------------------------------

boolean no_menu_tint;
boolean no_berserk_tint;
boolean no_radsuit_tint;
boolean comp_godface;
boolean comp_unusedpals;
static boolean sts_show_berserk;

typedef enum hudtype_s
{
  HUDTYPE_SBARDEF,
  HUDTYPE_NUGHUD
} hudtype_t;

static hudtype_t hud_type;

// Font extras ---------------------------------------------------------------

#define HU_FONTEXTRAS 7

static patch_t *font_extras[HU_FONTEXTRAS] = { NULL };

sbarelem_t *health_elem = NULL;
sbarelem_t *armor_elem = NULL;
sbarelem_t *ammo_elem = NULL;

// NUGHUD --------------------------------------------------------------------

static int nughud_patchlump[NUMNUGHUDPATCHES];

static patch_t *nhbersrk;   // NHBERSRK
static patch_t *nhammo[4];  // NHAMMO#, from 0 to 3
static patch_t *nhambar[2]; // NHAMBAR#, from 0 to 1
static patch_t *nhealth[2]; // NHEALTH#, from 0 to 1
static patch_t *nhhlbar[2]; // NHHLBAR#, from 0 to 1
static patch_t *nharmor[3]; // NHARMOR#, from 0 to 2
static patch_t *nharbar[2]; // NHARBAR#, from 0 to 1
static patch_t *nhinfnty;   // NHINFNTY

static boolean st_nughud;

boolean ST_GetNughudOn(void)
{
  return st_nughud;
}

sbarelem_t *message_elem = NULL;
sbarelem_t *chat_elem = NULL;

static pixel_t *st_bar = NULL; // Used for Status-Bar chunks

static sbaralignment_t NughudConvertAlignment(const int wide, const int align);
static void DrawNughudGraphics();
static sbardef_t *CreateNughudSbarDef(void);

// NUGHUD stacks -------------------------------------------------------------

#define NUMSQWIDGETS (sbw_num) // Number of widgets

typedef struct widgetpair_s {
  const nughud_textline_t *ntl;
  sbarelem_t *elem;
  int order;
} widgetpair_t;

typedef struct stackqueue_s {
  int offset;
  widgetpair_t pairs[NUMSQWIDGETS];
} stackqueue_t;

static stackqueue_t nughud_stackqueues[NUMNUGHUDSTACKS];

static boolean NughudAddToStack(
  const nughud_textline_t *const ntl,
  sbarelem_t *const elem,
  const int stack
);

static int NughudSortWidgets(const void *_p1, const void *_p2);

// [Nugget] =================================================================/

//
// STATUS BAR DATA
//

// Palette indices.
// For damage/bonus red-/gold-shifts
#define STARTREDPALS            1
#define STARTBONUSPALS          9
#define NUMREDPALS              8
#define NUMBONUSPALS            4
// Radiation suit, green shift.
#define RADIATIONPAL            13

// Number of status faces.
#define ST_NUMPAINFACES         5
#define ST_NUMSTRAIGHTFACES     3
#define ST_NUMTURNFACES         2
#define ST_NUMSPECIALFACES      3

#define ST_FACESTRIDE \
          (ST_NUMSTRAIGHTFACES+ST_NUMTURNFACES+ST_NUMSPECIALFACES)

#define ST_NUMEXTRAFACES        2
#define ST_NUMXDTHFACES         9

#define ST_NUMFACES \
    (ST_FACESTRIDE * ST_NUMPAINFACES + ST_NUMEXTRAFACES + ST_NUMXDTHFACES)

#define ST_TURNOFFSET        (ST_NUMSTRAIGHTFACES)
#define ST_OUCHOFFSET        (ST_TURNOFFSET + ST_NUMTURNFACES)
#define ST_EVILGRINOFFSET    (ST_OUCHOFFSET + 1)
#define ST_RAMPAGEOFFSET     (ST_EVILGRINOFFSET + 1)
#define ST_GODFACE           (ST_NUMPAINFACES * ST_FACESTRIDE)
#define ST_DEADFACE          (ST_GODFACE + 1)
#define ST_XDTHFACE          (ST_DEADFACE + 1)

#define ST_EVILGRINCOUNT     (2 * TICRATE)
#define ST_STRAIGHTFACECOUNT (TICRATE / 2)
#define ST_TURNCOUNT         (1 * TICRATE)
#define ST_OUCHCOUNT         (1 * TICRATE)
#define ST_RAMPAGEDELAY      (2 * TICRATE)

#define ST_MUCHPAIN          20

// graphics are drawn to a backing screen and blitted to the real screen
static pixel_t *st_backing_screen = NULL;

// [Alaux]
static boolean hud_animated_counts;

static boolean sts_colored_numbers;

static boolean sts_pct_always_gray;

//jff 2/16/98 status color change levels
static int ammo_red;      // ammo percent less than which status is red
static int ammo_yellow;   // ammo percent less is yellow more green
int health_red;           // health amount less than which status is red
int health_yellow;        // health amount less than which status is yellow
int health_green;         // health amount above is blue, below is green
static int armor_red;     // armor amount less than which status is red
static int armor_yellow;  // armor amount less than which status is yellow
static int armor_green;   // armor amount above is blue, below is green

static boolean hud_armor_type; // color of armor depends on type

// used for evil grin
static boolean  oldweaponsowned[NUMWEAPONS];

// [crispy] blinking key or skull in the status bar
int st_keyorskull[3];

static sbardef_t *sbardef;

static statusbar_t *statusbar;

typedef enum
{
    st_original,
    st_wide
} st_layout_t;

static st_layout_t st_layout;

static patch_t **facepatches = NULL;

static int have_xdthfaces;

//
// STATUS BAR CODE
//

static patch_t *CachePatchName(const char *name)
{
    int lumpnum = W_CheckNumForName(name);
    if (lumpnum < 0)
    {
        lumpnum = (W_CheckNumForName)(name, ns_sprites);
        if (lumpnum < 0)
        {
            return NULL;
        }
    }
    return V_CachePatchNum(lumpnum, PU_STATIC);
}

static void LoadFacePatches(void)
{
    char lump[9] = {0};

    int painface;

    for (painface = 0; painface < ST_NUMPAINFACES; ++painface)
    {
        for (int straightface = 0; straightface < ST_NUMSTRAIGHTFACES;
             ++straightface)
        {
            M_snprintf(lump, sizeof(lump), "STFST%d%d", painface, straightface);
            array_push(facepatches, V_CachePatchName(lump, PU_STATIC));
        }

        M_snprintf(lump, sizeof(lump), "STFTR%d0", painface); // turn right
        array_push(facepatches, V_CachePatchName(lump, PU_STATIC));

        M_snprintf(lump, sizeof(lump), "STFTL%d0", painface); // turn left
        array_push(facepatches, V_CachePatchName(lump, PU_STATIC));

        M_snprintf(lump, sizeof(lump), "STFOUCH%d", painface); // ouch!
        array_push(facepatches, V_CachePatchName(lump, PU_STATIC));

        M_snprintf(lump, sizeof(lump), "STFEVL%d", painface); // evil grin ;)
        array_push(facepatches, V_CachePatchName(lump, PU_STATIC));

        M_snprintf(lump, sizeof(lump), "STFKILL%d", painface); // pissed off
        array_push(facepatches, V_CachePatchName(lump, PU_STATIC));
    }

    M_snprintf(lump, sizeof(lump), "STFGOD0");
    array_push(facepatches, V_CachePatchName(lump, PU_STATIC));

    M_snprintf(lump, sizeof(lump), "STFDEAD0");
    array_push(facepatches, V_CachePatchName(lump, PU_STATIC));

    // [FG] support face gib animations as in the 3DO/Jaguar/PSX ports
    for (painface = 0; painface < ST_NUMXDTHFACES; ++painface)
    {
        M_snprintf(lump, sizeof(lump), "STFXDTH%d", painface);

        if (W_CheckNumForName(lump) != -1)
        {
            array_push(facepatches, V_CachePatchName(lump, PU_STATIC));
        }
        else
        {
            break;
        }
    }
    have_xdthfaces = painface;
}

static boolean CheckWidgetState(widgetstate_t state)
{
    if ((state == HUD_WIDGET_AUTOMAP && automapactive == AM_FULL)
        || (state == HUD_WIDGET_HUD && automapactive != AM_FULL)
        || (state == HUD_WIDGET_ALWAYS))
    {
        return true;
    }

    return false;
}

static boolean CheckConditions(sbarcondition_t *conditions, player_t *player)
{
    boolean result = true;
    int currsessiontype = netgame ? MIN(deathmatch + 1, 2) : 0;
    // TODO
    // boolean compacthud = frame_width < frame_adjusted_width;

    sbarcondition_t *cond;
    array_foreach(cond, conditions)
    {
        switch (cond->condition)
        {
            case sbc_weaponowned:
                if (cond->param >= 0 && cond->param < NUMWEAPONS)
                {
                    result &= !!player->weaponowned[cond->param];
                }
                break;

            case sbc_weaponselected:
                result &= player->readyweapon == cond->param;
                break;

            case sbc_weaponnotselected:
                result &= player->readyweapon != cond->param;
                break;

            case sbc_weaponhasammo:
                if (cond->param >= 0 && cond->param < NUMWEAPONS)
                {
                    result &= weaponinfo[cond->param].ammo != am_noammo;
                }
                break;

            case sbc_selectedweaponhasammo:
                result &= weaponinfo[player->readyweapon].ammo != am_noammo;
                break;

            case sbc_selectedweaponammotype:
                result &=
                    weaponinfo[player->readyweapon].ammo == cond->param;
                break;

            case sbc_weaponslotowned:
                {
                    boolean owned = false;
                    for (int i = 0; i < NUMWEAPONS; ++i)
                    {
                        if (weaponinfo[i].slot == cond->param
                            && player->weaponowned[i])
                        {
                            owned = true;
                            break;
                        }
                    }
                    result &= owned;
                }
                break;

            case sbc_weaponslotnotowned:
                {
                    boolean notowned = true;
                    for (int i = 0; i < NUMWEAPONS; ++i)
                    {
                        if (weaponinfo[i].slot == cond->param
                            && player->weaponowned[i])
                        {
                            notowned = false;
                            break;
                        }
                    }
                    result &= notowned;
                }
                break;

            case sbc_weaponslotselected:
                result &= weaponinfo[player->readyweapon].slot == cond->param;
                break;

            case sbc_weaponslotnotselected:
                result &= weaponinfo[player->readyweapon].slot != cond->param;
                break;

            case sbc_itemowned:
                result &=
                    !!P_EvaluateItemOwned((itemtype_t)cond->param, player);
                break;

            case sbc_itemnotowned:
                result &=
                    !P_EvaluateItemOwned((itemtype_t)cond->param, player);
                break;

            case sbc_featurelevelgreaterequal:
                // ignore
                break;

            case sbc_featurelevelless:
                // ignore
                break;

            case sbc_sessiontypeeequal:
                result &= currsessiontype == cond->param;
                break;

            case sbc_sessiontypenotequal:
                result &= currsessiontype != cond->param;
                break;

            case sbc_modeeequal:
                result &= gamemode == (cond->param + 1);
                break;

            case sbc_modenotequal:
                result &= gamemode != (cond->param + 1);
                break;

            case sbc_hudmodeequal:
                // TODO
                // result &= ( !!cond->param == compacthud );
                result &= (!!cond->param == false);
                break;

            case sbc_widgetmode:
                {
                    int enabled = 0;
                    if (cond->param & sbc_mode_overlay)
                    {
                        enabled |= (automapactive == AM_FULL && automapoverlay);
                    }
                    else if (cond->param & sbc_mode_automap)
                    {
                        enabled |= (automapactive == AM_FULL && !automapoverlay);
                    }
                    if (cond->param & sbc_mode_hud)
                    {
                        enabled |= automapactive != AM_FULL;
                    }
                    result &= (enabled > 0);
                }
                break;

            case sbc_widgetenabled:
                {
                    widgetstate_t state = HUD_WIDGET_OFF;
                    switch ((sbarwidgettype_t)cond->param)
                    {
                        case sbw_monsec:
                            state = hud_level_stats;
                            break;
                        case sbw_time:
                            state = hud_level_time;
                            break;
                        case sbw_coord:
                            state = hud_player_coords;
                            break;
                        default:
                            break;
                    }
                    result &= CheckWidgetState(state);
                }
                break;

            case sbc_widgetdisabled:
                {
                    widgetstate_t state = HUD_WIDGET_OFF;
                    switch ((sbarwidgettype_t)cond->param)
                    {
                        case sbw_monsec:
                            state = hud_level_stats;
                            break;
                        case sbw_time:
                            state = hud_level_time;
                            break;
                        case sbw_coord:
                            state = hud_player_coords;
                            break;
                        default:
                            break;
                    }
                    result &= !CheckWidgetState(state);
                }
                break;

            // [Nugget] NUGHUD
            case sbc_weaponnotowned:
                if (st_nughud && cond->param >= 0 && cond->param < NUMWEAPONS)
                {
                    result &= !player->weaponowned[cond->param];
                }
                break;

            case sbc_none:
            default:
                result = false;
                break;
        }
    }

    return result;
}

// [Alaux]
static int SmoothCount(int shownval, int realval)
{
    int step = realval - shownval;

    // [Nugget] Disallowed in Strict Mode
    if (strictmode || !hud_animated_counts || !step)
    {
        return realval;
    }
    else
    {
        int sign = step / abs(step);
        step = BETWEEN(1, 7, abs(step) / 20);
        shownval += (step + 1) * sign;

        if ((sign > 0 && shownval > realval)
            || (sign < 0 && shownval < realval))
        {
            shownval = realval;
        }

        return shownval;
    }
}

static int ResolveNumber(sbe_number_t *number, player_t *player)
{
    int result = 0;
    int param = number->param;

    switch (number->type)
    {
        case sbn_health:
            if (number->oldvalue == -1)
            {
                number->oldvalue = player->health;
            }
            result = SmoothCount(number->oldvalue, player->health);
            number->oldvalue = result;
            break;

        case sbn_armor:
            if (number->oldvalue == -1)
            {
                number->oldvalue = player->armorpoints;
            }
            result = SmoothCount(number->oldvalue, player->armorpoints);
            number->oldvalue = result;
            break;

        case sbn_frags:
            for (int p = 0; p < MAXPLAYERS; ++p)
            {
                result += player->frags[p];
            }
            break;

        case sbn_ammo:
            if (param >= 0 && param < NUMAMMO)
            {
                result = player->ammo[param];
            }
            break;

        case sbn_ammoselected:
            result = player->ammo[weaponinfo[player->readyweapon].ammo];
            break;

        case sbn_maxammo:
            if (param >= 0 && param < NUMAMMO)
            {
                result = player->maxammo[param];
            }
            break;

        case sbn_weaponammo:
            if (param >= 0 && param < NUMWEAPONS)
            {
                result = player->ammo[weaponinfo[param].ammo];
            }
            break;

        case sbn_weaponmaxammo:
            if (param >= 0 && param < NUMWEAPONS)
            {
                result = player->maxammo[weaponinfo[param].ammo];
            }
            break;

        case sbn_none:
        default:
            break;
    }

    return result;
}

static int CalcPainOffset(sbe_face_t *face, player_t *player)
{
    int health = player->health > 100 ? 100 : player->health;
    int lasthealthcalc =
        ST_FACESTRIDE * (((100 - health) * ST_NUMPAINFACES) / 101);
    face->oldhealth = health;
    return lasthealthcalc;
}

static int DeadFace(player_t *player)
{
    const int state =
        (player->mo->state - states) - mobjinfo[player->mo->type].xdeathstate;

    // [FG] support face gib animations as in the 3DO/Jaguar/PSX ports
    if (have_xdthfaces && state >= 0)
    {
        return ST_XDTHFACE + MIN(state, have_xdthfaces - 1);
    }

    return ST_DEADFACE;
}

static void UpdateFace(sbe_face_t *face, player_t *player)
{
    static int priority;
    static int lastattackdown = -1;

    // [Nugget]
    const boolean invul = (player->cheats & CF_GODMODE) || player->powers[pw_invulnerability];

    if (priority < 10)
    {
        // dead
        if (!player->health || STRICTMODE(comp_godface && invul)) // [Nugget]
        {
            priority = 9;
            face->faceindex = player->health ? ST_GODFACE : DeadFace(player);
            face->facecount = 1;
        }
    }

    if (priority < 9)
    {
        if (player->bonuscount)
        {
            // picking up bonus
            boolean doevilgrin = false;

            for (int i = 0; i < NUMWEAPONS; ++i)
            {
                if (oldweaponsowned[i] != player->weaponowned[i])
                {
                    doevilgrin = true;
                    oldweaponsowned[i] = player->weaponowned[i];
                }
            }

            if (doevilgrin)
            {
                // evil grin if just picked up weapon
                priority = 8;
                face->facecount = ST_EVILGRINCOUNT;
                face->faceindex = CalcPainOffset(face, player) + ST_EVILGRINOFFSET;
            }
        }
    }

    if (priority < 8)
    {
        if (player->damagecount && player->attacker
            && player->attacker != player->mo)
        {
            // being attacked
            priority = 7;

            angle_t diffangle = 0;
            boolean right = false;

            // [FG] show "Ouch Face" as intended
            if (player->health - face->oldhealth > ST_MUCHPAIN)
            {
                // [FG] raise "Ouch Face" priority
                priority = 8;
                face->facecount = ST_TURNCOUNT;
                face->faceindex = CalcPainOffset(face, player) + ST_OUCHOFFSET;
            }
            else
            {
                angle_t badguyangle =
                    R_PointToAngle2(player->mo->x, player->mo->y,
                                     player->attacker->x, player->attacker->y);

                if (badguyangle > player->mo->angle)
                {
                    // whether right or left
                    diffangle = badguyangle - player->mo->angle;
                    right = diffangle > ANG180;
                }
                else
                {
                    // whether left or right
                    diffangle = player->mo->angle - badguyangle;
                    right = diffangle <= ANG180;
                } // confusing, aint it?

                face->facecount = ST_TURNCOUNT;
                face->faceindex = CalcPainOffset(face, player);

                if (diffangle < ANG45)
                {
                    // head-on
                    face->faceindex += ST_RAMPAGEOFFSET;
                }
                else if (right ^ STRICTMODE(flip_levels)) // [Nugget] Flip levels
                {
                    // turn face right
                    face->faceindex += ST_TURNOFFSET;
                }
                else
                {
                    // turn face left
                    face->faceindex += ST_TURNOFFSET + 1;
                }
            }
        }
    }

    if (priority < 7)
    {
        // getting hurt because of your own damn stupidity
        if (player->damagecount)
        {
            if (player->health - face->oldhealth > ST_MUCHPAIN)
            {
                priority = 7;
                face->facecount = ST_TURNCOUNT;
                face->faceindex = CalcPainOffset(face, player) + ST_OUCHOFFSET;
            }
            else
            {
                priority = 6;
                face->facecount = ST_TURNCOUNT;
                face->faceindex = CalcPainOffset(face, player) + ST_RAMPAGEOFFSET;
            }
        }
    }

    if (priority < 6)
    {
        // rapid firing
        if (player->attackdown)
        {
            if (lastattackdown == -1)
            {
                lastattackdown = ST_RAMPAGEDELAY;
            }
            else if (!--lastattackdown)
            {
                priority = 5;
                face->faceindex = CalcPainOffset(face, player) + ST_RAMPAGEOFFSET;
                face->facecount = 1;
                lastattackdown = 1;
            }
        }
        else
        {
            lastattackdown = -1;
        }
    }

    if (priority < 5)
    {
        // invulnerability
        if (invul && !STRICTMODE(comp_godface)) // [Nugget]
        {
            priority = 4;
            face->faceindex = ST_GODFACE;
            face->facecount = 1;
        }
    }

    // look left or look right if the facecount has timed out
    if (!face->facecount)
    {
        face->faceindex = CalcPainOffset(face, player) + (M_Random() % 3);
        face->facecount = ST_STRAIGHTFACECOUNT;
        priority = 0;
    }

    --face->facecount;
}

static void UpdateNumber(sbarelem_t *elem, player_t *player)
{
    sbe_number_t *number = elem->subtype.number;

    int value = ResolveNumber(number, player);
    int power = (value < 0 ? number->maxlength - 1 : number->maxlength);
    int max = (int)pow(10.0, power) - 1;
    int valglyphs = 0;
    int numvalues = 0;

    numberfont_t *font = number->font;
    if (font == NULL)
    {
        array_foreach(font, sbardef->numberfonts)
        {
            if (!strcmp(font->name, number->font_name))
            {
                break;
            }
        }
    }

    if (value < 0 && font->minus != NULL)
    {
        value = MAX(-max, value);
        numvalues = (int)log10(-value) + 1;
        valglyphs = numvalues + 1;
    }
    else
    {
        value = BETWEEN(0, max, value);
        numvalues = valglyphs = value != 0 ? ((int)log10(value) + 1) : 1;
    }

    if (elem->type == sbe_percent && font->percent != NULL
        && (!st_nughud || (nughud.percents && elem->alignment & sbe_h_middle))) // [Nugget] NUGHUD
    {
        ++valglyphs;
    }

    int totalwidth = font->monowidth * valglyphs;
    if (font->type == sbf_proportional)
    {
        totalwidth = 0;
        if (value < 0 && font->minus != NULL)
        {
            totalwidth += SHORT(font->minus->width);
        }
        int tempnum = value;
        while (tempnum > 0)
        {
            int workingnum = tempnum % 10;
            totalwidth = SHORT(font->numbers[workingnum]->width);
            tempnum /= 10;
        }
        if (elem->type == sbe_percent && font->percent != NULL)
        {
            totalwidth += SHORT(font->percent->width);
        }
    }

    number->xoffset = 0;
    if (elem->alignment & sbe_h_middle)
    {
        number->xoffset -= (totalwidth >> 1);
    }
    else if (elem->alignment & sbe_h_right)
    {
        number->xoffset -= totalwidth;
    }

    number->font = font;
    number->value = value;
    number->numvalues = numvalues;
}

static void UpdateLines(sbarelem_t *elem)
{
    sbe_widget_t *widget = elem->subtype.widget;

    hudfont_t *font = widget->font;
    if (font == NULL)
    {
        array_foreach(font, sbardef->hudfonts)
        {
            if (!strcmp(font->name, widget->font_name))
            {
                break;
            }
        }
    }

    widgetline_t *line;
    array_foreach(line, widget->lines)
    {
        int totalwidth = 0;

        const char *str = line->string;
        while (*str)
        {
            int ch = *str++;
            if (ch == '\x1b' && *str)
            {
                ++str;
                continue;
            }

            if (font->type == sbf_proportional)
            {
                ch = (unsigned char) M_ToUpper(ch) - HU_FONTSTART; // [Nugget] Cast to unsigned
                if (ch < 0 || ch >= HU_FONTSIZE + HU_FONTEXTRAS) // [Nugget]
                {
                    totalwidth += SPACEWIDTH;
                    continue;
                }
                patch_t *patch = (ch >= HU_FONTSIZE) // [Nugget]
                                 ? font_extras[ch - HU_FONTSIZE]
                                 : font->characters[ch];

                if (patch == NULL)
                {
                    totalwidth += SPACEWIDTH;
                    continue;
                }
                totalwidth += SHORT(patch->width);
            }
            else
            {
                totalwidth += font->monowidth;
            }
        }

        line->xoffset = 0;
        if (elem->alignment & sbe_h_middle)
        {
            line->xoffset -= (totalwidth >> 1);
        }
        else if (elem->alignment & sbe_h_right)
        {
            line->xoffset -= totalwidth;
        }
        line->totalwidth = totalwidth;
    }

    widget->font = font;
}

static void UpdateAnimation(sbarelem_t *elem)
{
    sbe_animation_t *animation = elem->subtype.animation;

    if (animation->duration_left == 0)
    {
        ++animation->frame_index;
        if (animation->frame_index == array_size(animation->frames))
        {
            animation->frame_index = 0;
        }
        animation->duration_left = animation->frames[animation->frame_index].duration;
    }

    --animation->duration_left;
}

static void UpdateBoomColors(sbarelem_t *elem, player_t *player)
{
    if (!sts_colored_numbers)
    {
        elem->crboom = CR_NONE;
        return;
    }

    sbe_number_t *number = elem->subtype.number;

    boolean invul = (player->powers[pw_invulnerability]
                     || player->cheats & CF_GODMODE);

    crange_idx_e cr;

    switch (number->type)
    {
        case sbn_health:
            {
                int health = player->health;
                if (invul)
                    cr = CR_GRAY;
                else if (health < health_red)
                    cr = CR_RED;
                else if (health < health_yellow)
                    cr = CR_GOLD;
                else if (health <= health_green)
                    cr = CR_GREEN;
                else
                    cr = CR_BLUE2;
            }
            break;
        case sbn_armor:
            if (hud_armor_type)
            {
                // [Nugget] Make it gray *only* if the player is in God Mode
                if (player->cheats & CF_GODMODE)
                    cr = CR_GRAY;
                else if (!player->armortype)
                    cr = CR_RED;
                else if (player->armortype == 1)
                    cr = CR_GREEN;
                else
                    cr = CR_BLUE2;
            }
            else
            {
                int armor = player->armorpoints;
                // [Nugget] Make it gray *only* if the player is in God Mode
                if (player->cheats & CF_GODMODE)
                    cr = CR_GRAY;
                else if (armor < armor_red)
                    cr = CR_RED;
                else if (armor < armor_yellow)
                    cr = CR_GOLD;
                else if (armor <= armor_green)
                    cr = CR_GREEN;
                else
                    cr = CR_BLUE2;
            }
            break;
        case sbn_ammoselected:
            {
                ammotype_t type = weaponinfo[player->readyweapon].ammo;
                if (type == am_noammo)
                {
                    return;
                }

                int maxammo = player->maxammo[type];
                if (maxammo == 0)
                {
                    return;
                }

                // [Nugget] Make it gray if the player has infinite ammo
                if (player->cheats & CF_INFAMMO)
                {
                    cr = CR_GRAY;
                    break;
                }

                int ammo = player->ammo[type];

                // backpack changes thresholds
                if (player->backpack)
                {
                    maxammo /= 2;
                }

                if (ammo * 100 < ammo_red * maxammo)
                    cr = CR_RED;
                else if (ammo * 100 < ammo_yellow * maxammo)
                    cr = CR_GOLD;
                else if (ammo > maxammo)
                    cr = CR_BLUE2;
                else
                    cr = CR_GREEN;
            }
            break;
        default:
            cr = CR_NONE;
            break;
    }

    elem->crboom = cr;
}

static void UpdateElem(sbarelem_t *elem, player_t *player)
{
    if (!CheckConditions(elem->conditions, player))
    {
        return;
    }

    switch (elem->type)
    {
        case sbe_face:
            UpdateFace(elem->subtype.face, player);
            break;

        case sbe_animation:
            UpdateAnimation(elem);
            break;

        case sbe_number:
        case sbe_percent:
            UpdateBoomColors(elem, player);
            UpdateNumber(elem, player);
            break;

        case sbe_widget:
            ST_UpdateWidget(elem, player);
            UpdateLines(elem);
            break;

        default:
            break;
    }

    sbarelem_t *child;
    array_foreach(child, elem->children)
    {
        UpdateElem(child, player);
    }
}

static void UpdateStatusBar(player_t *player)
{
    int barindex = MAX(screenblocks - 10, 0);

    if (automapactive == AM_FULL && automapoverlay == AM_OVERLAY_OFF)
    {
        barindex = 0;
    }

    if (st_nughud) { barindex = 0; } // [Nugget] NUGHUD

    statusbar = &sbardef->statusbars[barindex];

    health_elem = armor_elem = ammo_elem = NULL; // [Nugget]

    sbarelem_t *child;
    array_foreach(child, statusbar->children)
    {
        if (child->type == sbe_number || child->type == sbe_percent)
        {
            switch (child->subtype.number->type)
            {
                case sbn_ammoselected:  ammo_elem   = child;  break;
                case sbn_health:        health_elem = child;  break;
                case sbn_armor:         armor_elem  = child;  break;
                default:                                      break;
            }
        }

        UpdateElem(child, player);
    }
}

static void ResetElem(sbarelem_t *elem)
{
    switch (elem->type)
    {
        case sbe_graphic:
            {
                sbe_graphic_t *graphic = elem->subtype.graphic;
                graphic->patch = CachePatchName(graphic->patch_name);
            }
            break;

        case sbe_face:
            {
                sbe_face_t *face = elem->subtype.face;
                face->faceindex = 0;
                face->facecount = 0;
                face->oldhealth = -1;
            }
            break;

        case sbe_animation:
            {
                sbe_animation_t *animation = elem->subtype.animation;
                sbarframe_t *frame;
                array_foreach(frame, animation->frames)
                {
                    frame->patch = CachePatchName(frame->patch_name);
                }
                animation->frame_index = 0;
                animation->duration_left = 0;
            }
            break;

        case sbe_number:
        case sbe_percent:
            elem->subtype.number->oldvalue = -1;
            break;

        default:
            break;
    }

    sbarelem_t *child;
    array_foreach(child, elem->children)
    {
        ResetElem(child);
    }
}

static void ResetStatusBar(void)
{
    statusbar_t *local_statusbar;
    array_foreach(local_statusbar, sbardef->statusbars)
    {
        sbarelem_t *child;
        array_foreach(child, local_statusbar->children)
        {
            ResetElem(child);
        }
    }

    ST_ResetTitle();
}

static boolean draw_shadow = false; // [Nugget] HUD/menu shadows

// [Nugget] Extended to accept two colorings
static void DrawPatchEx(int x, int y, int maxheight, sbaralignment_t alignment,
                        patch_t *patch, crange_idx_e cr, crange_idx_e cr2, byte *tl)
{
    if (!patch)
    {
        return;
    }

    int width = SHORT(patch->width);
    int height = maxheight ? maxheight : SHORT(patch->height);

    if (alignment & sbe_h_middle)
    {
        x -= (width >> 1);
    }
    else if (alignment & sbe_h_right)
    {
        x -= width;
    }

    if (alignment & sbe_v_middle)
    {
        y -= (height >> 1);
    }
    else if (alignment & sbe_v_bottom)
    {
        y -= height;
    }

    if (st_layout == st_wide
        || (st_nughud && alignment & sbe_wide_force)) // [Nugget] NUGHUD
    {
        if (alignment & sbe_wide_left)
        {
            x -= video.deltaw;
        }
        if (alignment & sbe_wide_right)
        {
            x += video.deltaw;
        }
    }

    byte *outr = colrngs[cr];
    byte *outr2 = colrngs[cr2]; // [Nugget]

    // [Nugget] HUD/menu shadows

    V_ToggleShadows(draw_shadow);

    if (outr && outr2)
    {
        if (tl)
        {
            V_DrawPatchTRTRTLSH(x, y, patch, outr, outr2, tl);
        }
        else
        {
            V_DrawPatchTRTRSH(x, y, patch, outr, outr2);
        }
    }
    else if (outr || outr2)
    {
        if (tl)
        {
            V_DrawPatchTRTLSH(x, y, patch, outr2 ? outr2 : outr, tl);
        }
        else
        {
            V_DrawPatchTranslatedSH(x, y, patch, outr2 ? outr2 : outr);
        }
    }
    else if (tl)
    {
        V_DrawPatchTLSH(x, y, patch, tl);
    }
    else
    {
        V_DrawPatchTranslatedSH(x, y, patch, outr2 ? outr2 : outr);
    }

    V_ToggleShadows(true);
}

// [Nugget]
#define DrawPatch(x, y, mh, a, p, cr, tl) \
  DrawPatchEx(x, y, mh, a, p, cr, CR_NONE, tl)

static void DrawGlyphNumber(int x, int y, sbarelem_t *elem, patch_t *glyph)
{
    sbe_number_t *number = elem->subtype.number;
    numberfont_t *font = number->font;

    int width, widthdiff;

    if (font->type == sbf_proportional)
    {
        width = glyph ? SHORT(glyph->width) : SPACEWIDTH;
        widthdiff = 0;
    }
    else
    {
        width = font->monowidth;
        widthdiff = glyph ? SHORT(glyph->width) - width : SPACEWIDTH - width;
    }

    if (elem->alignment & sbe_h_middle)
    {
        number->xoffset += ((width + widthdiff) >> 1);
    }
    else if (elem->alignment & sbe_h_right)
    {
        number->xoffset += (width + widthdiff);
    }

    if (glyph)
    {
        DrawPatch(x + number->xoffset, y, font->maxheight, elem->alignment,
                  glyph, elem->crboom == CR_NONE ? elem->cr : elem->crboom,
                  elem->tranmap);
    }

    if (elem->alignment & sbe_h_middle)
    {
        number->xoffset += (width - ((width - widthdiff) >> 1));
    }
    else if (elem->alignment & sbe_h_right)
    {
        number->xoffset += -widthdiff;
    }
    else
    {
        number->xoffset += width;
    }
}

static void DrawGlyphLine(int x, int y, sbarelem_t *elem, widgetline_t *line,
                          patch_t *glyph)
{
    sbe_widget_t *widget = elem->subtype.widget;
    hudfont_t *font = widget->font;

    int width, widthdiff;

    if (font->type == sbf_proportional)
    {
        width = glyph ? SHORT(glyph->width) : SPACEWIDTH;
        widthdiff = 0;
    }
    else
    {
        width = font->monowidth;
        widthdiff = glyph ? SHORT(glyph->width) - width : 0;
    }

    if (elem->alignment & sbe_h_middle)
    {
        line->xoffset += ((width + widthdiff) >> 1);
    }
    else if (elem->alignment & sbe_h_right)
    {
        line->xoffset += (width + widthdiff);
    }

    if (glyph)
    {
        // [Nugget] HUD/menu shadows: don't draw for digits
        draw_shadow = hud_menu_shadows && strcmp(widget->font_name, "Digits");

        // [Nugget] Message flash
        DrawPatchEx(x + line->xoffset, y, font->maxheight, elem->alignment, glyph,
                    elem->cr, line->flash ? CR_BRIGHT : CR_NONE, elem->tranmap);

        draw_shadow = false;
    }

    if (elem->alignment & sbe_h_middle)
    {
        line->xoffset += (width - ((width - widthdiff) >> 1));
    }
    else if (elem->alignment & sbe_h_right)
    {
        line->xoffset += -widthdiff;
    }
    else
    {
        line->xoffset += width;
    }
}

static void DrawNumber(int x, int y, sbarelem_t *elem)
{
    sbe_number_t *number = elem->subtype.number;

    int value = number->value;
    int base_xoffset = number->xoffset;
    numberfont_t *font = number->font;

    if (value < 0 && font->minus != NULL)
    {
        DrawGlyphNumber(x, y, elem, font->minus);
        value = -value;
    }

    int glyphindex = number->numvalues;
    while (glyphindex > 0)
    {
        int glyphbase = (int)pow(10.0, --glyphindex);
        int workingnum = value / glyphbase;
        DrawGlyphNumber(x, y, elem, font->numbers[workingnum]);
        value -= (workingnum * glyphbase);
    }

    if (elem->type == sbe_percent && font->percent != NULL
        && (!st_nughud || nughud.percents)) // [Nugget] NUGHUD
    {
        crange_idx_e oldcr = elem->crboom;
        if (sts_pct_always_gray)
        {
            elem->crboom = CR_GRAY;
        }
        DrawGlyphNumber(x, y, elem, font->percent);
        elem->crboom = oldcr;
    }

    number->xoffset = base_xoffset;
}

static void DrawLines(int x, int y, sbarelem_t *elem)
{
    sbe_widget_t *widget = elem->subtype.widget;

    int cr = elem->cr;

    widgetline_t *line;
    array_foreach(line, widget->lines)
    {
        int base_xoffset = line->xoffset;
        hudfont_t *font = widget->font;

        const char *str = line->string;
        while (*str)
        {
            int ch = *str++;

            if (ch == '\x1b' && *str)
            {
                ch = *str++;
                if (ch >= '0' && ch <= '0' + CR_NONE)
                {
                    elem->cr = ch - '0';
                }
                else if (ch == '0' + CR_ORIG)
                {
                    elem->cr = cr;
                }
                continue;
            }

            ch = (unsigned char) M_ToUpper(ch) - HU_FONTSTART; // [Nugget] Cast to unsigned

            patch_t *glyph;
            if (ch < 0 || ch >= HU_FONTSIZE + HU_FONTEXTRAS) // [Nugget]
            {
                glyph = NULL;
            }

            // [Nugget]
            else if (ch >= HU_FONTSIZE)
            {
                glyph = font_extras[ch - HU_FONTSIZE];
            }

            else
            {
                glyph = font->characters[ch];
            }
            DrawGlyphLine(x, y, elem, line, glyph);
        }

        if (elem->alignment & sbe_v_bottom)
        {
            y -= font->maxheight;
        }
        else
        {
            y += font->maxheight;
        }

        line->xoffset = base_xoffset;
    }
}

static void DrawElem(int x, int y, sbarelem_t *elem, player_t *player)
{
    if (!CheckConditions(elem->conditions, player))
    {
        return;
    }

    x += elem->x_pos;
    y += elem->y_pos;

    switch (elem->type)
    {
        case sbe_graphic:
            {
                sbe_graphic_t *graphic = elem->subtype.graphic;
                DrawPatch(x, y, 0, elem->alignment, graphic->patch, elem->cr,
                          elem->tranmap);
            }
            break;

        case sbe_face:
            {
                sbe_face_t *face = elem->subtype.face;
                DrawPatch(x, y, 0, elem->alignment,
                          facepatches[face->faceindex], elem->cr,
                          elem->tranmap);
            }
            break;

        case sbe_animation:
            {
                sbe_animation_t *animation = elem->subtype.animation;
                patch_t *patch =
                    animation->frames[animation->frame_index].patch;
                DrawPatch(x, y, 0, elem->alignment, patch, elem->cr,
                          elem->tranmap);
            }
            break;

        case sbe_number:
        case sbe_percent:
            DrawNumber(x, y, elem);
            break;

        case sbe_widget:
            DrawLines(x, y, elem);
            break;

        default:
            break;
    }

    sbarelem_t *child;
    array_foreach(child, elem->children)
    {
        DrawElem(x, y, child, player);
    }
}

static boolean st_solidbackground;

static void DrawSolidBackground(void)
{
    // [FG] calculate average color of the 16px left and right of the status bar
    const int vstep[][2] = { {0, 1}, {1, 2}, {2, ST_HEIGHT} };

    patch_t *sbar = V_CachePatchName("STBAR", PU_CACHE);
    // [FG] temporarily draw status bar to background buffer
    V_DrawPatch(-video.deltaw, 0, sbar);

    byte *pal = W_CacheLumpName("PLAYPAL", PU_CACHE);

    const int width = MIN(SHORT(sbar->width), video.unscaledw);
    const int depth = 16;
    int v;

    // [FG] separate colors for the top rows
    for (v = 0; v < arrlen(vstep); v++)
    {
        int x, y;
        const int v0 = vstep[v][0], v1 = vstep[v][1];
        unsigned r = 0, g = 0, b = 0;
        byte col;

        for (y = v0; y < v1; y++)
        {
            for (x = 0; x < depth; x++)
            {
                byte *c = st_backing_screen + V_ScaleY(y) * video.pitch
                          + V_ScaleX(x);
                r += pal[3 * c[0] + 0];
                g += pal[3 * c[0] + 1];
                b += pal[3 * c[0] + 2];

                c += V_ScaleX(width - 2 * x - 1);
                r += pal[3 * c[0] + 0];
                g += pal[3 * c[0] + 1];
                b += pal[3 * c[0] + 2];
            }
        }

        r /= 2 * depth * (v1 - v0);
        g /= 2 * depth * (v1 - v0);
        b /= 2 * depth * (v1 - v0);

        // [FG] tune down to half saturation (for empiric reasons)
        col = I_GetNearestColor(pal, r / 2, g / 2, b / 2);

        V_FillRect(0, v0, video.unscaledw, v1 - v0, col);
    }
}

static boolean st_refresh_background = true; // [Nugget] Static

static void DrawBackground(const char *name)
{
    if (st_refresh_background)
    {
        V_UseBuffer(st_backing_screen);

        if (st_solidbackground)
        {
            DrawSolidBackground();
        }
        else
        {
            if (!name)
            {
                name = (gamemode == commercial) ? "GRNROCK" : "FLOOR7_2";
            }

            byte *flat =
                V_CacheFlatNum(firstflat + R_FlatNumForName(name), PU_CACHE);

            V_TileBlock64(ST_Y, video.unscaledw, ST_HEIGHT, flat);

            if (screenblocks == 10)
            {
                patch_t *patch = V_CachePatchName("brdr_b", PU_CACHE);
                for (int x = 0; x < video.unscaledw; x += 8)
                {
                    V_DrawPatch(x - video.deltaw, 0, patch);
                }
            }
        }

        V_RestoreBuffer();

        st_refresh_background = false;
    }

    V_CopyRect(0, 0, st_backing_screen, video.unscaledw, ST_HEIGHT, 0, ST_Y);
}

static void DrawStatusBar(void)
{
    player_t *player = &players[displayplayer];

    if (!statusbar->fullscreenrender)
    {
        DrawBackground(statusbar->fillflat);
    }

    DrawNughudGraphics(); // [Nugget] NUGHUD

    sbarelem_t *child;
    array_foreach(child, statusbar->children)
    {
        DrawElem(0, SCREENHEIGHT - statusbar->height, child, player);
    }

    // [Nugget] In case of `am_noammo`
    if (ammo_elem && !CheckConditions(ammo_elem->conditions, player))
    {
        int wide = 0;

        if (ammo_elem->alignment & sbe_wide_left)  { wide--; }
        if (ammo_elem->alignment & sbe_wide_right) { wide++; }

        wide *= 1 + !!(ammo_elem->alignment & sbe_wide_force);

        const int ammox = ammo_elem->x_pos + NughudWideShift(wide),
                  ammoy = ammo_elem->y_pos;

        // [Nugget]: [crispy] draw berserk pack instead of no ammo if appropriate
        if (sts_show_berserk && player->readyweapon == wp_fist && player->powers[pw_strength])
        {
            // NUGHUD Berserk
            if (st_nughud && nhbersrk)
            {
                V_DrawPatch(ammox, ammoy, nhbersrk);
            }
            // Status Bar Berserk
            else if (stbersrk)
            {
                V_DrawPatch(ammox, ammoy, stbersrk);
            }
            // Berserk or Medkit sprite
            else if (lu_berserk >= 0)
            {
                patch_t *const patch = V_CachePatchNum(lu_berserk, PU_STATIC);
                
                V_DrawPatch(
                    ammox - (21 * ((ammo_elem->alignment & sbe_h_mask) - 1))
                    - SHORT(patch->width)/2 + SHORT(patch->leftoffset),
                    ammoy + 8 - SHORT(patch->height)/2 + SHORT(patch->topoffset),
                    patch
                );
            }
        }
        // NUGHUD Infinity
        else if (st_nughud && nhinfnty)
        {
            V_DrawPatch(ammox, ammoy, nhinfnty);
        }
        // Status Bar Infinity
        else if (stinfnty)
        {
            V_DrawPatch(ammox, ammoy, stinfnty);
        }
    }
}

static void EraseElem(int x, int y, sbarelem_t *elem, player_t *player)
{
    if (!CheckConditions(elem->conditions, player))
    {
        return;
    }

    x += elem->x_pos;
    y += elem->y_pos;

    if (elem->type == sbe_widget)
    {
        sbe_widget_t *widget = elem->subtype.widget;
        hudfont_t *font = widget->font;

        int height = 0;
        widgetline_t *line;
        array_foreach(line, widget->lines)
        {
            if (elem->alignment & sbe_v_bottom)
            {
                y -= font->maxheight;
            }
            height += font->maxheight;
        }

        if (y > scaledviewy && y < scaledviewy + scaledviewheight - height)
        {
            R_VideoErase(0, y, scaledviewx, height);
            R_VideoErase(scaledviewx + scaledviewwidth, y, scaledviewx, height);
        }
        else
        {
            R_VideoErase(0, y, video.unscaledw, height);
        }
    }

    sbarelem_t *child;
    array_foreach(child, elem->children)
    {
        EraseElem(x, y, child, player);
    }
}

void ST_Erase(void)
{
    if (!sbardef)
    {
        return;
    }

    player_t *player = &players[displayplayer];

    sbarelem_t *child;
    array_foreach(child, statusbar->children)
    {
        EraseElem(0, SCREENHEIGHT - statusbar->height, child, player);
    }
}

// Respond to keyboard input events,
//  intercept cheats.
boolean ST_Responder(event_t *ev)
{
    // Filter automap on/off.
    if (ev->type == ev_keyup && (ev->data1.i & 0xffff0000) == AM_MSGHEADER)
    {
        return false;
    }
    else if (ST_MessagesResponder(ev))
    {
        return true;
    }
    else // if a user keypress...
    {
        return M_CheatResponder(ev); // Try cheat responder in m_cheat.c
    }
}

boolean palette_changes = true;

static void DoPaletteStuff(player_t *player)
{
    static int oldpalette = 0;
    int palette;

    static boolean old_less_blinding_tints = false; // [Cherry]

    int damagecount = player->damagecount;

    // killough 7/14/98: beta version did not cause red berserk palette
    if (!beta_emulation)
    {

        if (player->powers[pw_strength])
        {
            // slowly fade the berzerk out
            int berzerkcount = 12 - (player->powers[pw_strength] >> 6);
            if (berzerkcount > damagecount && !STRICTMODE(no_berserk_tint)) // [Nugget]
            {
                damagecount = berzerkcount;
            }
        }
    }

    // [Nugget] Disable palette tint in menus
    if (STRICTMODE(!palette_changes || (no_menu_tint && menuactive)))
    {
        palette = 0;
    }
    else if (damagecount)
    {
        // In Chex Quest, the player never sees red. Instead, the radiation suit
        // palette is used to tint the screen green, as though the player is
        // being covered in goo by an attacking flemoid.
        if (gameversion == exe_chex)
        {
            palette = RADIATIONPAL;
        }
        else
        {
            palette = (damagecount + 7) >> 3;
            if (palette >= NUMREDPALS)
            {
                palette = NUMREDPALS - 1 + STRICTMODE(comp_unusedpals); // [Nugget]
            }
            // [crispy] tune down a bit so the menu remains legible
            if (menuactive || paused)
            {
                palette >>= 1;
            }
            palette += STARTREDPALS - STRICTMODE(comp_unusedpals); // [Nugget]
        }
    }
    else if (player->bonuscount)
    {
        palette = (player->bonuscount + 7) >> 3;
        if (palette >= NUMBONUSPALS)
        {
            palette = NUMBONUSPALS - 1 + STRICTMODE(comp_unusedpals); // [Nugget]
        }
        palette += STARTBONUSPALS - STRICTMODE(comp_unusedpals); // [Nugget]
    }
    // killough 7/14/98: beta version did not cause green palette
    else if (beta_emulation)
    {
        palette = 0;
    }
    else if (POWER_RUNOUT(player->powers[pw_ironfeet])
             && !STRICTMODE(no_radsuit_tint)) // [Nugget]
    {
        palette = RADIATIONPAL;
    }
    else
    {
        palette = 0;
    }

    if (palette != oldpalette
        || old_less_blinding_tints != less_blinding_tints) // [Cherry]
    {
        old_less_blinding_tints = less_blinding_tints;
        oldpalette = palette;
        // haleyjd: must cast to byte *, arith. on void pointer is
        // a GNU C extension
        byte *pal =
            STRICTMODE(less_blinding_tints) // [Cherry] Less Blinding Tints
                ? (byte *)(alttintpal + palette * 768)
                : (byte *)W_CacheLumpNum("PLAYPAL", PU_CACHE) + palette * 768;

        I_SetPalette(pal);
    }
}

void ST_Ticker(void)
{
    // [Nugget] NUGHUD /------------------------------------------------------

    st_nughud = screenblocks == 11 && hud_type == HUDTYPE_NUGHUD && automap_off;

    static int old_st_nughud = 0;

    if (old_st_nughud != st_nughud)
    {
      old_st_nughud = st_nughud;
      ST_Init();
      ST_Start();
    }

    // [Nugget] -------------------------------------------------------------/

    if (!sbardef)
    {
        return;
    }

    // check for incoming chat characters
    if (netgame)
    {
        ST_UpdateChatMessage();
    }

    player_t *player = &players[displayplayer];

    UpdateStatusBar(player);

    // [Nugget] NUGHUD
    if (st_nughud)
    {
        if (nughud.message.x != -1 && nughud.message.y != -1)
        {
            chat_elem->y_pos = nughud.message.y;

            const sbe_widget_t *const mwid = message_elem->subtype.widget;
            const int numlines = array_size(mwid->lines);

            if (numlines > 0)
            {
                const int lineheight = mwid->font->maxheight;

                for (int i = 0;  i < numlines;  i++)
                {
                    if (mwid->lines[i].totalwidth) { chat_elem->y_pos += lineheight; }
                }
            }
        }

        // Stacks
        for (int i = 0;  i < NUMNUGHUDSTACKS;  i++)
        {
            nughud_stackqueues[i].offset = 0;

            int secondtime = 0;

            do {
                for (int j = 0;  j < NUMSQWIDGETS;  j++)
                {
                    const widgetpair_t *const pair = &nughud_stackqueues[i].pairs[j];

                    if (!pair->ntl) { continue; }

                    sbarelem_t *const elem = pair->elem;

                    if (!CheckConditions(elem->conditions, player))
                    { continue; }

                    sbe_widget_t *const wid = elem->subtype.widget;
                    const int numlines = array_size(wid->lines);

                    if (numlines <= 0) { continue; }

                    const nughud_vlignable_t *const stack = &nughud.stacks[i];
                    const sbarwidgettype_t type = wid->type;

                    if (secondtime && i == pair->ntl->stack - 1)
                    {
                        elem->y_pos = stack->y - nughud_stackqueues[i].offset;

                        if (!((type == sbw_message || type == sbw_chat) && nughud.message_defx))
                        {
                            elem->alignment = NughudConvertAlignment(stack->wide, stack->align);
                            elem->x_pos = stack->x;
                        }
                    }

                    const int lineheight = wid->font->maxheight;

                    for (int k = 0;  k < numlines;  k++)
                    {
                        if (!wid->lines[k].totalwidth
                            && !(type == sbw_chat && ST_GetChatOn()))
                        {
                            continue;
                        }

                        if (!secondtime)
                        {
                            switch (stack->vlign) {
                                case -1:  nughud_stackqueues[i].offset += lineheight;      break;
                                case  0:  nughud_stackqueues[i].offset += lineheight / 2;  break;
                                case  1:  default:                                         break;
                            }
                        }
                        else { nughud_stackqueues[i].offset -= lineheight; }
                    }
                }
            } while (!secondtime++);
        }
    }

    if (hud_crosshair)
    {
        HU_UpdateCrosshair();
    }

    if (!nodrawers)
    {
        DoPaletteStuff(player); // Do red-/gold-shifts from damage/items
    }
}

void ST_Drawer(void)
{
    if (!sbardef)
    {
        return;
    }

    DrawStatusBar();

    if (hud_crosshair_on) // [Nugget] Crosshair toggle
    {
        HU_DrawCrosshair();
    }
}

void ST_Start(void)
{
    if (!sbardef)
    {
        return;
    }

    ResetStatusBar();

    HU_StartCrosshair();
}

patch_t **hu_font = NULL;

void ST_Init(void)
{
    // [Nugget] NUGHUD /------------------------------------------------------

    static boolean firsttime = true;

    static sbardef_t *normal_sbardef = NULL, *nughud_sbardef = NULL;

    if (!normal_sbardef) { normal_sbardef = ST_ParseSbarDef(); }
    if (!nughud_sbardef) { nughud_sbardef = CreateNughudSbarDef(); }

    // [Nugget] -------------------------------------------------------------/

    sbardef = st_nughud ? nughud_sbardef : normal_sbardef;

    if (!sbardef)
    {
        health_elem = armor_elem = ammo_elem = NULL; // [Nugget]
        return;
    }

    // [Nugget] Moved `LoadFacePatches()` below

    hudfont_t *hudfont;
    array_foreach(hudfont, sbardef->hudfonts)
    {
        if (!strcmp(hudfont->name, "Console"))
        {
            hu_font = hudfont->characters;
            break;
        }
    }

    if (!hu_font)
    {
        I_Error("ST_Init: \"Console\" font not found");
    }

    // [Nugget]
    if (firsttime) { firsttime = false; }
    else           { return; }

    LoadFacePatches();

    HU_InitCrosshair();
    HU_InitCommandHistory();
    HU_InitObituaries();

    ST_InitWidgets();

    // [Nugget] ==============================================================

    LoadNuggetGraphics();

    // NUGHUD ----------------------------------------------------------------

    const nughud_textline_t *ntl;

    for (int i = 0;  i < NUMNUGHUDSTACKS;  i++)
    {
        stackqueue_t *const sq = &nughud_stackqueues[i];

        sq->offset = 0;

        for (int j = 0;  j < NUMSQWIDGETS;  j++)
        {
            sq->pairs[j].ntl = NULL;
            sq->pairs[j].elem = NULL;
            sq->pairs[j].order = 0;
        }
    }

    sbarelem_t *elem;
    array_foreach(elem, nughud_sbardef->statusbars[0].children)
    {
        if (elem->type != sbe_widget) { continue; }

        const sbarwidgettype_t type = elem->subtype.widget->type;

        switch (type)
        {
            case sbw_monsec:   ntl = &nughud.sts;      break;
            case sbw_time:     ntl = &nughud.time;     break;
            case sbw_coord:    ntl = &nughud.coord;    break;
            case sbw_fps:      ntl = &nughud.fps;      break;
            case sbw_rate:     ntl = &nughud.rate;     break;
            case sbw_cmd:      ntl = &nughud.cmd;      break;
            case sbw_speed:    ntl = &nughud.speed;    break;

            case sbw_message:
              ntl = &nughud.message;
              message_elem = elem;
              break;

            case sbw_chat:
              ntl = &nughud.message;
              chat_elem = elem;
              break;

            case sbw_secret:   ntl = &nughud.secret;   break;
            case sbw_title:    ntl = &nughud.title;    break;
            case sbw_powers:   ntl = &nughud.powers;   break;

            default: continue;
        }

        if (nughud.message_defx)
        {
            if (type == sbw_message)
            {
                /*if (message_centered)
                {
                  elem->x_pos = SCREENWIDTH/2;
                  elem->alignment = align_center;
                }
                else*/ {
                  elem->x_pos = 0;
                  elem->alignment = sbe_h_left | sbe_wide_left;
                }
            }
            else if (type == sbw_chat)
            {
                elem->x_pos = 0;
                elem->alignment = sbe_h_left | sbe_wide_left;
            }
        }

        if (NughudAddToStack(ntl, elem, ntl->stack))
        {
            if ((type == sbw_message || type == sbw_chat)
                && ntl->stack < NUMNUGHUDSTACKS)
            {
                NughudAddToStack(ntl, elem, ntl->stack + 1);
            }
        }
    }

    for (int i = 0;  i < NUMNUGHUDSTACKS;  i++)
    {
        qsort(
          nughud_stackqueues[i].pairs,
          NUMSQWIDGETS,
          sizeof(widgetpair_t),
          NughudSortWidgets
        );
    }
}

void ST_InitRes(void)
{
    // killough 11/98: allocate enough for hires
    st_backing_screen =
        Z_Malloc(video.pitch * V_ScaleY(ST_HEIGHT) * sizeof(*st_backing_screen),
                 PU_RENDERER, 0);

  // [Nugget] Status-Bar chunks
  // More than necessary (we only use the section visible in 4:3), but so be it
  st_bar = Z_Malloc((video.pitch * V_ScaleY(ST_HEIGHT)) * sizeof(*st_bar), PU_RENDERER, 0);
}

void ST_ResetPalette(void)
{
    I_SetPalette(W_CacheLumpName("PLAYPAL", PU_CACHE));
}

// [FG] draw Time widget on intermission screen
void WI_DrawWidgets(void)
{
    if (!st_time_elem || !(hud_level_time & HUD_WIDGET_HUD))
    {
        return;
    }

    sbarelem_t time = *st_time_elem;
    time.alignment = sbe_wide_left;
    // leveltime is already added to totalleveltimes before WI_Start()
    DrawLines(0, 0, &time);
}

// [Nugget] /=================================================================

int ST_GetMessageFontHeight(void)
{
    int height = 8;

    if (!sbardef) { return height; }

    const char *font_name = NULL;

    const sbarelem_t *child;
    array_foreach(child, statusbar->children)
    {
        if (child->type == sbe_widget && child->subtype.widget->type == sbw_message)
        { font_name = child->subtype.widget->font_name; }
    }

    if (!font_name) { return height; }

    const hudfont_t *font;
    array_foreach(font, sbardef->hudfonts)
    {
        if (!strcmp(font->name, font_name))
        { height = font->maxheight; }
    }

    return height;
}

boolean ST_IconAvailable(const int i)
{
    return font_extras[i] != NULL;
}

void LoadNuggetGraphics(void)
{
  int lumpnum = 0;
  char namebuf[16];

  // Find Status Bar Berserk patch
  if ((lumpnum = (W_CheckNumForName)("STBERSRK", ns_global)) >= 0)
  {
    stbersrk = (patch_t *) V_CachePatchNum(lumpnum, PU_STATIC);
  }
  else {
    stbersrk = NULL;

    lu_berserk = (W_CheckNumForName)("PSTRA0", ns_sprites);

    if (lu_berserk == -1)
    { lu_berserk = (W_CheckNumForName)("MEDIA0", ns_sprites); }
  }

  // Find Status Bar Infinity patch
  if ((lumpnum = (W_CheckNumForName)("STINFNTY", ns_global)) >= 0)
  {
    stinfnty = (patch_t *) V_CachePatchNum(lumpnum, PU_STATIC);
  }
  else { stinfnty = NULL; }

  // Load HUD icons
  for (int i = 0;  i < HU_FONTEXTRAS;  i++)
  {
    static const char *names[] = {
      "HUDKILLS", "HUDITEMS", "HUDSCRTS",           // Stats icons
      "HUDINVIS", "HUDINVUL", "HUDLIGHT", "HUDSUIT" // Powerup icons
    };

    const int lumpnum = W_CheckNumForName(names[i]);

    if (lumpnum != -1)
    { font_extras[i] = V_CachePatchNum(lumpnum, PU_STATIC); }
  }

  // NUGHUD ==================================================================

  for (int i = 0;  i < NUMNUGHUDPATCHES;  i++)
  {
    if (nughud.patchnames[i] != NULL)
    {
      nughud_patchlump[i] = (W_CheckNumForName)(nughud.patchnames[i], ns_sprites);

      if (nughud_patchlump[i] == -1)
      { nughud_patchlump[i] = (W_CheckNumForName)(nughud.patchnames[i], ns_global); }
    }
    else { nughud_patchlump[i] = -1; }
  }

  // Berserk -----------------------------------------------------------------

  // Load NHBERSRK
  if ((lumpnum = (W_CheckNumForName)("NHBERSRK", ns_global)) >= 0)
  {
    nhbersrk = (patch_t *) V_CachePatchNum(lumpnum, PU_STATIC);
  }
  else { nhbersrk = NULL; }

  // Ammo icons --------------------------------------------------------------

  // Load NHAMMO0 to NHAMMO3 if available
  for (int i = 0;  i < 4;  i++)
  {
    M_snprintf(namebuf, sizeof(namebuf), "NHAMMO%d", i);

    if ((lumpnum = (W_CheckNumForName)(namebuf, ns_global)) >= 0)
    {
      nhammo[i] = (patch_t *) V_CachePatchNum(lumpnum, PU_STATIC);
    }
    else {
      nhammo[0] = NULL;
      break;
    }
  }

  // Ammo bar ----------------------------------------------------------------

  // Load NHAMBAR0 to NHAMBAR1 if available
  for (int i = 0;  i < 2;  i++)
  {
    M_snprintf(namebuf, sizeof(namebuf), "NHAMBAR%d", i);

    if ((lumpnum = (W_CheckNumForName)(namebuf, ns_global)) >= 0)
    {
      nhambar[i] = (patch_t *) V_CachePatchNum(lumpnum, PU_STATIC);
    }
    else if (!i) { break; }
  }

  // Health icons ------------------------------------------------------------

  // Load NHEALTH0 to NHEALTH1 if available
  for (int i = 0;  i < 2;  i++)
  {
    M_snprintf(namebuf, sizeof(namebuf), "NHEALTH%d", i);

    if ((lumpnum = (W_CheckNumForName)(namebuf, ns_global)) >= 0)
    {
      nhealth[i] = (patch_t *) V_CachePatchNum(lumpnum, PU_STATIC);
    }
    else {
      nhealth[0] = NULL;
      break;
    }
  }

  // Health bar --------------------------------------------------------------

  // Load NHHLBAR0 to NHHLBAR1 if available
  for (int i = 0;  i < 2;  i++)
  {
    M_snprintf(namebuf, sizeof(namebuf), "NHHLBAR%d", i);

    if ((lumpnum = (W_CheckNumForName)(namebuf, ns_global)) >= 0)
    {
      nhhlbar[i] = (patch_t *) V_CachePatchNum(lumpnum, PU_STATIC);
    }
    else if (!i) { break; }
  }

  // Armor icons -------------------------------------------------------------

  // Load NHARMOR0 to NHARMOR2 if available
  for (int i = 0;  i < 3;  i++)
  {
    M_snprintf(namebuf, sizeof(namebuf), "NHARMOR%d", i);

    if ((lumpnum = (W_CheckNumForName)(namebuf, ns_global)) >= 0)
    {
      nharmor[i] = (patch_t *) V_CachePatchNum(lumpnum, PU_STATIC);
    }
    else {
      nharmor[0] = NULL;
      break;
    }
  }

  // Armor bar ---------------------------------------------------------------

  // Load NHARBAR0 to NHARBAR1 if available
  for (int i = 0;  i < 2;  i++)
  {
    M_snprintf(namebuf, sizeof(namebuf), "NHARBAR%d", i);

    if ((lumpnum = (W_CheckNumForName)(namebuf, ns_global)) >= 0)
    {
      nharbar[i] = (patch_t *) V_CachePatchNum(lumpnum, PU_STATIC);
    }
    else if (!i) { break; }
  }

  // Infinity ----------------------------------------------------------------

  // Load NHINFNTY
  if ((lumpnum = (W_CheckNumForName)("NHINFNTY", ns_global)) >= 0)
  {
    nhinfnty = (patch_t *) V_CachePatchNum(lumpnum, PU_STATIC);
  }
  else { nhinfnty = NULL; }
}

// NUGHUD --------------------------------------------------------------------

void ST_refreshBackground(void)
{
  st_refresh_background = true;

  // Status-Bar chunks -------------------------------------------------------

  patch_t *const sbar = V_CachePatchName("STBAR", PU_STATIC);
  
  V_UseBuffer(st_bar);

  V_DrawPatch((SCREENWIDTH - SHORT(sbar->width)) / 2 + SHORT(sbar->leftoffset), 0, sbar);

  V_RestoreBuffer();
}

static sbaralignment_t NughudConvertAlignment(const int wide, const int align)
{
  sbaralignment_t alignment;

  alignment = (align + 1) | sbe_v_top;

  alignment |= (wide < 0) ? sbe_wide_left  :
               (wide > 0) ? sbe_wide_right : 0;

  if (abs(wide) == 2) { alignment |= sbe_wide_force; }

  return alignment;
}

static int NughudWideShift(const int wide)
{
  return   (abs(wide) == 2) ? video.deltaw             * (wide / 2)
         : (abs(wide) == 1) ? video.deltaw * st_layout *  wide
         :                    0;
}

static void DrawNughudPatch(nughud_vlignable_t *widget, patch_t *patch, boolean no_offsets)
{
  int x, y;

  x = widget->x + NughudWideShift(widget->wide)
    - ((widget->align == 1) ? SHORT(patch->width)   :
       (widget->align == 0) ? SHORT(patch->width)/2 : 0);

  y = widget->y
    - ((widget->vlign == -1) ? SHORT(patch->height)   :
       (widget->vlign ==  0) ? SHORT(patch->height)/2 : 0);

  if (no_offsets) {
    x += SHORT(patch->leftoffset);
    y += SHORT(patch->topoffset);
  }

  V_DrawPatch(x, y, patch);
}

static void DrawNughudSBChunk(nughud_sbchunk_t *chunk)
{
  int x  = chunk->x + NughudWideShift(chunk->wide) + video.deltaw,
      y  = chunk->y,
      sx = chunk->sx + video.deltaw,
      sy = chunk->sy,
      sw = chunk->sw,
      sh = chunk->sh;

  sw = MIN(ST_WIDTH - chunk->sx, sw);
  sw = MIN(video.unscaledw - x,  sw);

  sh = MIN(ST_HEIGHT - chunk->sy, sh);
  sh = MIN(SCREENHEIGHT - y,      sh);

  V_CopyRect(sx, sy, st_bar, sw, sh, x, y);
}

static void DrawNughudBar(nughud_bar_t *widget, patch_t **patches, int units, int maxunits)
{
  if (widget->x > -1 && patches[0])
  {
    const boolean twobars = patches[1] && maxunits < units;

    for (int i = 0;  i < (1 + twobars);  i++)
    {
      const int slices = MIN(100 * (2 - twobars), (units * 100 / maxunits) - (100 * i)) * 100 / widget->ups;

      const int xstep = (widget->xstep || widget->ystep)
                        ? widget->xstep : SHORT(patches[i]->width);

      const int ystep = widget->ystep;

      const int x = widget->x
                  + NughudWideShift(widget->wide)
                  - ((widget->align == 1) ? slices * xstep     :
                     (widget->align == 0) ? slices * xstep / 2 : 0);

      const int y = widget->y
                  - ((widget->vlign == -1) ? slices * ystep     :
                     (widget->vlign ==  0) ? slices * ystep / 2 : 0);

      for (int j = 0;  j < slices;  j++)
      { V_DrawPatch(x + (xstep * j), y + (ystep * j), patches[i]); }
    }
  }
}

static void DrawNughudGraphics(void)
{
  if (!st_nughud) { return; }

  player_t *plyr = &players[displayplayer];

  // Status-Bar chunks -------------------------------------------------------

  for (int i = 0;  i < NUMSBCHUNKS;  i++)
  {
    if (nughud.sbchunks[i].x > -1) { DrawNughudSBChunk(&nughud.sbchunks[i]); }
  }

  // Patches -----------------------------------------------------------------

  // First 4 patches are drawn before bars
  for (int i = 0;  i < NUMNUGHUDPATCHES/2;  i++)
  {
    if (nughud_patchlump[i] >= 0)
    {
      DrawNughudPatch(
        &nughud.patches[i],
        V_CachePatchNum(nughud_patchlump[i], PU_STATIC),
        !nughud.patch_offsets
      );
    }
  }

  {
    extern int maxhealth, max_armor;

    if (weaponinfo[plyr->readyweapon].ammo != am_noammo)
    {
      DrawNughudBar(
        &nughud.ammobar, nhambar,
        plyr->ammo[weaponinfo[plyr->readyweapon].ammo],
        plyr->maxammo[weaponinfo[plyr->readyweapon].ammo] / (1 + plyr->backpack)
      );
    }

    const int health = health_elem ? SmoothCount(health_elem->subtype.number->oldvalue, plyr->health)
                                   : plyr->health;

    const int armor = armor_elem ? SmoothCount(armor_elem->subtype.number->oldvalue, plyr->armorpoints)
                                 : plyr->armorpoints;

    DrawNughudBar(&nughud.healthbar, nhhlbar, health, maxhealth);
    DrawNughudBar(&nughud.armorbar,  nharbar,  armor, max_armor/2);
  }

  for (int i = NUMNUGHUDPATCHES/2;  i < NUMNUGHUDPATCHES;  i++)
  {
    if (nughud_patchlump[i] >= 0)
    {
      DrawNughudPatch(
        &nughud.patches[i],
        V_CachePatchNum(nughud_patchlump[i], PU_STATIC),
        !nughud.patch_offsets
      );
    }
  }

  // Singleplayer Face background --------------------------------------------

  if (nughud.face.x > -1 && nughud.face_bg && !netgame)
  {
    patch_t *const bg = V_CachePatchName("STFB0", PU_STATIC);
    const int x = nughud.face.x + NughudWideShift(nughud.face.wide),
              y = nughud.face.y + ST_HEIGHT - SHORT(bg->height);

    nughud_sbchunk_t chunk; // Reuse the chunk drawing function for its bounds-checking

    chunk.x = x + SHORT(bg->leftoffset);
    chunk.y = y - SHORT(bg->topoffset);
    chunk.wide = 0;
    chunk.sx = 143 + SHORT(bg->leftoffset);
    chunk.sy = 1 - SHORT(bg->topoffset);
    chunk.sw = SHORT(bg->width);
    chunk.sh = SHORT(bg->height);

    DrawNughudSBChunk(&chunk);
  }

  // Icons -------------------------------------------------------------------

  if (nughud.ammoicon.x > -1 && weaponinfo[plyr->readyweapon].ammo != am_noammo)
  {
    patch_t *patch;
    int lump;
    boolean no_offsets = false;

    if (nhammo[0])
    {
      patch = nhammo[BETWEEN(0, 3, weaponinfo[plyr->readyweapon].ammo)];
    }
    else {
      char namebuf[32];
      boolean big = nughud.ammoicon_big;

      no_offsets = true;

      switch (BETWEEN(0, 3, weaponinfo[plyr->readyweapon].ammo))
      {
        case 0: M_snprintf(namebuf, sizeof(namebuf), big ? "AMMOA0" : "CLIPA0"); break;
        case 1: M_snprintf(namebuf, sizeof(namebuf), big ? "SBOXA0" : "SHELA0"); break;
        case 2: M_snprintf(namebuf, sizeof(namebuf), big ? "CELPA0" : "CELLA0"); break;
        case 3: M_snprintf(namebuf, sizeof(namebuf), big ? "BROKA0" : "ROCKA0"); break;
      }

      if ((lump = (W_CheckNumForName)(namebuf, ns_sprites)) >= 0)
      {
        patch = (patch_t *) V_CachePatchNum(lump, PU_STATIC);
      }
      else { patch = NULL; }
    }

    if (patch) { DrawNughudPatch(&nughud.ammoicon, patch, no_offsets); }
  }

  if (nughud.healthicon.x > -1)
  {
    patch_t *patch;
    int lump;
    boolean no_offsets = false;

    if (nhealth[0])
    {
      patch = nhealth[plyr->powers[pw_strength] ? 1 : 0];
    }
    else {
      char namebuf[32];

      no_offsets = true;

      switch (plyr->powers[pw_strength] ? 1 : 0)
      {
        case 0: M_snprintf(namebuf, sizeof(namebuf), "MEDIA0"); break;
        case 1: M_snprintf(namebuf, sizeof(namebuf), "PSTRA0"); break;
      }

      if ((lump = (W_CheckNumForName)(namebuf, ns_sprites)) >= 0)
      {
        patch = (patch_t *) V_CachePatchNum(lump, PU_STATIC);
      }
      else { patch = NULL; }
    }

    if (patch) { DrawNughudPatch(&nughud.healthicon, patch, no_offsets); }
  }

  if (nughud.armoricon.x > -1)
  {
    patch_t *patch;
    int lump;
    boolean no_offsets = false;

    if (nharmor[0])
    {
      patch = nharmor[BETWEEN(0, 2, plyr->armortype)];
    }
    else {
      char namebuf[32];

      no_offsets = true;

      switch (BETWEEN(0, 2, plyr->armortype))
      {
        case 0: M_snprintf(namebuf, sizeof(namebuf), "BON2A0"); break;
        case 1: M_snprintf(namebuf, sizeof(namebuf), "ARM1A0"); break;
        case 2: M_snprintf(namebuf, sizeof(namebuf), "ARM2A0"); break;
      }

      if ((lump = (W_CheckNumForName)(namebuf, ns_sprites)) >= 0)
      {
        patch = (patch_t *) V_CachePatchNum(lump, PU_STATIC);
      }
      else { patch = NULL; }
    }

    if (patch) { DrawNughudPatch(&nughud.armoricon, patch, no_offsets); }
  }
}

// NUGHUD stacks -------------------------------------------------------------

static boolean NughudAddToStack(
  const nughud_textline_t *const ntl,
  sbarelem_t *const elem,
  const int stack
)
{
  if (ntl->x != -1 || ntl->y != -1) { return false; }

  for (int i = 0;  i < NUMSQWIDGETS;  i++)
  {
    widgetpair_t *pair = &nughud_stackqueues[stack - 1].pairs[i];

    if (!pair->ntl)
    {
      pair->ntl = ntl;
      pair->elem = elem;

      pair->order = (elem->subtype.widget->type == sbw_message) ? -2 :
                    (elem->subtype.widget->type == sbw_chat)    ? -1 : ntl->order;

      break;
    }
  }

  return true;
}

static int NughudSortWidgets(const void *_p1, const void *_p2)
{
  const widgetpair_t *p1 = (widgetpair_t *) _p1,
                     *p2 = (widgetpair_t *) _p2;

  if (!p1->ntl) { return 0; }

  int ret;

       if (p1->order < p2->order) { ret = -1; }
  else if (p1->order > p2->order) { ret =  1; }
  else                            { ret =  0; }

  if (nughud.stacks[p1->ntl->stack - 1].vlign == -1) { ret = -ret; }

  return ret;
}

// NUGHUD loading ------------------------------------------------------------

static sbardef_t *CreateNughudSbarDef(void)
{
  sbardef_t *out = calloc(1, sizeof(*out));

  char namebuf[16];
  int lumpnum;
  int maxwidth, maxheight;

  // Fonts ===================================================================

  // Tall Numbers ------------------------------------------------------------

  numberfont_t tnum = {0};

  tnum.name = M_StringDuplicate("Tall");
  tnum.type = sbf_mono0;

  maxwidth = maxheight = 0;

  for (int i = 0;  i < 10;  i++)
  {
    M_snprintf(namebuf, sizeof(namebuf), "NHTNUM%d", i);

    if ((lumpnum = (W_CheckNumForName)(namebuf, ns_global)) >= 0)
    {
      tnum.numbers[i] = V_CachePatchNum(lumpnum, PU_STATIC);
      maxwidth  = MAX(maxwidth,  SHORT(tnum.numbers[i]->width));
      maxheight = MAX(maxheight, SHORT(tnum.numbers[i]->height));
    }
    else { goto no_nhtnum; }
  }

  if ((lumpnum = (W_CheckNumForName)("NHTMINUS", ns_global)) >= 0)
  {
    tnum.minus = V_CachePatchNum(lumpnum, PU_STATIC);
    maxwidth  = MAX(maxwidth,  SHORT(tnum.minus->width));
    maxheight = MAX(maxheight, SHORT(tnum.minus->height));
  }
  else { goto no_nhtnum; }

  if ((lumpnum = (W_CheckNumForName)("NHTPRCNT", ns_global)) >= 0)
  {
    tnum.percent = V_CachePatchNum(lumpnum, PU_STATIC);
    maxwidth  = MAX(maxwidth,  SHORT(tnum.percent->width));
    maxheight = MAX(maxheight, SHORT(tnum.percent->height));
  }
  else { goto no_nhtnum; }

  goto end_tnum;

no_nhtnum:

  maxwidth = maxheight = 0;

  for (int i = 0;  i < 10;  i++)
  {
    M_snprintf(namebuf, sizeof(namebuf), "STTNUM%d", i);

    if ((lumpnum = (W_CheckNumForName)(namebuf, ns_global)) >= 0)
    {
      tnum.numbers[i] = V_CachePatchNum(lumpnum, PU_STATIC);
      maxwidth  = MAX(maxwidth,  SHORT(tnum.numbers[i]->width));
      maxheight = MAX(maxheight, SHORT(tnum.numbers[i]->height));
    }
  }

  if ((lumpnum = (W_CheckNumForName)("STTMINUS", ns_global)) >= 0)
  {
    tnum.minus = V_CachePatchNum(lumpnum, PU_STATIC);
    maxwidth  = MAX(maxwidth,  SHORT(tnum.minus->width));
    maxheight = MAX(maxheight, SHORT(tnum.minus->height));
  }

  if ((lumpnum = (W_CheckNumForName)("STTPRCNT", ns_global)) >= 0)
  {
    tnum.percent = V_CachePatchNum(lumpnum, PU_STATIC);
    maxwidth  = MAX(maxwidth,  SHORT(tnum.percent->width));
    maxheight = MAX(maxheight, SHORT(tnum.percent->height));
  }

end_tnum:

  tnum.monowidth = SHORT(tnum.numbers[0]->width);
  tnum.maxheight = maxheight;

  array_push(out->numberfonts, tnum);

  // Ready Ammo Numbers ------------------------------------------------------

  numberfont_t rnum = {0};

  rnum.name = M_StringDuplicate("ReadyAmmo");
  rnum.type = sbf_mono0;

  maxwidth = maxheight = 0;

  for (int i = 0;  i < 10;  i++)
  {
    M_snprintf(namebuf, sizeof(namebuf), "NHRNUM%d", i);

    if ((lumpnum = (W_CheckNumForName)(namebuf, ns_global)) >= 0)
    {
      rnum.numbers[i] = V_CachePatchNum(lumpnum, PU_STATIC);
      maxwidth  = MAX(maxwidth,  SHORT(rnum.numbers[i]->width));
      maxheight = MAX(maxheight, SHORT(rnum.numbers[i]->height));
    }
    else { goto no_nhrnum; }
  }

  if ((lumpnum = (W_CheckNumForName)("NHRMINUS", ns_global)) >= 0)
  {
    rnum.minus = V_CachePatchNum(lumpnum, PU_STATIC);
    maxwidth  = MAX(maxwidth,  SHORT(rnum.minus->width));
    maxheight = MAX(maxheight, SHORT(rnum.minus->height));
  }
  else { goto no_nhrnum; }

  goto end_rnum;

no_nhrnum:

  maxwidth  = tnum.monowidth;
  maxheight = tnum.maxheight;

  for (int i = 0;  i < 10;  i++)
  {
    rnum.numbers[i] = tnum.numbers[i];
  }

  rnum.minus = tnum.minus;

end_rnum:

  rnum.monowidth = SHORT(rnum.numbers[0]->width);
  rnum.maxheight = maxheight;

  array_push(out->numberfonts, rnum);

  // Ammo Numbers ------------------------------------------------------------

  numberfont_t amnum = {0};

  amnum.name = M_StringDuplicate("Ammo");
  amnum.type = sbf_mono0;

  maxwidth = maxheight = 0;

  for (int i = 0;  i < 10;  i++)
  {
    M_snprintf(namebuf, sizeof(namebuf), "NHAMNUM%d", i);

    if ((lumpnum = (W_CheckNumForName)(namebuf, ns_global)) >= 0)
    {
      amnum.numbers[i] = V_CachePatchNum(lumpnum, PU_STATIC);
      maxwidth  = MAX(maxwidth,  SHORT(amnum.numbers[i]->width));
      maxheight = MAX(maxheight, SHORT(amnum.numbers[i]->height));
    }
    else { goto no_nhamnum; }
  }

  goto end_amnum;

no_nhamnum:

  maxwidth = maxheight = 0;

  for (int i = 0;  i < 10;  i++)
  {
    M_snprintf(namebuf, sizeof(namebuf), "STYSNUM%d", i);

    if ((lumpnum = (W_CheckNumForName)(namebuf, ns_global)) >= 0)
    {
      amnum.numbers[i] = V_CachePatchNum(lumpnum, PU_STATIC);
      maxwidth  = MAX(maxwidth,  SHORT(amnum.numbers[i]->width));
      maxheight = MAX(maxheight, SHORT(amnum.numbers[i]->height));
    }
  }

end_amnum:

  amnum.monowidth = SHORT(amnum.numbers[0]->width);
  amnum.maxheight = maxheight;

  array_push(out->numberfonts, amnum);

  // Console Font ------------------------------------------------------------

  hudfont_t cfn = {0};

  cfn.name = M_StringDuplicate("Console");
  cfn.type = sbf_proportional;

  maxwidth = maxheight = 0;

  for (int i = 0;  i < HU_FONTSIZE;  i++)
  {
    M_snprintf(namebuf, sizeof(namebuf), "STCFN%03d", i + HU_FONTSTART);

    if ((lumpnum = (W_CheckNumForName)(namebuf, ns_global)) >= 0)
    {
      cfn.characters[i] = V_CachePatchNum(lumpnum, PU_STATIC);
      maxwidth  = MAX(maxwidth,  SHORT(cfn.characters[i]->width));
      maxheight = MAX(maxheight, SHORT(cfn.characters[i]->height));
    }
  }

  cfn.monowidth = maxwidth;
  cfn.maxheight = maxheight;

  array_push(out->hudfonts, cfn);

  // Digits Font -------------------------------------------------------------

  hudfont_t dig = {0};

  dig.name = M_StringDuplicate("Digits");
  dig.type = sbf_mono0;

  maxwidth = maxheight = 0;

  for (int i = 0;  i < HU_FONTSIZE;  i++)
  {
    M_snprintf(namebuf, sizeof(namebuf), "DIG%03d", i + HU_FONTSTART);

    if ((lumpnum = (W_CheckNumForName)(namebuf, ns_global)) >= 0)
    {
      dig.characters[i] = V_CachePatchNum(lumpnum, PU_STATIC);
      maxwidth  = MAX(maxwidth,  SHORT(dig.characters[i]->width));
      maxheight = MAX(maxheight, SHORT(dig.characters[i]->height));
    }
  }

  dig.monowidth = maxwidth;
  dig.maxheight = maxheight;

  array_push(out->hudfonts, dig);

  // Widgets =================================================================

  statusbar_t sb = {0};

  sb.height = 200;
  sb.fullscreenrender = true;

  // Ammo --------------------------------------------------------------------

  if (nughud.ammo.x > -1)
  {
    sbarelem_t elem = {0};
    elem.cr = elem.crboom = CR_NONE;

    elem.type = sbe_number;

    elem.x_pos = nughud.ammo.x;
    elem.y_pos = nughud.ammo.y;
    elem.alignment = NughudConvertAlignment(nughud.ammo.wide, nughud.ammo.align);

    sbe_number_t *number = calloc(1, sizeof(*number));

    number->type = sbn_ammoselected;
    number->font_name = M_StringDuplicate("ReadyAmmo");
    number->maxlength = 3;

    elem.subtype.number = number;

    sbarcondition_t condition = {0};

    condition.condition = sbc_selectedweaponhasammo;

    array_push(elem.conditions, condition);

    array_push(sb.children, elem);
  }

  // Health ------------------------------------------------------------------

  if (nughud.health.x > -1)
  {
    sbarelem_t elem = {0};
    elem.cr = elem.crboom = CR_NONE;

    elem.type = sbe_percent;

    elem.x_pos = nughud.health.x;
    elem.y_pos = nughud.health.y;
    elem.alignment = NughudConvertAlignment(nughud.health.wide, nughud.health.align);

    sbe_number_t *number = calloc(1, sizeof(*number));

    number->type = sbn_health;
    number->font_name = M_StringDuplicate("Tall");
    number->maxlength = 3;

    elem.subtype.number = number;

    array_push(sb.children, elem);
  }

  // Arms Numbers ------------------------------------------------------------

  const char *unavailable_string = "NHW0NUM",
               *available_string = "NHW1NUM";

  for (int i = 0;  i < 2;  i++)
  {
    int j;

    for (j = 0;  j < 9;  j++)
    {
      M_snprintf(namebuf, sizeof(namebuf), "NHW%dNUM%d", i, j+1);

      if ((W_CheckNumForName)(namebuf, ns_global) == -1)
      {
        unavailable_string = "STGNUM";
          available_string = "STYSNUM";
        break;
      }
    }

    if (j < 9) { break; }
  }

  for (int i = 0;  i < 9;  i++)
  {
    if (nughud.arms[i].x > -1)
    {
      for (int j = 0;  j < 2;  j++)
      {
        sbarelem_t elem = {0};
        elem.cr = elem.crboom = CR_NONE;

        elem.type = sbe_graphic;

        elem.x_pos = nughud.arms[i].x;
        elem.y_pos = nughud.arms[i].y;
        elem.alignment = NughudConvertAlignment(nughud.arms[i].wide, -1);

        sbe_graphic_t *graphic = calloc(1, sizeof(*graphic));

        M_snprintf(
          namebuf, sizeof(namebuf), "%s%d",
          j ? available_string : unavailable_string,
          i + 1
        );

        graphic->patch_name = M_StringDuplicate(namebuf);

        elem.subtype.graphic = graphic;

        sbarcondition_t condition = {0};

        if (i == wp_fist)
        {
            condition.condition = j ? sbc_itemowned : sbc_itemnotowned;
            condition.param = item_berserk;
        }
        else if (i == wp_shotgun && nughud.arms[wp_supershotgun].x == -1)
        {
            condition.condition = j ? sbc_weaponslotowned : sbc_weaponslotnotowned;
            condition.param = 3;
        }
        else {
            condition.condition = j ? sbc_weaponowned : sbc_weaponnotowned;
            condition.param = i;
        }

        array_push(elem.conditions, condition);

        array_push(sb.children, elem);
      }
    }
  }

  // Frags -------------------------------------------------------------------

  if (nughud.frags.x > -1)
  {
    sbarelem_t elem = {0};
    elem.cr = elem.crboom = CR_NONE;

    elem.type = sbe_number;

    elem.x_pos = nughud.frags.x;
    elem.y_pos = nughud.frags.y;
    elem.alignment = NughudConvertAlignment(nughud.frags.wide, nughud.frags.align);

    sbe_number_t *number = calloc(1, sizeof(*number));

    number->type = sbn_frags;
    number->font_name = M_StringDuplicate("Tall");
    number->maxlength = 2;

    elem.subtype.number = number;

    sbarcondition_t condition = {0};

    condition.condition = sbc_sessiontypeeequal;
    condition.param = 2;

    array_push(elem.conditions, condition);

    array_push(sb.children, elem);
  }

  // Face --------------------------------------------------------------------

  if (nughud.face.x > -1)
  {
    if (nughud.face_bg)
    {
      sbarelem_t elem = {0};
      elem.cr = elem.crboom = CR_NONE;

      elem.type = sbe_facebackground;

      elem.x_pos = nughud.face.x - 1;
      elem.y_pos = nughud.face.y - 1;
      elem.alignment = NughudConvertAlignment(nughud.face.wide, -1);

      sbarcondition_t condition = {0};

      condition.condition = sbc_sessiontypenotequal;
      condition.param = 0;

      array_push(elem.conditions, condition);

      array_push(sb.children, elem);
    }

    sbarelem_t elem = {0};
    elem.cr = elem.crboom = CR_NONE;

    elem.type = sbe_face;

    elem.x_pos = nughud.face.x;
    elem.y_pos = nughud.face.y;
    elem.alignment = NughudConvertAlignment(nughud.face.wide, -1);

    sbe_face_t *face = calloc(1, sizeof(*face));

    elem.subtype.face = face;

    array_push(sb.children, elem);
  }

  // Keys --------------------------------------------------------------------

  const char *keys_string = "NHKEYS";

  for (int i = 0;  i < 9;  i++)
  {
    M_snprintf(namebuf, sizeof(namebuf), "NHKEYS%d", i);

    if ((W_CheckNumForName)(namebuf, ns_global) == -1)
    {
      keys_string = "STKEYS";
      break;
    }
  }

  for (int i = 0;  i < 3;  i++)
  {
    nughud_widget_t nh = nughud.keys[i];

    if (nh.x > -1)
    {
      for (int j = 0;  j < 3;  j++)
      {
        sbarelem_t elem = {0};
        elem.cr = elem.crboom = CR_NONE;

        elem.type = sbe_graphic;

        elem.x_pos = nh.x;
        elem.y_pos = nh.y;
        elem.alignment = NughudConvertAlignment(nh.wide, -1);

        sbe_graphic_t *graphic = calloc(1, sizeof(*graphic));

        M_snprintf(namebuf, sizeof(namebuf), "%s%d", keys_string, (j * 3) + i);

        graphic->patch_name = M_StringDuplicate(namebuf);

        elem.subtype.graphic = graphic;

        sbarcondition_t cond1 = {0};
        sbarcondition_t cond2 = {0};

        switch (j)
        {
          case 0: { // Card
            // Card owned
            cond1.condition = sbc_itemowned;
            cond1.param = i + 1;

            // Skull not owned
            cond2.condition = sbc_itemnotowned;
            cond2.param = i + 4;

            break;
          }

          case 1: { // Skull
            // Card not owned
            cond1.condition = sbc_itemnotowned;
            cond1.param = i + 1;

            // Skull owned
            cond2.condition = sbc_itemowned;
            cond2.param = i + 4;

            break;
          }

          case 2: { // Both
            // Card owned
            cond1.condition = sbc_itemowned;
            cond1.param = i + 1;

            // Skull owned
            cond2.condition = sbc_itemowned;
            cond2.param = i + 4;

            break;
          }
        }

        array_push(elem.conditions, cond1);
        array_push(elem.conditions, cond2);

        array_push(sb.children, elem);
      }
    }
  }

  // Armor -------------------------------------------------------------------

  if (nughud.armor.x > -1)
  {
    sbarelem_t elem = {0};
    elem.cr = elem.crboom = CR_NONE;

    elem.type = sbe_percent;

    elem.x_pos = nughud.armor.x;
    elem.y_pos = nughud.armor.y;
    elem.alignment = NughudConvertAlignment(nughud.armor.wide, nughud.armor.align);

    sbe_number_t *number = calloc(1, sizeof(*number));

    number->type = sbn_armor;
    number->font_name = M_StringDuplicate("Tall");
    number->maxlength = 3;

    elem.subtype.number = number;

    array_push(sb.children, elem);
  }

  // Ammos -------------------------------------------------------------------

  for (int i = 0;  i < 4;  i++)
  {
    for (int j = 0;  j < 2;  j++)
    {
      nughud_alignable_t nh = (j ? nughud.maxammos : nughud.ammos)[i];

      if (nh.x > -1)
      {
        sbarelem_t elem = {0};
        elem.cr = elem.crboom = CR_NONE;

        elem.type = sbe_number;

        elem.x_pos = nh.x;
        elem.y_pos = nh.y;
        elem.alignment = NughudConvertAlignment(nh.wide, nh.align);

        sbe_number_t *number = calloc(1, sizeof(*number));

        number->type = j ? sbn_maxammo : sbn_ammo;
        number->param = i;
        number->font_name = M_StringDuplicate("Ammo");
        number->maxlength = 3;

        elem.subtype.number = number;

        array_push(sb.children, elem);
      }
    }
  }

  // Time --------------------------------------------------------------------

  {
    sbarelem_t elem = {0};
    elem.cr = elem.crboom = CR_NONE;

    elem.type = sbe_widget;

    elem.x_pos = MAX(0, nughud.time.x);
    elem.y_pos = MAX(0, nughud.time.y);
    elem.alignment = NughudConvertAlignment(nughud.time.wide, nughud.time.align);

    sbe_widget_t *widget = calloc(1, sizeof(*widget));

    widget->type = sbw_time;
    widget->font_name = M_StringDuplicate("Digits");

    elem.subtype.widget = widget;

    array_push(sb.children, elem);
  }

  // Stats -------------------------------------------------------------------

  {
    sbarelem_t elem = {0};
    elem.cr = elem.crboom = CR_NONE;

    elem.type = sbe_widget;

    elem.x_pos = MAX(0, nughud.sts.x);
    elem.y_pos = MAX(0, nughud.sts.y);
    elem.alignment = NughudConvertAlignment(nughud.sts.wide, nughud.sts.align);

    sbe_widget_t *widget = calloc(1, sizeof(*widget));

    widget->type = sbw_monsec;
    widget->font_name = M_StringDuplicate("Digits");

    elem.subtype.widget = widget;

    array_push(sb.children, elem);
  }

  // Title -------------------------------------------------------------------

  {
    sbarelem_t elem = {0};
    elem.cr = elem.crboom = CR_NONE;

    elem.type = sbe_widget;

    elem.x_pos = MAX(0, nughud.title.x);
    elem.y_pos = MAX(0, nughud.title.y);
    elem.alignment = NughudConvertAlignment(nughud.title.wide, nughud.title.align);

    sbe_widget_t *widget = calloc(1, sizeof(*widget));

    widget->type = sbw_title;
    widget->font_name = M_StringDuplicate("Console");

    elem.subtype.widget = widget;

    sbarcondition_t condition = {0};

    condition.condition = sbc_widgetmode;
    condition.param = sbc_mode_automap | sbc_mode_overlay;

    array_push(elem.conditions, condition);

    array_push(sb.children, elem);
  }

  // Powerup timers ----------------------------------------------------------

  {
    sbarelem_t elem = {0};
    elem.cr = elem.crboom = CR_NONE;

    elem.type = sbe_widget;

    elem.x_pos = MAX(0, nughud.powers.x);
    elem.y_pos = MAX(0, nughud.powers.y);
    elem.alignment = NughudConvertAlignment(nughud.powers.wide, nughud.powers.align);

    sbe_widget_t *widget = calloc(1, sizeof(*widget));

    widget->type = sbw_powers;
    widget->font_name = M_StringDuplicate("Digits");

    elem.subtype.widget = widget;

    array_push(sb.children, elem);
  }

  // Coords ------------------------------------------------------------------

  {
    sbarelem_t elem = {0};
    elem.cr = elem.crboom = CR_NONE;

    elem.type = sbe_widget;

    elem.x_pos = MAX(0, nughud.coord.x);
    elem.y_pos = MAX(0, nughud.coord.y);
    elem.alignment = NughudConvertAlignment(nughud.coord.wide, nughud.coord.align);

    sbe_widget_t *widget = calloc(1, sizeof(*widget));

    widget->type = sbw_coord;
    widget->font_name = M_StringDuplicate("Digits");

    elem.subtype.widget = widget;

    array_push(sb.children, elem);
  }

  // FPS ---------------------------------------------------------------------

  {
    sbarelem_t elem = {0};
    elem.cr = elem.crboom = CR_NONE;

    elem.type = sbe_widget;

    elem.x_pos = MAX(0, nughud.fps.x);
    elem.y_pos = MAX(0, nughud.fps.y);
    elem.alignment = NughudConvertAlignment(nughud.fps.wide, nughud.fps.align);

    sbe_widget_t *widget = calloc(1, sizeof(*widget));

    widget->type = sbw_fps;
    widget->font_name = M_StringDuplicate("Digits");

    elem.subtype.widget = widget;

    array_push(sb.children, elem);
  }

  // Rate --------------------------------------------------------------------

  {
    sbarelem_t elem = {0};
    elem.cr = elem.crboom = CR_NONE;

    elem.type = sbe_widget;

    elem.x_pos = MAX(0, nughud.rate.x);
    elem.y_pos = MAX(0, nughud.rate.y);
    elem.alignment = NughudConvertAlignment(nughud.rate.wide, nughud.rate.align);

    sbe_widget_t *widget = calloc(1, sizeof(*widget));

    widget->type = sbw_rate;
    widget->font_name = M_StringDuplicate("Digits");

    elem.subtype.widget = widget;

    array_push(sb.children, elem);
  }

  // Command history ---------------------------------------------------------

  {
    sbarelem_t elem = {0};
    elem.cr = elem.crboom = CR_NONE;

    elem.type = sbe_widget;

    elem.x_pos = MAX(0, nughud.cmd.x);
    elem.y_pos = MAX(0, nughud.cmd.y);
    elem.alignment = NughudConvertAlignment(nughud.cmd.wide, nughud.cmd.align);

    sbe_widget_t *widget = calloc(1, sizeof(*widget));

    widget->type = sbw_cmd;
    widget->font_name = M_StringDuplicate("Digits");

    elem.subtype.widget = widget;

    array_push(sb.children, elem);
  }

  // Speedometer -------------------------------------------------------------

  {
    sbarelem_t elem = {0};
    elem.cr = elem.crboom = CR_NONE;

    elem.type = sbe_widget;

    elem.x_pos = MAX(0, nughud.speed.x);
    elem.y_pos = MAX(0, nughud.speed.y);
    elem.alignment = NughudConvertAlignment(nughud.speed.wide, nughud.speed.align);

    sbe_widget_t *widget = calloc(1, sizeof(*widget));

    widget->type = sbw_speed;
    widget->font_name = M_StringDuplicate("Digits");

    elem.subtype.widget = widget;

    array_push(sb.children, elem);
  }

  // Message -----------------------------------------------------------------

  {
    sbarelem_t elem = {0};
    elem.cr = elem.crboom = CR_NONE;

    elem.type = sbe_widget;

    elem.x_pos = MAX(0, nughud.message.x);
    elem.y_pos = MAX(0, nughud.message.y);
    elem.alignment = NughudConvertAlignment(nughud.message.wide, nughud.message.align);

    sbe_widget_t *widget = calloc(1, sizeof(*widget));

    widget->type = sbw_message;
    widget->font_name = M_StringDuplicate("Console");
    widget->duration = 4*TICRATE;

    elem.subtype.widget = widget;

    array_push(sb.children, elem);
  }

  // Chat --------------------------------------------------------------------

  {
    sbarelem_t elem = {0};
    elem.cr = CR_GOLD;
    elem.crboom = CR_NONE;

    elem.type = sbe_widget;

    elem.x_pos = MAX(0, nughud.message.x);
    elem.y_pos = MAX(0, nughud.message.y) + cfn.maxheight;
    elem.alignment = NughudConvertAlignment(nughud.message.wide, nughud.message.align);

    sbe_widget_t *widget = calloc(1, sizeof(*widget));

    widget->type = sbw_chat;
    widget->font_name = M_StringDuplicate("Console");

    elem.subtype.widget = widget;

    array_push(sb.children, elem);
  }

  // Secret message ----------------------------------------------------------

  {
    sbarelem_t elem = {0};
    elem.cr = CR_GOLD;
    elem.crboom = CR_NONE;

    elem.type = sbe_widget;

    elem.x_pos = MAX(0, nughud.secret.x);
    elem.y_pos = MAX(0, nughud.secret.y);
    elem.alignment = NughudConvertAlignment(nughud.secret.wide, nughud.secret.align);

    sbe_widget_t *widget = calloc(1, sizeof(*widget));

    widget->type = sbw_secret;
    widget->font_name = M_StringDuplicate("Console");
    widget->duration = 2.5f*TICRATE;

    elem.subtype.widget = widget;

    array_push(sb.children, elem);
  }

  array_push(out->statusbars, sb);

  return out;
}

// [Nugget] =================================================================/

void ST_BindSTSVariables(void)
{
  // [Nugget] NUGHUD
  M_BindNum("fullscreen_hud_type", &hud_type, NULL,
            HUDTYPE_SBARDEF, HUDTYPE_SBARDEF, HUDTYPE_NUGHUD,
            ss_stat, wad_no, "Fullscreen HUD type (0 = SBARDEF; 1 = NUGHUD)");

  M_BindNum("st_layout", &st_layout, NULL,  st_wide, st_original, st_wide,
             ss_stat, wad_no, "HUD layout");
  M_BindBool("sts_colored_numbers", &sts_colored_numbers, NULL,
             false, ss_stat, wad_yes, "Colored numbers on the status bar");
  M_BindBool("sts_pct_always_gray", &sts_pct_always_gray, NULL,
             false, ss_stat, wad_yes,
             "Percent signs on the status bar are always gray");
  M_BindBool("st_solidbackground", &st_solidbackground, NULL,
             false, ss_stat, wad_no,
             "Use solid-color borders for the status bar in widescreen mode");
  M_BindBool("hud_animated_counts", &hud_animated_counts, NULL,
            false, ss_stat, wad_no, "Animated health/armor counts");
  M_BindBool("hud_armor_type", &hud_armor_type, NULL, false, ss_stat, wad_no,
             "Armor count is colored based on armor type");

  // [Nugget]
  M_BindBool("sts_show_berserk", &sts_show_berserk, NULL, true, ss_stat, wad_yes,
             "Show Berserk pack on the status bar when using the Fist, if available");

  M_BindNum("health_red", &health_red, NULL, 25, 0, 200, ss_none, wad_yes,
            "Amount of health for red-to-yellow transition");
  M_BindNum("health_yellow", &health_yellow, NULL, 50, 0, 200, ss_none, wad_yes,
            "Amount of health for yellow-to-green transition");
  M_BindNum("health_green", &health_green, NULL, 100, 0, 200, ss_none, wad_yes,
            "Amount of health for green-to-blue transition");
  M_BindNum("armor_red", &armor_red, NULL, 25, 0, 200, ss_none, wad_yes,
            "Amount of armor for red-to-yellow transition");
  M_BindNum("armor_yellow", &armor_yellow, NULL, 50, 0, 200, ss_none, wad_yes,
            "Amount of armor for yellow-to-green transition");
  M_BindNum("armor_green", &armor_green, NULL, 100, 0, 200, ss_none, wad_yes,
            "Amount of armor for green-to-blue transition");
  M_BindNum("ammo_red", &ammo_red, NULL, 25, 0, 100, ss_none, wad_yes,
            "Percent of ammo for red-to-yellow transition");
  M_BindNum("ammo_yellow", &ammo_yellow, NULL, 50, 0, 100, ss_none, wad_yes,
            "Percent of ammo for yellow-to-green transition");

  // [Nugget] Crosshair toggle
  M_BindBool("hud_crosshair_on", &hud_crosshair_on, NULL,
             false, ss_stat, wad_no, "Crosshair");

  // [Nugget] Crosshair type
  M_BindNum("hud_crosshair", &hud_crosshair, NULL, 1, 1, 10 - 1, ss_stat, wad_no,
            "Crosshair type");

  // [Nugget] Translucent crosshair
  M_BindNum("hud_crosshair_tran_pct", &hud_crosshair_tran_pct, NULL,
            100, 0, 100, ss_stat, wad_no,
            "Crosshair translucency percent");

  // [Cherry] Disable crosshair on slot 1
  M_BindBool("hud_crosshair_slot1_disable", &hud_crosshair_slot1_disable, NULL,
             false, ss_stat, wad_no,
             "Disable the crosshair when the weapon slot 1 is selected (Fist and Chainsaw)");

  M_BindBool("hud_crosshair_health", &hud_crosshair_health, NULL,
             false, ss_stat, wad_no, "Change crosshair color based on player health");
  M_BindNum("hud_crosshair_target", &hud_crosshair_target, NULL,
            0, 0, 2, ss_stat, wad_no,
            "Change crosshair color when locking on target (1 = Highlight; 2 = Health)");

  // [Nugget] Vertical-only option
  M_BindNum("hud_crosshair_lockon", &hud_crosshair_lockon, NULL,
            0, 0, 2, ss_stat, wad_no, "Lock crosshair on target (1 = Vertically only; 2 = Fully)");

  // [Nugget] Horizontal autoaim indicators
  M_BindBool("hud_crosshair_indicators", &hud_crosshair_indicators, NULL,
             false, ss_stat, wad_no, "Horizontal-autoaim indicators for crosshair");

  // [Nugget]
  M_BindBool("hud_crosshair_fuzzy", &hud_crosshair_fuzzy, NULL,
             false, ss_stat, wad_no, "Account for fuzzy targets when coloring and/or locking-on");

  // [Cherry] /----------------------------------------------------------------
  
  M_BindBool("hud_crosshair_dark", &hud_crosshair_dark, NULL,
             true, ss_stat, wad_no, "Account for targets in darkness when coloring and/or locking-on");

  M_BindNum("hud_crosshair_dark_level", &hud_crosshair_dark_level, NULL,
            120, 1, 256, ss_stat, wad_no, "Targets are not revealed if the light level is lower than this");

  // [Cherry] ----------------------------------------------------------------/

  M_BindNum("hud_crosshair_color", &hud_crosshair_color, NULL,
            CR_GRAY, CR_BRICK, CR_NONE, ss_stat, wad_no,
            "Default crosshair color");
  M_BindNum("hud_crosshair_target_color", &hud_crosshair_target_color, NULL,
            CR_YELLOW, CR_BRICK, CR_NONE, ss_stat, wad_no,
            "Crosshair color when aiming at target");
}

//----------------------------------------------------------------------------
//
// $Log: st_stuff.c,v $
// Revision 1.46  1998/05/06  16:05:40  jim
// formatting and documenting
//
// Revision 1.45  1998/05/03  22:50:58  killough
// beautification, move external declarations, remove cheats
//
// Revision 1.44  1998/04/27  17:30:39  jim
// Fix DM demo/newgame status, remove IDK (again)
//
// Revision 1.43  1998/04/27  02:30:12  killough
// fuck you
//
// Revision 1.42  1998/04/24  23:52:31  thldrmn
// Removed idk cheat
//
// Revision 1.41  1998/04/24  11:39:23  killough
// Fix cheats while demo is played back
//
// Revision 1.40  1998/04/19  01:10:19  killough
// Generalize cheat engine to add deh support
//
// Revision 1.39  1998/04/16  06:26:06  killough
// Prevent cheats from working inside menu
//
// Revision 1.38  1998/04/12  10:58:24  jim
// IDMUSxy for DOOM 1 fix
//
// Revision 1.37  1998/04/12  10:23:52  jim
// IDMUS00 ok in DOOM 1
//
// Revision 1.36  1998/04/12  02:00:39  killough
// Change tranmap to main_tranmap
//
// Revision 1.35  1998/04/12  01:08:51  jim
// Fixed IDMUS00 crash
//
// Revision 1.34  1998/04/11  14:48:11  thldrmn
// Replaced IDK with TNTKA cheat
//
// Revision 1.33  1998/04/10  06:36:45  killough
// Fix -fast parameter bugs
//
// Revision 1.32  1998/03/31  10:37:17  killough
// comment clarification
//
// Revision 1.31  1998/03/28  18:09:19  killough
// Fix deh-cheat self-annihilation bug, make iddt closer to Doom
//
// Revision 1.30  1998/03/28  05:33:02  jim
// Text enabling changes for DEH
//
// Revision 1.29  1998/03/23  15:24:54  phares
// Changed pushers to linedef control
//
// Revision 1.28  1998/03/23  06:43:26  jim
// linedefs reference initial version
//
// Revision 1.27  1998/03/23  03:40:46  killough
// Fix idclip bug, make monster kills message smart
//
// Revision 1.26  1998/03/20  00:30:37  phares
// Changed friction to linedef control
//
// Revision 1.25  1998/03/17  20:44:32  jim
// fixed idmus non-restore, space bug
//
// Revision 1.24  1998/03/12  14:35:01  phares
// New cheat codes
//
// Revision 1.23  1998/03/10  07:14:38  jim
// Initial DEH support added, minus text
//
// Revision 1.22  1998/03/09  07:31:48  killough
// Fix spy mode to display player correctly, add TNTFAST
//
// Revision 1.21  1998/03/06  05:31:02  killough
// PEst control, from the TNT'EM man
//
// Revision 1.20  1998/03/02  15:35:03  jim
// Enabled Lee's status changes, added new types to common.cfg
//
// Revision 1.19  1998/03/02  12:09:18  killough
// blue status bar color, monsters_remember, traditional_keys
//
// Revision 1.18  1998/02/27  11:00:58  phares
// Can't own weapons that don't exist
//
// Revision 1.17  1998/02/26  22:57:45  jim
// Added message review display to HUD
//
// Revision 1.16  1998/02/24  08:46:45  phares
// Pushers, recoil, new friction, and over/under work
//
// Revision 1.15  1998/02/24  04:14:19  jim
// Added double keys to status
//
// Revision 1.14  1998/02/23  04:57:29  killough
// Fix TNTEM cheat again, add new cheats
//
// Revision 1.13  1998/02/20  21:57:07  phares
// Preliminarey sprite translucency
//
// Revision 1.12  1998/02/19  23:15:52  killough
// Add TNTAMMO in addition to TNTAMO
//
// Revision 1.11  1998/02/19  16:55:22  jim
// Optimized HUD and made more configurable
//
// Revision 1.10  1998/02/18  00:59:20  jim
// Addition of HUD
//
// Revision 1.9  1998/02/17  06:15:48  killough
// Add TNTKEYxx, TNTAMOx, TNTWEAPx cheats, and cheat engine support for them.
//
// Revision 1.8  1998/02/15  02:48:01  phares
// User-defined keys
//
// Revision 1.7  1998/02/09  03:19:04  killough
// Rewrite cheat code engine, add IDK and TNTHOM
//
// Revision 1.6  1998/02/02  22:19:01  jim
// Added TNTEM cheat to kill every monster alive
//
// Revision 1.5  1998/01/30  18:48:10  phares
// Changed textspeed and textwait to functions
//
// Revision 1.4  1998/01/30  16:09:03  phares
// Faster end-mission text display
//
// Revision 1.3  1998/01/28  12:23:05  phares
// TNTCOMP cheat code added
//
// Revision 1.2  1998/01/26  19:24:58  phares
// First rev with no ^Ms
//
// Revision 1.1.1.1  1998/01/19  14:03:03  rand
// Lee's Jan 19 sources
//
//----------------------------------------------------------------------------
