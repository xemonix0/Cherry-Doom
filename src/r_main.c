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
//      Rendering main loop and setup functions,
//       utility functions (BSP, geometry, trigonometry).
//      See tables.c, too.
//
//-----------------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "d_loop.h"
#include "d_player.h"
#include "doomdata.h"
#include "doomdef.h"
#include "doomstat.h"
#include "i_video.h"
#include "p_mobj.h"
#include "p_pspr.h"
#include "p_setup.h" // P_SegLengths
#include "r_bsp.h"
#include "r_data.h"
#include "r_defs.h"
#include "r_draw.h"
#include "r_main.h"
#include "r_bmaps.h"
#include "r_plane.h"
#include "r_segs.h"
#include "r_sky.h"
#include "r_state.h"
#include "r_swirl.h"
#include "r_things.h"
#include "r_voxel.h"
#include "m_config.h"
#include "st_stuff.h"
#include "v_flextran.h"
#include "v_video.h"
#include "z_zone.h"

// [Nugget]
#include "g_game.h"
#include "m_nughud.h"
#include "m_random.h"
#include "p_map.h"
#include "p_user.h"
#include "s_sound.h"
#include "wi_stuff.h"

// Fineangles in the SCREENWIDTH wide window.
#define FIELDOFVIEW 2048

// killough: viewangleoffset is a legacy from the pre-v1.2 days, when Doom
// had Left/Mid/Right viewing. +/-ANG90 offsets were placed here on each
// node, by d_net.c, to set up a L/M/R session.

int viewangleoffset;
int validcount = 1;         // increment every time a check is made
lighttable_t *fixedcolormap;
int      centerx, centery;
fixed_t  centerxfrac, centeryfrac;
fixed_t  projection;
fixed_t  skyiscale,
         skyiscalediff; // [Nugget] FOV-based sky stretching
fixed_t  viewx, viewy, viewz;
angle_t  viewangle;
localview_t localview;
boolean raw_input;
fixed_t  viewcos, viewsin;
player_t *viewplayer;
fixed_t  viewheightfrac; // [FG] sprite clipping optimizations
int max_project_slope = 4;

static fixed_t focallength, lightfocallength;

//
// precalculated math tables
//

angle_t clipangle;
angle_t vx_clipangle;

// The viewangletox[viewangle + FINEANGLES/4] lookup
// maps the visible view angles to screen X coordinates,
// flattening the arc to a flat projection plane.
// There will be many angles mapped to the same X.

int viewangletox[FINEANGLES/2];

// The xtoviewangleangle[] table maps a screen pixel
// to the lowest viewangle that maps back to x ranges
// from clipangle to -clipangle.

angle_t *xtoviewangle = NULL;   // killough 2/8/98

// [FG] linear horizontal sky scrolling
angle_t *linearskyangle = NULL;

int LIGHTLEVELS;
int LIGHTSEGSHIFT;
int LIGHTBRIGHT;
int MAXLIGHTSCALE;
int LIGHTSCALESHIFT;
int MAXLIGHTZ;
int LIGHTZSHIFT;

// killough 3/20/98: Support dynamic colormaps, e.g. deep water
// killough 4/4/98: support dynamic number of them as well

int numcolormaps;
lighttable_t ***(*c_scalelight) = NULL;
lighttable_t ***(*c_zlight) = NULL;
lighttable_t **(*scalelight) = NULL;
lighttable_t **scalelightfixed = NULL;
lighttable_t **(*zlight) = NULL;
lighttable_t *fullcolormap;
lighttable_t **colormaps;

// killough 3/20/98, 4/4/98: end dynamic colormaps

int extralight;                           // bumped light from gun blasts
int extra_level_brightness;               // level brightness feature

// [Nugget] /=================================================================

// CVARs ---------------------------------------------------------------------

boolean flip_levels;
boolean nightvision_visor;
int fake_contrast;
boolean diminished_lighting;
static boolean a11y_weapon_flash;
boolean a11y_weapon_pspr;
boolean a11y_invul_colormap;
boolean translucent_pspr;
int translucent_pspr_pct;
int zoom_fov;
boolean comp_powerrunout;

// FOV effects ---------------------------------------------------------------

static boolean teleporter_zoom;

static float r_fov; // Rendered (currently applied) FOV, with effects added to it

static boolean keep_pspr_interp = false;

typedef struct fovfx_s {
  float old, current, target;
} fovfx_t;

static fovfx_t fovfx[NUMFOVFX]; // FOV effects (recoil, teleport)
static int     zoomed = 0;      // Current zoom state

void R_ClearFOVFX(void)
{
  R_SetZoom(ZOOM_RESET);

  for (int i = FOVFX_ZOOM+1;  i < NUMFOVFX;  i++)
  {
    fovfx[i] = (fovfx_t) { .target = 0, .current = 0, .old = 0 };
  }
}

int R_GetFOVFX(const int fx)
{
  return fovfx[fx].current;
}

void R_SetFOVFX(const int fx)
{
  if (strictmode) { return; }

  switch (fx) {
    case FOVFX_ZOOM:
      // Handled by `R_Get/SetZoom()`
      break;

    case FOVFX_TELEPORT:
      if (!teleporter_zoom) { break; }
      R_SetZoom(ZOOM_RESET);
      fovfx[FOVFX_TELEPORT].target = 50;
      break;
  }
}

int R_GetZoom(void)
{
  return zoomed;
}

void R_SetZoom(const int state)
{
  if (state == ZOOM_RESET || zoomed == ZOOM_RESET)
  {
    zoomed = ZOOM_RESET;
    return;
  }

  if (STRICTMODE(zoom_fov - custom_fov))
  {
    zoomed = state;
  }
  else { zoomed = ZOOM_OFF; }
}

// Explosion shake effect ----------------------------------------------------

boolean explosion_shake;
int explosion_shake_intensity_pct;

static fixed_t shake;
#define MAXSHAKE 50

void R_SetShake(int value)
{
  if (strictmode || !explosion_shake || value == -1)
  {
    shake = 0;
    return;
  }

  shake = MIN(shake + value, MAXSHAKE);
}

void R_ExplosionShake(fixed_t bombx, fixed_t bomby, int force, int range)
{
  #define SHAKERANGEMULT 5

  const mobj_t *const player = players[displayplayer].mo;
  fixed_t dx, dy, dist;

  if (strictmode || !explosion_shake) { return; }

  range *= SHAKERANGEMULT;
  force *= SHAKERANGEMULT;

  dx = abs(player->x - bombx);
  dy = abs(player->y - bomby);

  dist = MAX(dx, dy);
  dist = (dist - player->radius) >> FRACBITS;
  dist = MAX(0, dist);

  if (dist >= range) { return; }

  R_SetShake((force * (range - dist) / range) / ((128 / MAXSHAKE) * SHAKERANGEMULT));

  #undef SHAKERANGEMULT
}

// Chasecam ------------------------------------------------------------------

int chasecam_mode;
static int chasecam_distance;
static int chasecam_height;
boolean chasecam_crosshair;
static boolean death_camera;

static struct {
  fixed_t x, y, z;
  boolean hit;
} chasecam;

boolean chasecam_on = false;

// For Automap
fixed_t  chasexofs, chaseyofs;
angle_t  chaseaofs;

boolean R_GetChasecamOn(void)
{
  return chasecam_on;
}

void R_SetChasecamHit(const boolean value)
{
  chasecam.hit = value;
}

void R_UpdateChasecam(fixed_t x, fixed_t y, fixed_t z)
{
  chasecam.x = x;
  chasecam.y = y;
  chasecam.z = z;
}

// Freecam -------------------------------------------------------------------

static boolean       freecam_on = false;
static freecammode_t freecam_mode = FREECAM_OFF + 1;

static struct {
  fixed_t x, ox,
          y, oy,
          z, oz;
  angle_t angle, oangle;
  fixed_t pitch, opitch;

  int     reset;
  boolean interp, centering;
  mobj_t *mobj;
} freecam;

boolean R_GetFreecamOn(void)
{
  return freecam_on;
}

void R_SetFreecamOn(const boolean value)
{
  freecam_on = STRICTMODE(value);
}

freecammode_t R_GetFreecamMode(void)
{
  return freecam_on * freecam_mode;
}

