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
// DESCRIPTION:
//      Mission start screen wipe/melt, special effects.
//
//-----------------------------------------------------------------------------

#ifndef __F_WIPE_H__
#define __F_WIPE_H__

//
// SCREEN WIPE PACKAGE
//

extern int wipe_speed_percentage; // [Nugget]

enum {
  wipe_None,
  wipe_Melt,        // weird screen melt
  wipe_ColorXForm,
  wipe_Fizzle,
  wipe_BlackFade,   // [Nugget] Black fade
  wipe_NUMWIPES
};

int wipe_ScreenWipe (int wipeno,
                     int x, int y, int width, int height, int ticks);
int wipe_StartScreen(int x, int y, int width, int height);
int wipe_EndScreen  (int x, int y, int width, int height);

#endif

//----------------------------------------------------------------------------
//
// $Log: f_wipe.h,v $
// Revision 1.3  1998/05/03  22:11:27  killough
// beautification
//
// Revision 1.2  1998/01/26  19:26:49  phares
// First rev with no ^Ms
//
// Revision 1.1.1.1  1998/01/19  14:02:54  rand
// Lee's Jan 19 sources
//
//----------------------------------------------------------------------------
