//  Copyright (C) 1999 by
//  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
//  Copyright(C) 2020-2021 Fabian Greffrath
//  Copyright(C) 2021-2023 Alaux
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
// DESCRIPTION:
//  Variant of m_misc.c specifically for declaration and loading of NUGHUD
//  variables

#include "m_menu.h"
#include "w_wad.h"
#include "i_video.h"
#include "p_mobj.h"
#include "hu_stuff.h"
#include "st_stuff.h"
#include "m_misc.h"
#include "m_nughud.h"

#include "m_io.h"
#include <errno.h>

nughud_t nughud; // Behold!!!

//
// DEFAULTS
//

#define WIDGET_X(n, m, v) { n, (config_t *)&(m).x,     NULL, { v }, { -1, 320 }, number }
#define WIDGET_Y(n, m, v) { n, (config_t *)&(m).y,     NULL, { v }, { 0,  200 }, number }
#define WIDGET_W(n, m, v) { n, (config_t *)&(m).wide,  NULL, { v }, { -2, 2   }, number }

#define TEXTLINE_X(n, m, v) { n, (config_t *)&(m).x,     NULL, { v }, { 0,  320 }, number }
#define TEXTLINE_Y(n, m, v) { n, (config_t *)&(m).y,     NULL, { v }, { 0,  200 }, number }
#define TEXTLINE_W(n, m, v) { n, (config_t *)&(m).wide,  NULL, { v }, { -2, 2   }, number }
#define TEXTLINE_A(n, m, v) { n, (config_t *)&(m).align, NULL, { v }, { -1, 1   }, number }

#define PATCH_X(n, m) { n, (config_t *)&(m).x,    NULL, { 0         }, { 0,  320 }, number }
#define PATCH_Y(n, m) { n, (config_t *)&(m).y,    NULL, { 0         }, { 0,  200 }, number }
#define PATCH_W(n, m) { n, (config_t *)&(m).wide, NULL, { 0         }, { -2, 2   }, number }
#define PATCH_N(n, m) { n, (config_t *)&(m).name, NULL, { .s = NULL }, { 0       }, string }

#define TOGGLE(n, m, v) { n, (config_t *)&(m),       NULL, { v }, { 0,  1   }, number }