freecammode_t R_CycleFreecamMode(void)
{
  return (++freecam_mode >= NUMFREECAMMODES) ? freecam_mode = FREECAM_OFF + 1 : freecam_mode;
}

angle_t R_GetFreecamAngle(void)
{
  return freecam.angle;
}

void R_ResetFreecam(const boolean newmap)
{
  freecam.reset = (int) true + newmap;
}

void R_MoveFreecam(fixed_t x, fixed_t y, fixed_t z)
{
  if (freecam.mobj) { R_UpdateFreecamMobj(NULL); }

  freecam.x = x;
  freecam.y = y;
  freecam.z = z;

  freecam.interp = false;
}

void R_UpdateFreecamMobj(mobj_t *const mobj)
{
  if (freecam.mobj) { freecam.mobj->thinker.references--; }
  if         (mobj) {         mobj->thinker.references++; }

  freecam.mobj = mobj;
}

const mobj_t *R_GetFreecamMobj(void)
{
  return freecam_on ? freecam.mobj : NULL;
}

void R_UpdateFreecam(fixed_t x, fixed_t y, fixed_t z, angle_t angle,
                     fixed_t pitch, boolean center, boolean lock)
{
  if (freecam.reset)
  {
    const player_t *const player = &players[displayplayer];

    freecam.x     = player->mo->x;
    freecam.y     = player->mo->y;
    freecam.z     = player->mo->z + player->viewheight;
    freecam.angle = player->mo->angle;
    freecam.pitch = player->pitch;

    freecam.interp = false;
    freecam.centering = false;

    if (freecam.reset == 2)
    { freecam.mobj = NULL; }
    else
    { R_UpdateFreecamMobj(NULL); }

    freecam.reset = false;

    return;
  }

  freecam.interp = true;

  if (lock)
  {
    if (freecam.mobj) {
      R_UpdateFreecamMobj(NULL);
    }
    else {
      const boolean intercepts_overflow_enabled = overflow[emu_intercepts].enabled;
      overflow[emu_intercepts].enabled = false;

      player_t dummyplayer = {0};
      dummyplayer.slope = P_PitchToSlope(freecam.pitch);

      mobj_t dummymo = {0};
      dummymo.x = freecam.x;
      dummymo.y = freecam.y;
      dummymo.z = freecam.z;
      dummymo.player = &dummyplayer;

      P_AimLineAttack(&dummymo, freecam.angle, 16*64*FRACUNIT * (comp_longautoaim+1), CROSSHAIR_AIM);

      overflow[emu_intercepts].enabled = intercepts_overflow_enabled;

      if (linetarget && linetarget != players[consoleplayer].mo)
      {
        R_UpdateFreecamMobj(linetarget);
        freecam.interp = false;
        freecam.mobj->interp = -1;
        freecam.pitch = 0;
      }
    }
  }

  freecam.ox     = freecam.x;
  freecam.oy     = freecam.y;
  freecam.oz     = freecam.z;
  freecam.oangle = freecam.angle;
  freecam.opitch = freecam.pitch;

  if (freecam.mobj && freecam.mobj->health > 0)
  {
    freecam.x     = freecam.mobj->x;
    freecam.y     = freecam.mobj->y;
    freecam.z     = freecam.mobj->z + (freecam.mobj->height * 13/16);
    freecam.angle = freecam.mobj->angle;
  }
  else {
    freecam.x     += x;
    freecam.y     += y;
    freecam.z     += z;
    freecam.angle += angle;

    R_UpdateFreecamMobj(NULL);
  }

  if ((freecam.centering |= center))
  {
    if (!(freecam.pitch = MAX(0, abs(freecam.pitch) - 4*ANG1) * ((freecam.pitch > 0) ? 1 : -1)))
    { freecam.centering = false; }
  }
  else { freecam.pitch = BETWEEN(-MAX_PITCH_ANGLE, MAX_PITCH_ANGLE, freecam.pitch + pitch); }
}

// [Nugget] =================================================================/

// [Cherry] CVARs
int rocket_trails_tran;

void (*colfunc)(void);                    // current column draw function

//
// R_PointOnSide
// Traverse BSP (sub) tree,
//  check point against partition plane.
// Returns side 0 (front) or 1 (back).
//
// killough 5/2/98: reformatted
//

// Workaround for optimization bug in clang
// fixes desync in competn/doom/fp2-3655.lmp and in dmnsns.wad dmn01m909.lmp
#if defined(__clang__)
int R_PointOnSide(volatile fixed_t x, volatile fixed_t y, node_t *node)
#else
int R_PointOnSide(fixed_t x, fixed_t y, node_t *node)
#endif
{
  if (!node->dx)
    return x <= node->x ? node->dy > 0 : node->dy < 0;

  if (!node->dy)
    return y <= node->y ? node->dx < 0 : node->dx > 0;

  x -= node->x;
  y -= node->y;

  // Try to quickly decide by looking at sign bits.
  if ((node->dy ^ node->dx ^ x ^ y) < 0)
    return (node->dy ^ x) < 0;  // (left is negative)
  return FixedMul(y, node->dx>>FRACBITS) >= FixedMul(node->dy>>FRACBITS, x);
}

// killough 5/2/98: reformatted

int R_PointOnSegSide(fixed_t x, fixed_t y, seg_t *line)
{
  fixed_t lx = line->v1->x;
  fixed_t ly = line->v1->y;
  fixed_t ldx = line->v2->x - lx;
  fixed_t ldy = line->v2->y - ly;

  if (!ldx)
    return x <= lx ? ldy > 0 : ldy < 0;

  if (!ldy)
    return y <= ly ? ldx < 0 : ldx > 0;

  x -= lx;
  y -= ly;

  // Try to quickly decide by looking at sign bits.
  if ((ldy ^ ldx ^ x ^ y) < 0)
    return (ldy ^ x) < 0;          // (left is negative)
  return FixedMul(y, ldx>>FRACBITS) >= FixedMul(ldy>>FRACBITS, x);
}

//
// R_PointToAngle
// To get a global angle from cartesian coordinates,
//  the coordinates are flipped until they are in
//  the first octant of the coordinate system, then
//  the y (<=x) is scaled and divided by x to get a
//  tangent (slope) value which is looked up in the
//  tantoangle[] table. The +1 size of tantoangle[]
//  is to handle the case when x==y without additional
//  checking.
//
// killough 5/2/98: reformatted, cleaned up

angle_t R_PointToAngle(fixed_t x, fixed_t y)
{
  return (y -= viewy, (x -= viewx) || y) ?
    x >= 0 ?
      y >= 0 ?
        (x > y) ? tantoangle[SlopeDiv(y,x)] :                      // octant 0
                ANG90-1-tantoangle[SlopeDiv(x,y)] :                // octant 1
        x > (y = -y) ? 0-tantoangle[SlopeDiv(y,x)] :               // octant 8
                       ANG270+tantoangle[SlopeDiv(x,y)] :          // octant 7
      y >= 0 ? (x = -x) > y ? ANG180-1-tantoangle[SlopeDiv(y,x)] : // octant 3
                            ANG90 + tantoangle[SlopeDiv(x,y)] :    // octant 2
        (x = -x) > (y = -y) ? ANG180+tantoangle[ SlopeDiv(y,x)] :  // octant 4
                              ANG270-1-tantoangle[SlopeDiv(x,y)] : // octant 5
    0;
}

angle_t R_PointToAngle2(fixed_t viewx, fixed_t viewy, fixed_t x, fixed_t y)
{
  return (y -= viewy, (x -= viewx) || y) ?
    x >= 0 ?
      y >= 0 ?
        (x > y) ? tantoangle[SlopeDiv(y,x)] :                      // octant 0
                ANG90-1-tantoangle[SlopeDiv(x,y)] :                // octant 1
        x > (y = -y) ? 0-tantoangle[SlopeDiv(y,x)] :               // octant 8
                       ANG270+tantoangle[SlopeDiv(x,y)] :          // octant 7
      y >= 0 ? (x = -x) > y ? ANG180-1-tantoangle[SlopeDiv(y,x)] : // octant 3
                            ANG90 + tantoangle[SlopeDiv(x,y)] :    // octant 2
        (x = -x) > (y = -y) ? ANG180+tantoangle[ SlopeDiv(y,x)] :  // octant 4
                              ANG270-1-tantoangle[SlopeDiv(x,y)] : // octant 5
    0;
}

