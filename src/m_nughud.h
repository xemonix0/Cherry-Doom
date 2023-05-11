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
//  Variant of m_misc.h specifically for declaration and loading of NUGHUD
//  variables

#ifndef __M_NUGHUD__
#define __M_NUGHUD__

#include "m_fixed.h"

#define DELTA(x) ((abs(x) == 2) ? WIDESCREENDELTA*(x)/2                :  \
                  (abs(x) == 1) ? WIDESCREENDELTA*(x)*st_widecrispyhud : 0)

typedef struct nughud_widget_s {
  int x, y;
  int wide;
} nughud_widget_t;

typedef struct nughud_textline_s {
  int x, y;
  int wide;
  int align;
} nughud_textline_t;

typedef struct nughud_patch_s {
  int x, y;
  int wide;
  char *name;
} nughud_patch_t;

#define NUMNUGHUDPATCHES 8

typedef struct nughud_s {
  nughud_widget_t   ammo;
  nughud_widget_t   health;
  nughud_widget_t   arms[9];
  nughud_widget_t   frags;
  nughud_widget_t   face;
  boolean           face_bg;
  nughud_widget_t   armor;
  nughud_widget_t   keys[3];
  nughud_widget_t   ammos[4];
  nughud_widget_t   maxammos[4];
  nughud_textline_t time;
  boolean           time_sts;
  nughud_textline_t sts;
  nughud_textline_t title;
  nughud_textline_t coord;
  nughud_textline_t fps;
  nughud_patch_t    patches[NUMNUGHUDPATCHES];
  fixed_t           weapheight;
  // These determine whether or not a given NUGHUD font should be used
  boolean           nhtnum, nhamnum, nhwpnum, nhkeys;
} nughud_t;

extern nughud_t nughud;

void M_NughudLoadDefaults(void);
void M_NughudLoadOptions(void);

#endif