default_t nughud_defaults[] = {
  WIDGET_X(   "nughud_ammo_x",        nughud.ammo,             ST_AMMOX       ),
  WIDGET_Y(   "nughud_ammo_y",        nughud.ammo,             ST_AMMOY       ),
  WIDGET_W(   "nughud_ammo_wide",     nughud.ammo,             -1             ),
  WIDGET_X(   "nughud_health_x",      nughud.health,           ST_HEALTHX     ),
  WIDGET_Y(   "nughud_health_y",      nughud.health,           ST_HEALTHY     ),
  WIDGET_W(   "nughud_health_wide",   nughud.health,           -1             ),
  WIDGET_X(   "nughud_arms1_x",       nughud.arms[0],          -1             ),
  WIDGET_Y(   "nughud_arms1_y",       nughud.arms[0],          0              ),
  WIDGET_W(   "nughud_arms1_wide",    nughud.arms[0],          0              ),
  WIDGET_X(   "nughud_arms2_x",       nughud.arms[1],          111            ),
  WIDGET_Y(   "nughud_arms2_y",       nughud.arms[1],          172            ),
  WIDGET_W(   "nughud_arms2_wide",    nughud.arms[1],          -1             ),
  WIDGET_X(   "nughud_arms3_x",       nughud.arms[2],          119            ),
  WIDGET_Y(   "nughud_arms3_y",       nughud.arms[2],          172            ),
  WIDGET_W(   "nughud_arms3_wide",    nughud.arms[2],          -1             ),
  WIDGET_X(   "nughud_arms4_x",       nughud.arms[3],          127            ),
  WIDGET_Y(   "nughud_arms4_y",       nughud.arms[3],          172            ),
  WIDGET_W(   "nughud_arms4_wide",    nughud.arms[3],          -1             ),
  WIDGET_X(   "nughud_arms5_x",       nughud.arms[4],          135            ),
  WIDGET_Y(   "nughud_arms5_y",       nughud.arms[4],          172            ),
  WIDGET_W(   "nughud_arms5_wide",    nughud.arms[4],          -1             ),
  WIDGET_X(   "nughud_arms6_x",       nughud.arms[5],          111            ),
  WIDGET_Y(   "nughud_arms6_y",       nughud.arms[5],          182            ),
  WIDGET_W(   "nughud_arms6_wide",    nughud.arms[5],          -1             ),
  WIDGET_X(   "nughud_arms7_x",       nughud.arms[6],          119            ),
  WIDGET_Y(   "nughud_arms7_y",       nughud.arms[6],          182            ),
  WIDGET_W(   "nughud_arms7_wide",    nughud.arms[6],          -1             ),
  WIDGET_X(   "nughud_arms8_x",       nughud.arms[7],          127            ),
  WIDGET_Y(   "nughud_arms8_y",       nughud.arms[7],          182            ),
  WIDGET_W(   "nughud_arms8_wide",    nughud.arms[7],          -1             ),
  WIDGET_X(   "nughud_arms9_x",       nughud.arms[8],          135            ),
  WIDGET_Y(   "nughud_arms9_y",       nughud.arms[8],          182            ),
  WIDGET_W(   "nughud_arms9_wide",    nughud.arms[8],          -1             ),
  WIDGET_X(   "nughud_frags_x",       nughud.frags,            174            ),
  WIDGET_Y(   "nughud_frags_y",       nughud.frags,            171            ),
  WIDGET_W(   "nughud_frags_wide",    nughud.frags,            0              ),
  WIDGET_X(   "nughud_face_x",        nughud.face,             -1             ),
  WIDGET_Y(   "nughud_face_y",        nughud.face,             ST_FACESY      ),
  WIDGET_W(   "nughud_face_wide",     nughud.face,             0              ),
  TOGGLE(     "nughud_face_bg",       nughud.face_bg,          1              ),
  WIDGET_X(   "nughud_armor_x",       nughud.armor,            ST_ARMORX      ),
  WIDGET_Y(   "nughud_armor_y",       nughud.armor,            ST_ARMORY      ),
  WIDGET_W(   "nughud_armor_wide",    nughud.armor,            1              ),
  WIDGET_X(   "nughud_key0_x",        nughud.keys[0],          ST_KEY0X       ),
  WIDGET_Y(   "nughud_key0_y",        nughud.keys[0],          ST_KEY0Y       ),
  WIDGET_W(   "nughud_key0_wide",     nughud.keys[0],          1              ),
  WIDGET_X(   "nughud_key1_x",        nughud.keys[1],          ST_KEY1X       ),
  WIDGET_Y(   "nughud_key1_y",        nughud.keys[1],          ST_KEY1Y       ),
  WIDGET_W(   "nughud_key1_wide",     nughud.keys[1],          1              ),
  WIDGET_X(   "nughud_key2_x",        nughud.keys[2],          ST_KEY2X       ),
  WIDGET_Y(   "nughud_key2_y",        nughud.keys[2],          ST_KEY2Y       ),
  WIDGET_W(   "nughud_key2_wide",     nughud.keys[2],          1              ),
  WIDGET_X(   "nughud_ammo0_x",       nughud.ammos[0],         ST_AMMO0X      ),
  WIDGET_Y(   "nughud_ammo0_y",       nughud.ammos[0],         ST_AMMO0Y      ),
  WIDGET_W(   "nughud_ammo0_wide",    nughud.ammos[0],         1              ),
  WIDGET_X(   "nughud_ammo1_x",       nughud.ammos[1],         ST_AMMO1X      ),
  WIDGET_Y(   "nughud_ammo1_y",       nughud.ammos[1],         ST_AMMO1Y      ),
  WIDGET_W(   "nughud_ammo1_wide",    nughud.ammos[1],         1              ),
  WIDGET_X(   "nughud_ammo2_x",       nughud.ammos[2],         ST_AMMO2X      ),
  WIDGET_Y(   "nughud_ammo2_y",       nughud.ammos[2],         ST_AMMO2Y      ),
  WIDGET_W(   "nughud_ammo2_wide",    nughud.ammos[2],         1              ),
  WIDGET_X(   "nughud_ammo3_x",       nughud.ammos[3],         ST_AMMO3X      ),
  WIDGET_Y(   "nughud_ammo3_y",       nughud.ammos[3],         ST_AMMO3Y      ),
  WIDGET_W(   "nughud_ammo3_wide",    nughud.ammos[3],         1              ),
  WIDGET_X(   "nughud_maxammo0_x",    nughud.maxammos[0],      ST_MAXAMMO0X   ),
  WIDGET_Y(   "nughud_maxammo0_y",    nughud.maxammos[0],      ST_MAXAMMO0Y   ),
  WIDGET_W(   "nughud_maxammo0_wide", nughud.maxammos[0],      1              ),
  WIDGET_X(   "nughud_maxammo1_x",    nughud.maxammos[1],      ST_MAXAMMO1X   ),
  WIDGET_Y(   "nughud_maxammo1_y",    nughud.maxammos[1],      ST_MAXAMMO1Y   ),
  WIDGET_W(   "nughud_maxammo1_wide", nughud.maxammos[1],      1              ),
  WIDGET_X(   "nughud_maxammo2_x",    nughud.maxammos[2],      ST_MAXAMMO2X   ),
  WIDGET_Y(   "nughud_maxammo2_y",    nughud.maxammos[2],      ST_MAXAMMO2Y   ),
  WIDGET_W(   "nughud_maxammo2_wide", nughud.maxammos[2],      1              ),
  WIDGET_X(   "nughud_maxammo3_x",    nughud.maxammos[3],      ST_MAXAMMO3X   ),
  WIDGET_Y(   "nughud_maxammo3_y",    nughud.maxammos[3],      ST_MAXAMMO3Y   ),
  WIDGET_W(   "nughud_maxammo3_wide", nughud.maxammos[3],      1              ),
  TEXTLINE_X( "nughud_time_x",        nughud.time,             2              ),
  TEXTLINE_Y( "nughud_time_y",        nughud.time,             151            ),
  TEXTLINE_W( "nughud_time_wide",     nughud.time,             -2             ),
  TEXTLINE_A( "nughud_time_align",    nughud.time,             -1             ),
  TOGGLE(     "nughud_time_sts",      nughud.time_sts,         1              ),
  TEXTLINE_X( "nughud_sts_x",         nughud.sts,              2              ),
  TEXTLINE_Y( "nughud_sts_y",         nughud.sts,              159            ),
  TEXTLINE_W( "nughud_sts_wide",      nughud.sts,              -2             ),
  TEXTLINE_A( "nughud_sts_align",     nughud.sts,              -1             ),
  TEXTLINE_X( "nughud_title_x",       nughud.title,            2              ),
  TEXTLINE_Y( "nughud_title_y",       nughud.title,            143            ),
  TEXTLINE_W( "nughud_title_wide",    nughud.title,            -2             ),
  TEXTLINE_A( "nughud_title_align",   nughud.title,            -1             ),
  TEXTLINE_X( "nughud_coord_x",       nughud.coord,            318            ),
  TEXTLINE_Y( "nughud_coord_y",       nughud.coord,            0              ),
  TEXTLINE_W( "nughud_coord_wide",    nughud.coord,            2              ),
  TEXTLINE_A( "nughud_coord_align",   nughud.coord,            1              ),
  TEXTLINE_X( "nughud_fps_x",         nughud.fps,              318            ),
  TEXTLINE_Y( "nughud_fps_y",         nughud.fps,              8              ),
  TEXTLINE_W( "nughud_fps_wide",      nughud.fps,              2              ),
  TEXTLINE_A( "nughud_fps_align",     nughud.fps,              1              ),
  PATCH_X(    "nughud_patch1_x",      nughud.patches[0]                       ),
  PATCH_Y(    "nughud_patch1_y",      nughud.patches[0]                       ),
  PATCH_W(    "nughud_patch1_wide",   nughud.patches[0]                       ),
  PATCH_N(    "nughud_patch1_name",   nughud.patches[0]                       ),
  PATCH_X(    "nughud_patch2_x",      nughud.patches[1]                       ),
  PATCH_Y(    "nughud_patch2_y",      nughud.patches[1]                       ),
  PATCH_W(    "nughud_patch2_wide",   nughud.patches[1]                       ),
  PATCH_N(    "nughud_patch2_name",   nughud.patches[1]                       ),
  PATCH_X(    "nughud_patch3_x",      nughud.patches[2]                       ),
  PATCH_Y(    "nughud_patch3_y",      nughud.patches[2]                       ),
  PATCH_W(    "nughud_patch3_wide",   nughud.patches[2]                       ),
  PATCH_N(    "nughud_patch3_name",   nughud.patches[2]                       ),
  PATCH_X(    "nughud_patch4_x",      nughud.patches[3]                       ),
  PATCH_Y(    "nughud_patch4_y",      nughud.patches[3]                       ),
  PATCH_W(    "nughud_patch4_wide",   nughud.patches[3]                       ),
  PATCH_N(    "nughud_patch4_name",   nughud.patches[3]                       ),
  PATCH_X(    "nughud_patch5_x",      nughud.patches[4]                       ),
  PATCH_Y(    "nughud_patch5_y",      nughud.patches[4]                       ),
  PATCH_W(    "nughud_patch5_wide",   nughud.patches[4]                       ),
  PATCH_N(    "nughud_patch5_name",   nughud.patches[4]                       ),
  PATCH_X(    "nughud_patch6_x",      nughud.patches[5]                       ),
  PATCH_Y(    "nughud_patch6_y",      nughud.patches[5]                       ),
  PATCH_W(    "nughud_patch6_wide",   nughud.patches[5]                       ),
  PATCH_N(    "nughud_patch6_name",   nughud.patches[5]                       ),
  PATCH_X(    "nughud_patch7_x",      nughud.patches[6]                       ),
  PATCH_Y(    "nughud_patch7_y",      nughud.patches[6]                       ),
  PATCH_W(    "nughud_patch7_wide",   nughud.patches[6]                       ),
  PATCH_N(    "nughud_patch7_name",   nughud.patches[6]                       ),
  PATCH_X(    "nughud_patch8_x",      nughud.patches[7]                       ),
  PATCH_Y(    "nughud_patch8_y",      nughud.patches[7]                       ),
  PATCH_W(    "nughud_patch8_wide",   nughud.patches[7]                       ),
  PATCH_N(    "nughud_patch8_name",   nughud.patches[7]                       ),
  { "nughud_weapheight", (config_t *)&nughud.weapheight, NULL, { 0 }, { 0, 200 }, number },

  { NULL }         // last entry
};