// [FG] overflow-safe R_PointToAngle() flavor,
// only used in R_CheckBBox(), R_AddLine() and P_SegLengths()

angle_t R_PointToAngleCrispy(fixed_t x, fixed_t y)
{
  // [FG] fix overflows for very long distances
  int64_t y_viewy = (int64_t)y - viewy;
  int64_t x_viewx = (int64_t)x - viewx;

  // [FG] the worst that could happen is e.g. INT_MIN-INT_MAX = 2*INT_MIN
  if (x_viewx < INT_MIN || x_viewx > INT_MAX || y_viewy < INT_MIN || y_viewy > INT_MAX)
  {
    // [FG] preserving the angle by halfing the distance in both directions
    x = (int)(x_viewx / 2 + viewx);
    y = (int)(y_viewy / 2 + viewy);
  }

  return (y -= viewy, (x -= viewx) || y) ?
    x >= 0 ?
      y >= 0 ?
        (x > y) ? tantoangle[SlopeDivCrispy(y,x)] :                      // octant 0
                ANG90-1-tantoangle[SlopeDivCrispy(x,y)] :                // octant 1
        x > (y = -y) ? 0-tantoangle[SlopeDivCrispy(y,x)] :               // octant 8
                       ANG270+tantoangle[SlopeDivCrispy(x,y)] :          // octant 7
      y >= 0 ? (x = -x) > y ? ANG180-1-tantoangle[SlopeDivCrispy(y,x)] : // octant 3
                            ANG90 + tantoangle[SlopeDivCrispy(x,y)] :    // octant 2
        (x = -x) > (y = -y) ? ANG180+tantoangle[SlopeDivCrispy(y,x)] :  // octant 4
                              ANG270-1-tantoangle[SlopeDivCrispy(x,y)] : // octant 5
    0;
}

// WiggleFix: move R_ScaleFromGlobalAngle to r_segs.c,
// above R_StoreWallRange


// [crispy] in widescreen mode, make sure the same number of horizontal
// pixels shows the same part of the game scene as in regular rendering mode
static int scaledviewwidth_nonwide, viewwidth_nonwide;
static fixed_t centerxfrac_nonwide;

//
// CalcMaxProjectSlope
// Calculate the minimum divider needed to provide at least 45 degrees of FOV
// padding. For fast rejection during sprite/voxel projection.
//

static void CalcMaxProjectSlope(int fov)
{
  max_project_slope = 16;

  for (int i = 1; i < 16; i++)
  {
    if (atan(i) * FINEANGLES / M_PI - fov >= FINEANGLES / 8)
    {
      max_project_slope = i;
      break;
    }
  }
}

//
// R_InitTextureMapping
//
// killough 5/2/98: reformatted

static void R_InitTextureMapping(void)
{
  register int i,x;
  fixed_t slopefrac;
  angle_t fov;
  double linearskyfactor;

  // Use tangent table to generate viewangletox:
  //  viewangletox will give the next greatest x
  //  after the view angle.
  //
  // Calc focallength
  //  so FIELDOFVIEW angles covers SCREENWIDTH.

  // [Nugget] Use `r_fov` instead of `custom_fov`

  if (r_fov == FOV_DEFAULT && centerxfrac == centerxfrac_nonwide)
  {
    fov = FIELDOFVIEW;
    slopefrac = finetangent[FINEANGLES / 4 + fov / 2];
    focallength = FixedDiv(centerxfrac_nonwide, slopefrac);
    lightfocallength = centerxfrac_nonwide;
    projection = centerxfrac_nonwide;
  }
  else
  {
    const double slope = (tan(r_fov * M_PI / 360.0) *
                          centerxfrac / centerxfrac_nonwide);

    // For correct light across FOV range. Calculated like R_InitTables().
    const double lightangle = atan(slope) + M_PI / FINEANGLES;
    const double lightslopefrac = tan(lightangle) * FRACUNIT;
    lightfocallength = FixedDiv(centerxfrac, lightslopefrac);

    fov = atan(slope) * FINEANGLES / M_PI;
    slopefrac = finetangent[FINEANGLES / 4 + fov / 2];
    focallength = FixedDiv(centerxfrac, slopefrac);
    projection = centerxfrac / slope;
  }

  for (i=0 ; i<FINEANGLES/2 ; i++)
    {
      int t;
      if (finetangent[i] > slopefrac)
        t = -1;
      else
        if (finetangent[i] < -slopefrac)
          t = viewwidth+1;
      else
        {
          t = FixedMul(finetangent[i], focallength);
          t = (centerxfrac - t + FRACMASK) >> FRACBITS;
          if (t < -1)
            t = -1;
          else
            if (t > viewwidth+1)
              t = viewwidth+1;
        }
      viewangletox[i] = t;
    }
    
  // Scan viewangletox[] to generate xtoviewangle[]:
  //  xtoviewangle will give the smallest view angle
  //  that maps to x.

  linearskyfactor = FIXED2DOUBLE(slopefrac) * ANG90;

  for (x=0; x<=viewwidth; x++)
    {
      for (i=0; viewangletox[i] > x; i++)
        ;
      xtoviewangle[x] = (i<<ANGLETOFINESHIFT)-ANG90;
      // [FG] linear horizontal sky scrolling
      linearskyangle[x] = (0.5 - x / (double)viewwidth) * linearskyfactor;
    }
    
  // Take out the fencepost cases from viewangletox.
  for (i=0; i<FINEANGLES/2; i++)
    if (viewangletox[i] == -1)
      viewangletox[i] = 0;
    else 
      if (viewangletox[i] == viewwidth+1)
        viewangletox[i] = viewwidth;
        
  clipangle = xtoviewangle[0];

  vx_clipangle = clipangle - ((fov << ANGLETOFINESHIFT) - ANG90);
  CalcMaxProjectSlope(fov);
}

//
// R_InitLightTables
// Only inits the zlight table,
//  because the scalelight table changes with view size.
//

#define DISTMAP 2

boolean smoothlight;

void R_InitLightTables (void)
{
  int i, cm;

  if (c_scalelight)
  {
    for (cm = 0; cm < numcolormaps; ++cm)
    {
      for (i = 0; i < LIGHTLEVELS; ++i)
        Z_Free(c_scalelight[cm][i]);

      Z_Free(c_scalelight[cm]);
    }
    Z_Free(c_scalelight);
  }

  if (scalelightfixed)
  {
    Z_Free(scalelightfixed);
  }

  if (c_zlight)
  {
    for (cm = 0; cm < numcolormaps; ++cm)
    {
      for (i = 0; i < LIGHTLEVELS; ++i)
        Z_Free(c_zlight[cm][i]);

      Z_Free(c_zlight[cm]);
    }
    Z_Free(c_zlight);
  }

  if (smoothlight)
  {
      LIGHTLEVELS = 32;
      LIGHTSEGSHIFT = 3;
      LIGHTBRIGHT = 2;
      MAXLIGHTSCALE = 48;
      LIGHTSCALESHIFT = 12;
      MAXLIGHTZ = 1024;
      LIGHTZSHIFT = 17;
  }
  else
  {
      LIGHTLEVELS = 16;
      LIGHTSEGSHIFT = 4;
      LIGHTBRIGHT = 1;
      MAXLIGHTSCALE = 48;
      LIGHTSCALESHIFT = 12;
      MAXLIGHTZ = 128;
      LIGHTZSHIFT = 20;
  }

  scalelightfixed = Z_Malloc(MAXLIGHTSCALE * sizeof(*scalelightfixed), PU_STATIC, 0);

  // killough 4/4/98: dynamic colormaps
  c_zlight = Z_Malloc(sizeof(*c_zlight) * numcolormaps, PU_STATIC, 0);
  c_scalelight = Z_Malloc(sizeof(*c_scalelight) * numcolormaps, PU_STATIC, 0);

  for (cm = 0; cm < numcolormaps; ++cm)
  {
    c_zlight[cm] = Z_Malloc(LIGHTLEVELS * sizeof(**c_zlight), PU_STATIC, 0);
    c_scalelight[cm] = Z_Malloc(LIGHTLEVELS * sizeof(**c_scalelight), PU_STATIC, 0);
  }

  // Calculate the light levels to use
  //  for each level / distance combination.
  for (i=0; i< LIGHTLEVELS; i++)
    {
      int j, startmap = ((LIGHTLEVELS-LIGHTBRIGHT-i)*2)*NUMCOLORMAPS/LIGHTLEVELS;

      for (cm = 0; cm < numcolormaps; ++cm)
      {
        c_scalelight[cm][i] = Z_Malloc(MAXLIGHTSCALE * sizeof(***c_scalelight), PU_STATIC, 0);
        c_zlight[cm][i] = Z_Malloc(MAXLIGHTZ * sizeof(***c_zlight), PU_STATIC, 0);
      }

      for (j=0; j<MAXLIGHTZ; j++)
        {
          int scale = FixedDiv ((SCREENWIDTH/2*FRACUNIT), (j+1)<<LIGHTZSHIFT);
          int t, level = startmap - (scale >> LIGHTSCALESHIFT)/DISTMAP;

          if (level < 0)
            level = 0;
          else
            if (level >= NUMCOLORMAPS)
              level = NUMCOLORMAPS-1;

          // killough 3/20/98: Initialize multiple colormaps
          level *= 256;
          for (t=0; t<numcolormaps; t++)         // killough 4/4/98
            c_zlight[t][i][j] = colormaps[t] + level;
        }
    }
}

boolean setsmoothlight;

void R_SmoothLight(void)
{
  setsmoothlight = false;
  // [crispy] re-calculate the zlight[][] array
  R_InitLightTables();
  // [crispy] re-calculate the scalelight[][] array
  // R_ExecuteSetViewSize();
  // [crispy] re-calculate fake contrast
  P_SegLengths(true);
}

int R_GetLightIndex(fixed_t scale)
{
  const int index = ((int64_t)scale * (160 << FRACBITS) / lightfocallength) >> LIGHTSCALESHIFT;
  return BETWEEN(0, MAXLIGHTSCALE - 1, index);
}

static fixed_t viewpitch;

static void R_SetupFreelook(void)
{
  fixed_t dy;
  int i;

  if (viewpitch)
  {
    dy = FixedMul(projection, -finetangent[(ANG90 - viewpitch) >> ANGLETOFINESHIFT]);
  }
  else
  {
    dy = 0;
  }

  if (STRICTMODE(st_crispyhud) && gamestate == GS_LEVEL)
  {
    dy += (nughud.viewoffset * viewheight / SCREENHEIGHT) << FRACBITS;
  }

  centery = viewheight / 2 + (dy >> FRACBITS);
  centeryfrac = centery << FRACBITS;

  for (i = 0; i < viewheight; i++)
  {
    dy = abs(((i - centery) << FRACBITS) + FRACUNIT / 2);
    yslope[i] = FixedDiv(projection, dy);
  }
}


//
// R_SetViewSize
// Do not really change anything here,
//  because it might be in the middle of a refresh.
// The change will take effect next refresh.
//

boolean setsizeneeded;
int     setblocks;

void R_SetViewSize(int blocks)
{
  setsizeneeded = true;
  setblocks = blocks;
}

//
// R_ExecuteSetViewSize
//

void R_ExecuteSetViewSize (void)
{
  int i;
  vrect_t view;

  setsizeneeded = false;

  // [Nugget] Alt. intermission background
  if (WI_UsingAltInterpic() && gamestate == GS_INTERMISSION)
  { setblocks = 11; }

  if (setblocks == 11)
    {
      scaledviewwidth_nonwide = NONWIDEWIDTH;
      scaledviewwidth = video.unscaledw;
      scaledviewheight = SCREENHEIGHT;                    // killough 11/98
    }
  // [crispy] hard-code to SCREENWIDTH and SCREENHEIGHT minus status bar height
  else if (setblocks == 10)
    {
      scaledviewwidth_nonwide = NONWIDEWIDTH;
      scaledviewwidth = video.unscaledw;
      scaledviewheight = SCREENHEIGHT - ST_HEIGHT;
    }
  else
    {
      const int st_screen = SCREENHEIGHT - ST_HEIGHT;

      scaledviewwidth_nonwide = setblocks * 32;
      scaledviewheight = (setblocks * st_screen / 10) & ~7; // killough 11/98

      if (video.unscaledw > SCREENWIDTH)
        scaledviewwidth = (scaledviewheight * video.unscaledw / st_screen) & ~7;
      else
        scaledviewwidth = scaledviewwidth_nonwide;
    }

  scaledviewx = (video.unscaledw - scaledviewwidth) / 2;

  if (scaledviewwidth == video.unscaledw)
    scaledviewy = 0;
  else
    scaledviewy = (SCREENHEIGHT - ST_HEIGHT - scaledviewheight) / 2;

  view.x = scaledviewx;
  view.y = scaledviewy;
  view.w = scaledviewwidth;
  view.h = scaledviewheight;

  V_ScaleRect(&view);

  viewwindowx = view.sx;
  viewwindowy = view.sy;
  viewwidth   = view.sw;
  viewheight  = view.sh;

  viewwidth_nonwide = V_ScaleX(scaledviewwidth_nonwide);

  centerxfrac = (viewwidth << FRACBITS) / 2;
  centerx = centerxfrac >> FRACBITS;
  centerxfrac_nonwide = (viewwidth_nonwide << FRACBITS) / 2;

  viewheightfrac = viewheight << (FRACBITS + 1); // [FG] sprite clipping optimizations

  R_InitBuffer();       // killough 11/98

  R_InitTextureMapping();

  R_SetupFreelook();

  // psprite scales
  pspritescale = FixedDiv(viewwidth_nonwide, SCREENWIDTH);       // killough 11/98
  pspriteiscale = FixedDiv(SCREENWIDTH, viewwidth_nonwide);      // killough 11/98

  // [FG] make sure that the product of the weapon sprite scale factor
  //      and its reciprocal is always at least FRACUNIT to
  //      fix garbage lines at the top of weapon sprites
  while (FixedMul(pspriteiscale, pspritescale) < FRACUNIT)
    pspriteiscale++;

  // [Nugget] Use `r_fov` instead of `custom_fov`
  if (r_fov == FOV_DEFAULT)
  {
    skyiscale = FixedDiv(SCREENWIDTH, viewwidth_nonwide);
  }
  else
  {
    skyiscale = tan(r_fov * M_PI / 360.0) * SCREENWIDTH / viewwidth_nonwide * FRACUNIT;
  }

  // [Nugget] FOV-based sky stretching;
  // we intentionally use `custom_fov` to disregard any FOV effects
  if (custom_fov == FOV_DEFAULT)
  {
    skyiscalediff = FRACUNIT;
  }
  else {
    skyiscalediff = tan(custom_fov * M_PI / 360.0) * FRACUNIT;
  }

  for (i=0 ; i<viewwidth ; i++)
    {
      fixed_t cosadj = abs(finecosine[xtoviewangle[i]>>ANGLETOFINESHIFT]);
      distscale[i] = FixedDiv(FRACUNIT,cosadj);
      // thing clipping
      screenheightarray[i] = viewheight;
    }

  // Calculate the light levels to use
  //  for each level / scale combination.
  for (i=0; i<LIGHTLEVELS; i++)
    {
      int j, startmap = ((LIGHTLEVELS-LIGHTBRIGHT-i)*2)*NUMCOLORMAPS/LIGHTLEVELS;

      for (j=0 ; j<MAXLIGHTSCALE ; j++)
        {                                       // killough 11/98:
          int t, level = startmap - j / DISTMAP;

          if (level < 0)
            level = 0;

          if (level >= NUMCOLORMAPS)
            level = NUMCOLORMAPS-1;

          // killough 3/20/98: initialize multiple colormaps
          level *= 256;

          for (t=0; t<numcolormaps; t++)     // killough 4/4/98
            c_scalelight[t][i][j] = colormaps[t] + level;
        }
    }

  // [crispy] forcefully initialize the status bar backing screen
  // [Nugget] Unless the alt. intermission background is enabled
  if (!WI_UsingAltInterpic())
    ST_refreshBackground();

  // [Nugget]
  if (!keep_pspr_interp)
    pspr_interp = false;
}

//
// R_Init
//