#define NUMNUGHUDDEFAULTS ((unsigned)(sizeof nughud_defaults / sizeof *nughud_defaults - 1))

// killough 11/98: hash function for name lookup
static unsigned nughud_default_hash(const char *name)
{
  unsigned hash = 0;
  while (*name)
    hash = hash*2 + toupper(*name++);
  return hash % NUMNUGHUDDEFAULTS;
}


static default_t *M_NughudLookupDefault(const char *name)
{
  static int hash_init;
  register default_t *dp;

  // Initialize hash table if not initialized already
  if (!hash_init)
    for (hash_init = 1, dp = nughud_defaults; dp->name; dp++)
    {
      unsigned h = nughud_default_hash(dp->name);
      dp->next = nughud_defaults[h].first;
      nughud_defaults[h].first = dp;
    }

  // Look up name in hash table
  for (dp = nughud_defaults[nughud_default_hash(name)].first;
       dp && strcasecmp(name, dp->name);
       dp = dp->next);

  return dp;
}


static boolean M_NughudParseOption(const char *p, boolean wad)
{
  char name[80], strparm[1024];
  default_t *dp;
  int parm;

  while (isspace(*p)) { p++; } // killough 10/98: skip leading whitespace

  //jff 3/3/98 skip lines not starting with an alphanum
  // killough 10/98: move to be made part of main test, add comment-handling

  if (sscanf(p, "%79s %1023[^\n]", name, strparm) != 2 || !isalnum(*name)
      || !(dp = M_NughudLookupDefault(name))
      || (*strparm == '"') == (dp->type != string))
  { return 1; }

  if (dp->type == string)     // get a string default
  {
    int len = strlen(strparm)-1;

    while (isspace(strparm[len])) { len--; }

    if (strparm[len] == '"') { len--; }

    strparm[len+1] = 0;

    if (wad && !dp->modified) {             // Modified by wad
      dp->modified = 1;                     // Mark it as modified
      dp->orig_default.s = dp->location->s; // Save original default
    }
    else { free(dp->location->s); } // Free old value

    dp->location->s = strdup(strparm+1); // Change default value

    if (dp->current) {                    // Current value
      free(dp->current->s);               // Free old value
      dp->current->s = strdup(strparm+1); // Change current value
    }
  }
  else if (dp->type == number) {
    if (sscanf(strparm, "%i", &parm) != 1) { return 1; } // Not A Number

    //jff 3/4/98 range check numeric parameters
    if ((dp->limit.min == UL || dp->limit.min <= parm)
        && (dp->limit.max == UL || dp->limit.max >= parm))
    {
      if (wad) {
        if (!dp->modified) { // First time it's modified by wad
          dp->modified = 1;                     // Mark it as modified
          dp->orig_default.i = dp->location->i; // Save original default
        }
        if (dp->current) // Change current value
        { dp->current->i = parm; }
      }
      dp->location->i = parm;          // Change default
    }
  }

  if (wad && dp->setup_menu) { dp->setup_menu->m_flags |= S_DISABLE; }

  return 0; // Success
}


void M_NughudLoadOptions(void)
{
  int lump;

  if ((lump = W_CheckNumForName("NUGHUD")) != -1)
  {
    int size = W_LumpLength(lump), buflen = 0;
    char *buf = NULL, *p, *options = p = W_CacheLumpNum(lump, PU_STATIC);
    while (size > 0)
    {
      int len = 0;
      while (len < size && p[len++] && p[len-1] != '\n');
      if (len >= buflen)
      { buf = I_Realloc(buf, buflen = len+1); }
      strncpy(buf, p, len)[len] = 0;
      p += len;
      size -= len;
      M_NughudParseOption(buf, true);
    }
    free(buf);
    Z_ChangeTag(options, PU_CACHE);
  }
}


void M_NughudLoadDefaults (void)
{
  register default_t *dp;

  for (dp = nughud_defaults; dp->name; dp++)
    if (dp->type == string && dp->defaultvalue.s)
    { dp->location->s = strdup(dp->defaultvalue.s); }
    else if (dp->type == number)
    { dp->location->i = dp->defaultvalue.i; }
}