void R_Init (void)
{
  r_fov = custom_fov; // [Nugget]

  R_InitData();
  R_SetViewSize(screenblocks);
  R_InitPlanes();
  R_InitLightTables();
  R_InitSkyMap();
  R_InitTranslationTables();
  V_InitFlexTranTable();

  // [FG] spectre drawing mode
  R_SetFuzzColumnMode();

  colfunc = R_DrawColumn;
  R_InitDrawFunctions();
}

//
// R_PointInSubsector
//
// killough 5/2/98: reformatted, cleaned up

subsector_t *R_PointInSubsector(fixed_t x, fixed_t y)
{
  int nodenum = numnodes-1;

  // [FG] fix crash when loading trivial single subsector maps
  if (!numnodes)
  {
    return subsectors;
  }

  while (!(nodenum & NF_SUBSECTOR))
    nodenum = nodes[nodenum].children[R_PointOnSide(x, y, nodes+nodenum)];
  return &subsectors[nodenum & ~NF_SUBSECTOR];
}

static inline boolean CheckLocalView(const player_t *player)
{
  // [Nugget] Freecam: use localview unless
  // locked onto a mobj, or not controlling the camera
  if (freecam_on && !(freecam.mobj || freecam_mode != FREECAM_CAM))
  { return true; }

  return (
    // Don't use localview if the player is spying.
    player == &players[consoleplayer] &&
    // Don't use localview if the player is dead.
    player->playerstate != PST_DEAD &&
    // Don't use localview if the player just teleported.
    !player->mo->reactiontime &&
    // Don't use localview if a demo is playing.
    !demoplayback &&
    // Don't use localview during a netgame (single-player or solo-net only).
    (!netgame || solonet)
  );
}

static angle_t CalcViewAngle_RawInput(const player_t *player)
{
  return (player->mo->angle + localview.angle - player->ticangle +
          LerpAngle(player->oldticangle, player->ticangle));
}

static angle_t CalcViewAngle_LerpFakeLongTics(const player_t *player)
{
  return LerpAngle(player->mo->oldangle + localview.oldlerpangle,
                   player->mo->angle + localview.lerpangle);
}

static angle_t (*CalcViewAngle)(const player_t *player);

void R_UpdateViewAngleFunction(void)
{
  if (raw_input)
  {
    CalcViewAngle = CalcViewAngle_RawInput;
  }
  else if (lowres_turn && fake_longtics)
  {
    CalcViewAngle = CalcViewAngle_LerpFakeLongTics;
  }
  else
  {
    CalcViewAngle = NULL;
  }
}

//
// R_SetupFrame
//

void R_SetupFrame (player_t *player)
{
  // [Nugget] Freecam
  if (freecam_on && gamestate == GS_LEVEL)
  {
    static player_t dummyplayer = {0};
    static mobj_t dummymobj = {0};

    if (freecam.mobj)
    {
      dummymobj = *freecam.mobj;
    }
    else {
      memset(&dummymobj, 0, sizeof(mobj_t));

      dummymobj.oldx = freecam.ox;
      dummymobj.x    = freecam.x;
      dummymobj.oldy = freecam.oy;
      dummymobj.y    = freecam.y;
      dummymobj.oldz = freecam.oz;
      dummymobj.z    = freecam.z;

      dummymobj.oldangle = freecam.oangle;
      dummymobj.angle    = freecam.angle;

      dummymobj.interp = freecam.interp;
      dummymobj.subsector = R_PointInSubsector(freecam.x, freecam.y);
    }

    dummyplayer.oldviewz = freecam.oz;
    dummyplayer.viewz    = freecam.z;
    dummyplayer.oldpitch = freecam.opitch;
    dummyplayer.pitch    = freecam.pitch;

    dummyplayer.centering = (dummyplayer.centering && freecam.opitch != freecam.pitch)
                          | freecam.centering;

    dummymobj.player = &dummyplayer;
    dummyplayer.mo = &dummymobj;
    player = &dummyplayer;
  }

  int i, cm;
  fixed_t pitch;
  const boolean use_localview = CheckLocalView(player);
  const boolean camera_ready = (
    // Don't interpolate on the first tic of a level,
    // otherwise oldviewz might be garbage.
    leveltime > 1 &&
    // Don't interpolate if the player did something
    // that would necessitate turning it off for a tic.
    player->mo->interp == true &&
    // Don't interpolate during a paused state
    (leveltime > oldleveltime
     || (freecam_on && !freecam.mobj && gamestate == GS_LEVEL)) // [Nugget] Freecam
  );

  // [Nugget]
  fixed_t playerz, basepitch;
  static angle_t old_interangle, target_interangle;
  static fixed_t chasecamheight;

  viewplayer = player;

  // [AM] Interpolate the player camera if the feature is enabled.
  if (uncapped && camera_ready)
  {
    // Interpolate player camera from their old position to their current one.
    viewx = LerpFixed(player->mo->oldx, player->mo->x);
    viewy = LerpFixed(player->mo->oldy, player->mo->y);
    viewz = LerpFixed(player->oldviewz, player->viewz);

    playerz = LerpFixed(player->mo->oldz, player->mo->z); // [Nugget]

    if (use_localview && CalcViewAngle)
    {
      viewangle = CalcViewAngle(player);
    }
    else
    {
      viewangle = LerpAngle(player->mo->oldangle, player->mo->angle);
    }

    if ((use_localview || (freecam_on && freecam_mode == FREECAM_CAM)) // [Nugget] Freecam
        && raw_input && !player->centering)
    {
      basepitch = player->pitch + localview.pitch;
      basepitch = BETWEEN(-MAX_PITCH_ANGLE, MAX_PITCH_ANGLE, basepitch);
    }
    else
    {
      basepitch = LerpFixed(player->oldpitch, player->pitch);
    }

    pitch = basepitch;

    // [crispy] pitch is actual lookdir and weapon pitch
    pitch += LerpFixed(player->oldrecoilpitch, player->recoilpitch);

    // [Nugget] Flinching
    pitch += LerpFixed(player->oldflinch, player->flinch);
  }
  else
  {
    viewx = player->mo->x;
    viewy = player->mo->y;
    viewz = player->viewz; // [FG] moved here
    viewangle = player->mo->angle;
    // [crispy] pitch is actual lookdir and weapon pitch
    basepitch = player->pitch;
    pitch = basepitch + player->recoilpitch;

    // [Nugget]
    playerz = player->mo->z;
    pitch += player->flinch; // Flinching

    if (camera_ready && use_localview && lowres_turn && fake_longtics)
    {
      viewangle += localview.angle;
    }
  }

  // [Nugget] /===============================================================

  // Alt. intermission background --------------------------------------------

  if (WI_UsingAltInterpic() && gamestate == GS_INTERMISSION)
  {
    static int oldtic = -1;

    if (oldtic != gametic) {
      old_interangle = viewangle = target_interangle;
      target_interangle += ANG1;
    }
    else if (uncapped)
    { viewangle = LerpAngle(old_interangle, target_interangle); }

    oldtic = gametic;

    basepitch = pitch = 0;
  }
  else { target_interangle = viewangle; }

  // Explosion shake effect --------------------------------------------------

  chasecamheight = R_GetFreecamMobj() ? freecam.z - freecam.mobj->z
                                      : chasecam_height * FRACUNIT;

  if (shake > 0)
  {
    static fixed_t xofs = 0, yofs = 0, zofs = 0;

    if (!((menuactive && !demoplayback && !netgame) || paused))
    {
      static int oldtime = -1;
      const fixed_t intensity = FRACUNIT * explosion_shake_intensity_pct / 100;

      #define CALCSHAKE (((Woof_Random() - 128) % 3) * intensity) * shake / MAXSHAKE
      xofs = CALCSHAKE;
      yofs = CALCSHAKE;
      zofs = CALCSHAKE;
      #undef CALCSHAKE

      if (oldtime != leveltime) { shake--; }

      oldtime = leveltime;
    }

    viewx += xofs;
    viewy += yofs;
    viewz += zofs;
    chasecamheight += zofs;
  }

  // Chasecam ----------------------------------------------------------------

  chasecam_on = gamestate == GS_LEVEL
                && STRICTMODE(chasecam_mode || (death_camera && player->mo->health <= 0 && player->playerstate == PST_DEAD))
                && !(freecam_on && !freecam.mobj);

  if (chasecam_on)
  {
    fixed_t slope = -P_PitchToSlope(basepitch);
    const angle_t oldviewangle = viewangle;

    if (chasecam_mode == CHASECAMMODE_FRONT)
    {
      viewangle += ANG180;
      slope      = -slope;
      basepitch  = -basepitch;
      pitch     += basepitch * 2;
    }

    static fixed_t oldeffort = 0, effort = 0,
                   oldcrouchoffset = 0, crouchoffset = 0;

    static int oldtic = -1;

    if (oldtic != gametic)
    {
      oldeffort = effort;
      oldcrouchoffset = crouchoffset;

      fixed_t momx, momy;

      if (demo_version < DV_MBF) {
        momx = player->mo->momx;
        momy = player->mo->momy;
      }
      else {
        momx = player->momx;
        momy = player->momy;
      }

      effort = FixedMul(momx, finecosine[viewangle >> ANGLETOFINESHIFT])
             + FixedMul(momy,   finesine[viewangle >> ANGLETOFINESHIFT]);

      crouchoffset = player->crouchoffset;

      if (gametic - oldtic > 1) {
        // Chasecam was disabled; reset forcefully
        oldeffort = effort;
        oldcrouchoffset = crouchoffset;
      }

      oldtic = gametic;
    }

    static int oldmode = 0;

    if (oldmode != chasecam_mode)
    {
      oldeffort = effort;
      oldmode = chasecam_mode;
    }

    fixed_t dist = chasecam_distance * FRACUNIT;

    fixed_t curcrouchoffset;

    if (uncapped && leveltime > 1 && player->mo->interp == true && leveltime > oldleveltime)
    {
      dist += LerpFixed(oldeffort, effort);
      curcrouchoffset = LerpFixed(oldcrouchoffset, crouchoffset);
    }
    else {
      dist += effort;
      curcrouchoffset = crouchoffset;
    }

    const fixed_t z = MIN(player->mo->ceilingz - (2*FRACUNIT),
                          playerz + ((player->mo->health <= 0 && player->playerstate == PST_DEAD)
                                     ? 6*FRACUNIT : chasecamheight - curcrouchoffset));

    P_PositionChasecam(z, dist, slope);

    const fixed_t oldviewx = viewx,
                  oldviewy = viewy;

    if (chasecam.hit) {
      viewx = chasecam.x;
      viewy = chasecam.y;
      viewz = chasecam.z;
    }
    else {
      const fixed_t dx = FixedMul(dist, finecosine[viewangle >> ANGLETOFINESHIFT]),
                    dy = FixedMul(dist,   finesine[viewangle >> ANGLETOFINESHIFT]);

      const sector_t *const sec = R_PointInSubsector(viewx-dx, viewy-dy)->sector;

      viewz = z + FixedMul(slope, dist);

      if (viewz < sec->floorheight+FRACUNIT || sec->ceilingheight-FRACUNIT < viewz)
      {
        fixed_t frac;

        viewz  = BETWEEN(sec->floorheight+FRACUNIT, sec->ceilingheight-FRACUNIT, viewz);
        frac   = FixedDiv(viewz - z, FixedMul(slope, dist));
        viewx -= FixedMul(dx, frac);
        viewy -= FixedMul(dy, frac);
      }
      else {
        viewx -= dx;
        viewy -= dy;
      }
    }

    chasexofs = viewx - oldviewx;
    chaseyofs = viewy - oldviewy;
    chaseaofs = viewangle - oldviewangle;
  }
  else { chasexofs = chaseyofs = chaseaofs = 0; }

  // [Nugget] ===============================================================/

  if (pitch != viewpitch)
  {
    viewpitch = pitch;
    R_SetupFreelook();
  }

  // 3-screen display mode.
  viewangle += viewangleoffset;

  // [Nugget]: [crispy] A11Y
  if (!(strictmode || a11y_weapon_flash))
    extralight = 0;
  else
    extralight = player->extralight;

  extralight += STRICTMODE(LIGHTBRIGHT * extra_level_brightness);

  viewsin = finesine[viewangle>>ANGLETOFINESHIFT];
  viewcos = finecosine[viewangle>>ANGLETOFINESHIFT];

  // killough 3/20/98, 4/4/98: select colormap based on player status

  if (player->mo->subsector->sector->heightsec != -1)
    {
      const sector_t *s = player->mo->subsector->sector->heightsec + sectors;
      cm = viewz < s->interpfloorheight ? s->bottommap : viewz > s->interpceilingheight ?
        s->topmap : s->midmap;
      if (cm < 0 || cm > numcolormaps)
        cm = 0;
    }
  else
    cm = 0;

  fullcolormap = colormaps[cm];
  zlight = c_zlight[cm];
  scalelight = c_scalelight[cm];

  if (player->fixedcolormap)
    {
      fixedcolormap = fullcolormap   // killough 3/20/98: use fullcolormap
        + player->fixedcolormap*256*sizeof(lighttable_t);

      walllights = scalelightfixed;

      for (i=0 ; i<MAXLIGHTSCALE ; i++)
        scalelightfixed[i] = fixedcolormap;
    }
  else
    fixedcolormap = 0;

  validcount++;
}

//
// R_ShowStats
//

int rendered_visplanes, rendered_segs, rendered_vissprites, rendered_voxels;

static void R_ClearStats(void)
{
  rendered_visplanes = 0;
  rendered_segs = 0;
  rendered_vissprites = 0;
  rendered_voxels = 0;
}

static boolean flashing_hom;
int autodetect_hom = 0;       // killough 2/7/98: HOM autodetection flag

static boolean no_killough_face; // [Nugget]

//
// R_RenderView
//
void R_RenderPlayerView (player_t* player)
{
  R_ClearStats();

  { // [Nugget] FOV effects
    float targetfov = custom_fov;

    if (WI_UsingAltInterpic() && gamestate == GS_INTERMISSION)
    {
      targetfov = MAX(140, targetfov);
    }
    else {
      static int oldtic = -1;

      int zoomtarget;

      // Force zoom reset
      if (strictmode || zoomed == ZOOM_RESET)
      {
        zoomtarget = 0;
        fovfx[FOVFX_ZOOM] = (fovfx_t) { .target = 0, .current = 0, .old = 0 };
        zoomed = ZOOM_OFF;
      }
      else {
        zoomtarget = zoomed ? zoom_fov - custom_fov : 0;

        // In case `custom_fov` changes while zoomed in...
        if (zoomed && fabs(fovfx[FOVFX_ZOOM].target) > abs(zoomtarget))
        { fovfx[FOVFX_ZOOM] = (fovfx_t) { .target = zoomtarget, .current = zoomtarget, .old = zoomtarget }; }
      }

      boolean fovchange = false;

      if (!strictmode)
      {
        if (fovfx[FOVFX_ZOOM].target != zoomtarget)
        {
          fovchange = true;
        }
        else if (fovfx[FOVFX_SLOWMO].target
                 || (G_GetSlowMotion() && fovfx[FOVFX_SLOWMO].target != 10))
        {
          fovchange = true;
        }
        else for (int i = 0;  i < NUMFOVFX;  i++)
        {
          if (fovfx[i].target || fovfx[i].current)
          {
            fovchange = true;
            break;
          }
        }
      }

      if (fovchange)
      {
        if (oldtic != gametic)
        {
          fovchange = false;

          float *target;

          // Zoom ------------------------------------------------------------

          target = &fovfx[FOVFX_ZOOM].target;
          fovfx[FOVFX_ZOOM].old = fovfx[FOVFX_ZOOM].current = *target;

          // Special handling for zoom
          if (zoomtarget || *target)
          {
            float step = zoomtarget - *target;
            const int sign = ((step > 0) ? 1 : -1);

            *target += BETWEEN(1, 16, fabs(step) / 3.0) * sign;

            if (   (sign > 0 && *target > zoomtarget)
                || (sign < 0 && *target < zoomtarget))
            {
              *target = zoomtarget;
            }
          }

          // Teleporter Zoom -------------------------------------------------

          target = &fovfx[FOVFX_TELEPORT].target;
          fovfx[FOVFX_TELEPORT].old = fovfx[FOVFX_TELEPORT].current = *target;

          if (*target) { *target = MAX(0, *target - 5); }

          // Slow Motion -----------------------------------------------------

          target = &fovfx[FOVFX_SLOWMO].target;
          fovfx[FOVFX_SLOWMO].old = fovfx[FOVFX_SLOWMO].current = *target;

          if (G_GetSlowMotionFactor() != SLOWMO_FACTOR_NORMAL)
          {
            *target = -10 * (SLOWMO_FACTOR_NORMAL - G_GetSlowMotionFactor())
                          / (SLOWMO_FACTOR_NORMAL - SLOWMO_FACTOR_TARGET);
          }
          else { *target = 0; }
        }
        else if (uncapped)
          for (int i = 0;  i < NUMFOVFX;  i++)
          { fovfx[i].current = fovfx[i].old + ((fovfx[i].target - fovfx[i].old) * ((float) fractionaltic/FRACUNIT)); }
      }

      oldtic = gametic;

      for (int i = 0;  i < NUMFOVFX;  i++)
      {
        if (FOVFX_ZOOM < i && freecam_on) { break; }

        targetfov += fovfx[i].current;
      }
    }

    if (r_fov != targetfov)
    {
      r_fov = targetfov;

      keep_pspr_interp = true;
      R_ExecuteSetViewSize();
      keep_pspr_interp = false;
    }
  }

  R_SetupFrame (player);

  // Clear buffers.
  R_ClearClipSegs ();
  R_ClearDrawSegs ();
  R_ClearPlanes ();
  R_ClearSprites ();
  VX_ClearVoxels ();

  if (autodetect_hom)
    { // killough 2/10/98: add flashing red HOM indicators
      pixel_t c[47*47];
      int i , color = !flashing_hom || (gametic % 20) < 9 ? 0xb0 : 0;
      V_FillRect(scaledviewx, scaledviewy, scaledviewwidth, scaledviewheight, color);
      for (i=0;i<47*47;i++)
        {
          char t =
"/////////////////////////////////////////////////////////////////////////////"
"/////////////////////////////////////////////////////////////////////////////"
"///////jkkkkklk////////////////////////////////////hkllklklkklkj/////////////"
"///////////////////jkkkkklklklkkkll//////////////////////////////kkkkkklklklk"
"lkkkklk//////////////////////////jllkkkkklklklklklkkklk//////////////////////"
"//klkkllklklklkllklklkkklh//////////////////////kkkkkjkjjkkj\3\205\214\3lllkk"
"lkllh////////////////////kllkige\211\210\207\206\205\204\203\203\203\205`\206"
"\234\234\234\234kkllg//////////////////klkkjhfe\210\206\203\203\203\202\202"
"\202\202\202\202\203\205`\207\211eikkk//////////////////kkkk\3g\211\207\206"
"\204\203\202\201\201\200\200\200\200\200\201\201\202\204b\210\211\3lkh///////"
"//////////lklki\213\210b\206\203\201\201\200\200\200\200\200Z\200\200\200\202"
"\203\204\205\210\211jll/////////////////lkkk\3\212\210b\205\202\201\200\200"
"\200XW\200\200\200\200\200\200\202\203\204\206\207eklj////////////////lkkjg"
"\211b\206\204\202\200\200\200YWWX\200Z\200\200\200\202\203\203\205bdjkk//////"
"//////////llkig\211a\205\203\202\200\200\200YXWX\200\200\200\200\200\201\202"
"\203\203\206\207ekk////////////////lkki\3\211\206\204\202\201\200\200XXWWWXX"
"\200\200\200\200\202\202\204\206\207ekk////////////////lkkj\3e\206\206\204\\"
"\200\200XWVVWWWXX\200\200\200\\\203\205\207\231kk////////////////lkkjjgcccfd"
"\207\203WVUVW\200\200\202\202\204\204\205\204\206\210gkk////////////////kkkkj"
"e``\210hjjgb\200W\200\205\206fhghcbdcdfkk////////////////jkkj\3\207ab\211e"
"\213j\3g\204XX\207\213jii\212\207\203\204\210gfkj///////////////j\211lkjf\210"
"\214\3\3kj\213\213\211\205X\200\205\212\210\213\213\213\211\210\203\205gelj//"
"////////////hf\211\213kh\212\212i\212gkh\202\203\210\210\202\201\206\207\206"
"\\kkhf\210aabkk//////////////je\210\210\3g\210\207\210e\210c\205\204\202\210"
"\207\203\202\210\205\203\203fjbe\213\210bbieW/////////////ke\207\206ie\206"
"\203\203\203\205\205\204\203\210\211\207\202\202\206\210\203\204\206\207\210"
"\211\231\206\206`\206\206]/////////////kf\\\202ig\204\203\202\201\\\202\202"
"\205\207\210\207\203\202\206\206\206\205\203\203\203\202\202\203\204b\206\204"
"Z/////////////i\3\\\204j\212\204\202\201\200\202\202\202\203\206\211\210\203"
"\203c\205\202\201\201\201\200\200\201\202\204a\204\201W/////////////j\3\207"
"\210jh\206\202\200\200\200\200\200\202\206\211\205\202\202bb\201\200\200\200"
"\200\200\200\202\203b\\WW/////////////jke\206jic\203\201\200\200\200\200\202"
"\211\211\201\200\200\204\210\201\200\200W\200\200\200\201\204c\\\200]////////"
"//////kd\210\3\3e\205\202\200\200W\200\202\211\210\210\201\202\207\210\203"
"\200WWW\200\200\202\205d\\\202///////////////kkdhigb\203\201\200\200\200\202"
"\206\210\210\205\210\211\206\203\200WWW\200\201\203ce\203\205////////////////"
"ijkig\211\203\201\200\200\202\206\207\207\205\206\207\210\206\203\200\200WW"
"\200\203\206ce\202_//////////////////jig\210\203\202\200\201\206\210\210\205"
"\204\204\205\206\206\204\202\200\200\200\200\203bcd////////////////////hjgc"
"\205\202\201\203\206\210\206\204\204\202\202\204\205\206\204\200\200\200\201"
"\206\207c//////////////////////j\3\207\204\203\202\202\211c\204\201W\200\200"
"\203\205\206\203\200\200\200\203\206b///////////////////////ihd\204\203\202"
"\201\207f\205VTVTW\202\210\206Z\200\200\203aa////////////////////////jg\204"
"\204\203\201\202\210\211\211c\206\205\210d\210\200\200\200\202\204ac/////////"
"///////////////j\3b\203\203\202\202\205\207\206\205\207\207\206\206\202\200"
"\201\202\203ac/////////////////////////iid\206\204\203\202\204\205\377\205"
"\204\205\204\203\201\200\202\203\203bc//////////////////////////ej\207\205"
"\203\201\202\202\203\207\204\203\202\202\201\201\203\203bd///////////////////"
"////////ee\3a\204\201\200\201\202\205\203\201\200\200\201\202\204\205cc//////"
"//////////////////////c\3ec\203\201\200\200\201\202\201\200\200\202\203\206cc"
"//////////////////////////////c\3f\206\203\201\200\200\200\200\200\201\203bdc"
"////////////////////////////////g\3\211\206\202\\\201\200\201\202\203dde/////"
"/////////////////////////////\234\3db\203\203\203\203adec////////////////////"
"/////////////////hffed\211de////////////////////"[i];
          c[i] = t=='/' ? color : t;
        }
      if (gametic-lastshottic < TICRATE*2 && gametic-lastshottic > TICRATE/8
          && !no_killough_face) // [Nugget]
        V_DrawBlock(scaledviewx +  scaledviewwidth/2 - 24,
                    scaledviewy + scaledviewheight/2 - 24, 47, 47, c);
      R_DrawViewBorder();
    }

  // check for new console commands.
  NetUpdate ();

  // The head node is the last node output.
  R_RenderBSPNode (numnodes-1);

  R_NearbySprites ();

  // [FG] update automap while playing
  if (automap_on)
    return;

  // Check for new console commands.
  NetUpdate ();

  R_DrawPlanes ();

  // Check for new console commands.
  NetUpdate ();

  // [crispy] draw fuzz effect independent of rendering frame rate
  R_SetFuzzPosDraw();
  R_DrawMasked ();

  // Check for new console commands.
  NetUpdate ();
}

void R_InitAnyRes(void)
{
  R_InitSpritesRes();
  R_InitBufferRes();
  R_InitPlanesRes();
}

void R_BindRenderVariables(void)
{
  BIND_NUM_GENERAL(extra_level_brightness, 0, -8, 8, "Level brightness"); // [Nugget] Broader light-level range
  // [Cherry] Option to stretch short skies only when mouselook is enabled
  BIND_NUM_GENERAL(stretchsky, STRETCHSKY_OFF, STRETCHSKY_OFF, STRETCHSKY_MOUSELOOK,
                   "Stretch short skies for mouselook"); // [Nugget] Extended description

  // [Nugget] FOV-based sky stretching (CFG-only)
  BIND_BOOL(fov_stretchsky, true, "Stretch skies based on FOV");

  BIND_BOOL_GENERAL(linearsky, false, "Linear horizontal scrolling for skies");
  BIND_BOOL_GENERAL(r_swirl, false, "Swirling animated flats");
  BIND_BOOL_GENERAL(smoothlight, false, "Smooth diminishing lighting");

  // [Nugget] /---------------------------------------------------------------

  M_BindNum("fake_contrast", &fake_contrast, NULL, 1, 0, 2, ss_gen, wad_yes,
            "Fake contrast for walls (0 = Off, 1 = Smooth, 2 = Vanilla)");

  // (CFG-only)
  M_BindBool("diminished_lighting", &diminished_lighting, NULL,
             true, ss_none, wad_yes, "Diminished lighting (light emitted by player)");

  // [Nugget] ---------------------------------------------------------------/

  // [Cherry] Floating powerups from International Doom
  M_BindBool("floating_powerups", &floating_powerups, NULL,
             false, ss_gen, wad_yes, "Enable floating Megasphere, Supercharge, Invuln and Invis powerups");

  M_BindBool("voxels_rendering", &default_voxels_rendering, &voxels_rendering,
             true, ss_none, wad_no, "Allow voxel models");
  BIND_BOOL_GENERAL(brightmaps, false,
    "Brightmaps for textures and sprites");
  BIND_NUM_GENERAL(invul_mode, INVUL_MBF, INVUL_VANILLA, INVUL_GRAY,
    "Invulnerability effect (0 = Vanilla; 1 = MBF; 2 = Gray)");

  // [Nugget] /---------------------------------------------------------------

  BIND_BOOL_GENERAL(flip_levels, false, "Flip levels horizontally (visual filter)");

  BIND_BOOL_GENERAL(no_berserk_tint, false, "Disable Berserk tint");
  BIND_BOOL_GENERAL(no_radsuit_tint, false, "Disable Radiation Suit tint");

  M_BindBool("nightvision_visor", &nightvision_visor, NULL,
             false, ss_gen, wad_yes, "Night-vision effect for the light amplification visor");

  BIND_NUM_GENERAL(damagecount_cap, 100, 0, 100, "Player damage tint cap");
  BIND_NUM_GENERAL(bonuscount_cap, -1, -1, 100, "Player bonus tint cap");
  
  // [Nugget] ---------------------------------------------------------------/

  BIND_BOOL(flashing_hom, true, "Enable flashing of the HOM indicator");

  // [Nugget] (CFG-only)
  BIND_BOOL(no_killough_face, false, "Disable the Killough-face easter egg");

  BIND_NUM(screenblocks, 10, 3, 11, "Size of game-world screen");

  M_BindBool("translucency", &translucency, NULL, true, ss_gen, wad_yes,
             "Translucency for some things");
  M_BindNum("tran_filter_pct", &tran_filter_pct, NULL,
            66, 0, 100, ss_gen, wad_yes,
            "Percent of foreground/background translucency mix");

  M_BindBool("flipcorpses", &flipcorpses, NULL, false, ss_enem, wad_no,
             "Randomly mirrored death animations");
  M_BindBool("fuzzcolumn_mode", &fuzzcolumn_mode, NULL, true, ss_enem, wad_no,
             "Fuzz rendering (0 = Resolution-dependent; 1 = Blocky)");

  // [Nugget - ceski] Selective fuzz darkening
  M_BindBool("fuzzdark_mode", &fuzzdark_mode, NULL, false, ss_enem, wad_no,
             "Selective fuzz darkening");

  BIND_BOOL(draw_nearby_sprites, true,
    "Draw sprites overlapping into visible sectors");

  // [Nugget] ----------------------------------------------------------------

  M_BindNum("viewheight_value", &viewheight_value, NULL, 41, 32, 56, ss_gen, wad_yes,
            "Height of player's POV");

  M_BindNum("flinching", &flinching, NULL, 0, 0, 3, ss_gen, wad_yes,
            "Flinch player view (0 = Off; 1 = Upon landing; 2 = Upon taking damage; 3 = Upon either)");

  M_BindBool("explosion_shake", &explosion_shake, NULL,
             false, ss_gen, wad_yes, "Explosions shake the view");

  // (CFG-only)
  M_BindNum("explosion_shake_intensity_pct", &explosion_shake_intensity_pct, NULL,
            100, 10, 100, ss_none, wad_yes,
            "Explosion-shake intensity percent");

  M_BindBool("breathing", &breathing, NULL,
             false, ss_gen, wad_yes, "Imitate player's breathing (subtle idle bobbing)");

  M_BindBool("teleporter_zoom", &teleporter_zoom, NULL,
             false, ss_gen, wad_yes, "Zoom effect when teleporting");

  M_BindBool("death_camera", &death_camera, NULL,
             false, ss_gen, wad_yes, "Force third-person perspective upon death");

  M_BindNum("chasecam_mode", &chasecam_mode, NULL, 0, 0, 2, ss_gen, wad_no,
            "Chasecam mode (0 = Off; 1 = Back; 2 = Front)");

  M_BindNum("chasecam_distance", &chasecam_distance, NULL, 80, 1, 128, ss_gen, wad_no,
            "Chasecam distance");

  M_BindNum("chasecam_height", &chasecam_height, NULL, 48, 1, 64, ss_gen, wad_no,
            "Chasecam height");

  // (CFG-only)
  M_BindBool("chasecam_crosshair", &chasecam_crosshair, NULL,
             false, ss_none, wad_no, "Allow crosshair when using Chasecam");

  BIND_BOOL_GENERAL(a11y_weapon_flash,    true, "Allow weapon light flashes");
  BIND_BOOL_GENERAL(a11y_weapon_pspr,     true, "Allow rendering of weapon muzzleflash");
  BIND_BOOL_GENERAL(a11y_invul_colormap,  true, "Allow Invulnerability colormap");

  // [Cherry] /----------------------------------------------------------------

  BIND_NUM_GENERAL(rocket_trails_tran, 50, 0, 100, "Rocket smoke translucency percentage");

  BIND_BOOL_GENERAL(less_blinding_tints, false, "Less blinding tints");
}

//----------------------------------------------------------------------------
//
// $Log: r_main.c,v $
// Revision 1.13  1998/05/07  00:47:52  killough
// beautification
//
// Revision 1.12  1998/05/03  23:00:14  killough
// beautification, fix #includes and declarations
//
// Revision 1.11  1998/04/07  15:24:15  killough
// Remove obsolete HOM detector
//
// Revision 1.10  1998/04/06  04:47:46  killough
// Support dynamic colormaps
//
// Revision 1.9  1998/03/23  03:37:14  killough
// Add support for arbitrary number of colormaps
//
// Revision 1.8  1998/03/16  12:44:12  killough
// Optimize away some function pointers
//
// Revision 1.7  1998/03/09  07:27:19  killough
// Avoid using FP for point/line queries
//
// Revision 1.6  1998/02/17  06:22:45  killough
// Comment out audible HOM alarm for now
//
// Revision 1.5  1998/02/10  06:48:17  killough
// Add flashing red HOM indicator for TNTHOM cheat
//
// Revision 1.4  1998/02/09  03:22:17  killough
// Make TNTHOM control HOM detector, change array decl to MAX_*
//
// Revision 1.3  1998/02/02  13:29:41  killough
// comment out dead code, add HOM detector
//
// Revision 1.2  1998/01/26  19:24:42  phares
// First rev with no ^Ms
//
// Revision 1.1.1.1  1998/01/19  14:03:02  rand
// Lee's Jan 19 sources
//
//
//----------------------------------------------------------------------------
