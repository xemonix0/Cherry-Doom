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
//      Weapon sprite animation, weapon objects.
//      Action functions for weapons.
//
//-----------------------------------------------------------------------------

#include "d_event.h"
#include "d_items.h"
#include "d_player.h"
#include "doomstat.h"
#include "g_nextweapon.h"
#include "i_printf.h"
#include "i_video.h" // uncapped
#include "m_random.h"
#include "p_action.h"
#include "p_enemy.h"
#include "p_inter.h"
#include "p_map.h"
#include "p_mobj.h"
#include "p_pspr.h"
#include "p_tick.h"
#include "p_user.h"
#include "r_main.h"
#include "s_sound.h"
#include "sounds.h"
#include "tables.h"

// [Nugget]
#include "g_game.h"
#include "m_input.h"
#include "p_maputl.h"
#include "w_wad.h" // W_CheckNumForName

// [Nugget] CVARs
boolean weapswitch_interruption;
boolean always_bob;
boolean weaponsquat;
boolean sx_fix;
boolean comp_nomeleesnap;

#define LOWERSPEED   (FRACUNIT*6)
#define RAISESPEED   (FRACUNIT*6)
#define WEAPONBOTTOM (FRACUNIT*128)
#define WEAPONTOP    (FRACUNIT*32)

#define BFGCELLS bfgcells        /* Ty 03/09/98 externalized in p_inter.c */

// The following array holds the recoil values         // phares
static struct
{
  int thrust;
  int pitch;
} recoil_values[] = {    // phares
  { 10, 0 },   // wp_fist
  { 10, 2 },   // wp_pistol
  { 30, 4 },   // wp_shotgun
  { 10, 2 },   // wp_chaingun
  { 100, 7 }, // wp_missile
  { 20, 2 },   // wp_plasma
  { 100, 9 }, // wp_bfg
  { 0, -1 },   // wp_chainsaw
  { 80, 7 }   // wp_supershotgun
};

// [crispy] add weapon recoil pitch
boolean weapon_recoilpitch;

void A_Recoil(player_t* player)
{
    if (player && weapon_recoilpitch)
    {
        player->recoilpitch = recoil_values[player->readyweapon].pitch * ANG1;
    }
}

//
// P_SetPsprite
//

static void P_SetPsprite(player_t *player, int position, statenum_t stnum)
{
  if (position == ps_weapon)
  {
    const weaponinfo_t wp = weaponinfo[player->readyweapon];

    if (stnum == wp.upstate)
      player->switching = weapswitch_raising;
    else if (stnum == wp.downstate)
      player->switching = weapswitch_lowering;
  }

  P_SetPspritePtr(player, &player->psprites[position], stnum);
}

//
// mbf21: P_SetPspritePtr
//

void P_SetPspritePtr(player_t *player, pspdef_t *psp, statenum_t stnum)
{
  do {
    state_t *state;

      if (!stnum)
        {
          // object removed itself
          psp->state = NULL;
          break;
        }

      // killough 7/19/98: Pre-Beta BFG
      if (stnum == S_BFG1 && (STRICTMODE(classic_bfg) || beta_emulation))
	stnum = S_OLDBFG1;                 // Skip to alternative weapon frame

      state = &states[stnum];
      psp->state = state;
      psp->tics = state->tics;        // could be 0

      if (state->misc1)
        {
          // coordinate set
          psp->sx = state->misc1 << FRACBITS;
          psp->sy = state->misc2 << FRACBITS;
          // [FG] centered weapon sprite
          // [Nugget] If applicable, subtract 1 pixel from the misc1 calculation,
          // for consistency with the first person sprite centering correction
          psp->sx2 = (state->misc1 - STRICTMODE(sx_fix)) << FRACBITS;
          psp->sy2 = state->misc2 << FRACBITS;
        }

      // Call action routine.
      // Modified handling.
      if (state->action.p2)
        {
          state->action.p2(player, psp);
          if (!psp->state)
            break;
        }
      stnum = psp->state->nextstate;
  } while (!psp->tics);     // an initial state of 0 could cycle through
}

//
// P_BringUpWeapon
// Starts bringing the pending weapon up
// from the bottom of the screen.
// Uses player
//

static boolean switch_interrupted = false; // [Nugget] Weapon-switch interruption

static void P_BringUpWeapon(player_t *player)
{
  statenum_t newstate;

  if (player->pendingweapon == wp_nochange)
    player->pendingweapon = player->readyweapon;

  if (player->pendingweapon == wp_chainsaw)
    S_StartSoundPitchEx(player->mo, sfx_sawup, PITCH_HALF);

  if (player->pendingweapon >= NUMWEAPONS)
  {
    I_Printf(VB_WARNING, "P_BringUpWeapon: weaponinfo overrun has occurred.");
  }

  newstate = weaponinfo[player->pendingweapon].upstate;

  player->pendingweapon = wp_nochange;

  // [Nugget] Weapon-switch interruption
  if (switch_interrupted)
  {
    switch_interrupted = false;
  }
  else
  // killough 12/98: prevent pistol from starting visibly at bottom of screen:
  player->psprites[ps_weapon].sy2 = // [Nugget]
  player->psprites[ps_weapon].sy = demo_version >= DV_MBF ? 
    WEAPONBOTTOM+FRACUNIT*2 : WEAPONBOTTOM;

  // [Nugget]: [crispy] squat down weapon sprite
  player->psprites[ps_weapon].dy = 0;
  // [Nugget] Reset offsets for weapon inertia
  player->psprites[ps_weapon].wix = 0;
  player->psprites[ps_weapon].wiy = 0;

  P_SetPsprite(player, ps_weapon, newstate);
}

// The first set is where the weapon preferences from             // killough,
// default.cfg are stored. These values represent the keys used   // phares
// in DOOM2 to bring up the weapon, i.e. 6 = plasma gun. These    //    |
// are NOT the wp_* constants.                                    //    V

int weapon_preferences[2][NUMWEAPONS+1] = {
  {6, 9, 4, 3, 2, 8, 5, 7, 1, 0},  // !compatibility preferences
  {6, 9, 4, 3, 2, 8, 5, 7, 1, 0},  //  compatibility preferences
};

// mbf21: [XA] fixed version of P_SwitchWeapon that correctly
// takes each weapon's ammotype and ammopershot into account,
// instead of blindly assuming both.

static int P_SwitchWeaponMBF21(player_t *player)
{
  int *prefer;
  int currentweapon, newweapon;
  int i;
  weapontype_t checkweapon;
  ammotype_t ammotype;

  prefer = weapon_preferences[0];
  currentweapon = player->readyweapon;
  newweapon = currentweapon;
  i = NUMWEAPONS + 1;

  do
  {
    checkweapon = wp_nochange;
    switch (*prefer++)
    {
      case 1:
        if (!player->powers[pw_strength]) // allow chainsaw override
          break;
        // fallthrough
      case 0:
        checkweapon = wp_fist;
        break;
      case 2:
        checkweapon = wp_pistol;
        break;
      case 3:
        checkweapon = wp_shotgun;
        break;
      case 4:
        checkweapon = wp_chaingun;
        break;
      case 5:
        checkweapon = wp_missile;
        break;
      case 6:
        if (gamemode != shareware)
          checkweapon = wp_plasma;
        break;
      case 7:
        if (gamemode != shareware)
          checkweapon = wp_bfg;
        break;
      case 8:
        checkweapon = wp_chainsaw;
        break;
      case 9:
        if (ALLOW_SSG)
          checkweapon = wp_supershotgun;
        break;
    }

    if (checkweapon != wp_nochange && player->weaponowned[checkweapon])
    {
      ammotype = weaponinfo[checkweapon].ammo;
      if (ammotype == am_noammo ||
        player->ammo[ammotype] >= weaponinfo[checkweapon].ammopershot)
        newweapon = checkweapon;
    }
  }
  while (newweapon == currentweapon && --i);
  return newweapon;
}

// P_SwitchWeapon checks current ammo levels and gives you the
// most preferred weapon with ammo. It will not pick the currently
// raised weapon. When called from P_CheckAmmo this won't matter,
// because the raised weapon has no ammo anyway. When called from
// G_BuildTiccmd you want to toggle to a different weapon regardless.

int P_SwitchWeapon(player_t *player)
{
  int *prefer = weapon_preferences[demo_compatibility!=0]; // killough 3/22/98
  int currentweapon = player->readyweapon;
  int newweapon = currentweapon;
  int i = NUMWEAPONS+1;   // killough 5/2/98

  G_NextWeaponReset();

  // [XA] use fixed behavior for mbf21. no need
  // for a discrete compat option for this, as
  // it doesn't impact demo playback (weapon
  // switches are saved in the demo itself)
  if (mbf21)
    return P_SwitchWeaponMBF21(player);

  // killough 2/8/98: follow preferences and fix BFG/SSG bugs

  do
    switch (*prefer++)
      {
      case 1:
        if (!player->powers[pw_strength])      // allow chainsaw override
          break;
      case 0:
        newweapon = wp_fist;
        break;
      case 2:
        if (player->ammo[am_clip])
          newweapon = wp_pistol;
        break;
      case 3:
        if (player->weaponowned[wp_shotgun] && player->ammo[am_shell])
          newweapon = wp_shotgun;
        break;
      case 4:
        if (player->weaponowned[wp_chaingun] && player->ammo[am_clip])
          newweapon = wp_chaingun;
        break;
      case 5:
        if (player->weaponowned[wp_missile] && player->ammo[am_misl])
          newweapon = wp_missile;
        break;
      case 6:
        if (player->weaponowned[wp_plasma] && player->ammo[am_cell] &&
            gamemode != shareware)
          newweapon = wp_plasma;
        break;
      case 7:
        if (player->weaponowned[wp_bfg] && gamemode != shareware &&
            player->ammo[am_cell] >= (demo_compatibility ? 41 : 40))
          newweapon = wp_bfg;
        break;
      case 8:
        if (player->weaponowned[wp_chainsaw])
          newweapon = wp_chainsaw;
        break;
      case 9:
        if (player->weaponowned[wp_supershotgun] && ALLOW_SSG &&
            player->ammo[am_shell] >= (demo_compatibility ? 3 : 2))
          newweapon = wp_supershotgun;
        break;
      }
  while (newweapon==currentweapon && --i);          // killough 5/2/98
  return newweapon;
}

// killough 5/2/98: whether consoleplayer prefers weapon w1 over weapon w2.
int P_WeaponPreferred(int w1, int w2)
{
  return
    (weapon_preferences[0][0] != ++w2 && (weapon_preferences[0][0] == ++w1 ||
    (weapon_preferences[0][1] !=   w2 && (weapon_preferences[0][1] ==   w1 ||
    (weapon_preferences[0][2] !=   w2 && (weapon_preferences[0][2] ==   w1 ||
    (weapon_preferences[0][3] !=   w2 && (weapon_preferences[0][3] ==   w1 ||
    (weapon_preferences[0][4] !=   w2 && (weapon_preferences[0][4] ==   w1 ||
    (weapon_preferences[0][5] !=   w2 && (weapon_preferences[0][5] ==   w1 ||
    (weapon_preferences[0][6] !=   w2 && (weapon_preferences[0][6] ==   w1 ||
    (weapon_preferences[0][7] !=   w2 && (weapon_preferences[0][7] ==   w1
   ))))))))))))))));
}

//
// P_CheckAmmo
// Returns true if there is enough ammo to shoot.
// If not, selects the next weapon to use.
// (only in demo_compatibility mode -- killough 3/22/98)
//

boolean P_CheckAmmo(player_t *player)
{
  ammotype_t ammo = weaponinfo[player->readyweapon].ammo;
  int count = 1;  // Regular
  
  if (player->cheats & CF_INFAMMO) { return true; } // [Nugget] Yikes!

  if (mbf21)
    count = weaponinfo[player->readyweapon].ammopershot;
  else
  if (player->readyweapon == wp_bfg)  // Minimal amount for one shot varies.
    count = BFGCELLS;
  else
    if (player->readyweapon == wp_supershotgun)        // Double barrel.
      count = 2;

  // Some do not need ammunition anyway.
  // Return if current ammunition sufficient.

  if (ammo == am_noammo || player->ammo[ammo] >= count)
    return true;

  // Out of ammo, pick a weapon to change to.
  //
  // killough 3/22/98: for old demos we do the switch here and now;
  // for Boom games we cannot do this, and have different player
  // preferences across demos or networks, so we have to use the
  // G_BuildTiccmd() interface instead of making the switch here.

  if (demo_compatibility)
    {
      player->pendingweapon = P_SwitchWeapon(player);      // phares
      // Now set appropriate weapon overlay.
      P_SetPsprite(player,ps_weapon,weaponinfo[player->readyweapon].downstate);
    }

#if 0 /* PROBABLY UNSAFE */
  else
    if (demo_version >= 203)  // killough 9/5/98: force switch if out of ammo
      P_SetPsprite(player,ps_weapon,weaponinfo[player->readyweapon].downstate);
#endif

  return false;
}

//
// P_SubtractAmmo
// Subtracts ammo, w/compat handling. In mbf21, use
// readyweapon's "ammopershot" field if it's explicitly
// defined in dehacked; otherwise use the amount specified
// by the codepointer instead (Doom behavior)
//
// [XA] NOTE: this function is for handling Doom's native
// attack pointers; of note, the new A_ConsumeAmmo mbf21
// codepointer does NOT call this function, since it doesn't
// have to worry about any compatibility shenanigans.
//

void P_SubtractAmmo(struct player_s *player, int vanilla_amount)
{
  int amount;
  ammotype_t ammotype = weaponinfo[player->readyweapon].ammo;

  if (player->cheats & CF_INFAMMO) { return; } // [Nugget] Yikes!

  if (mbf21 && ammotype == am_noammo)
    return; // [XA] hmm... I guess vanilla/boom will go out of bounds then?

  if (mbf21 && (weaponinfo[player->readyweapon].intflags & WIF_ENABLEAPS))
    amount = weaponinfo[player->readyweapon].ammopershot;
  else
    amount = vanilla_amount;

  player->ammo[ammotype] -= amount;

  if (mbf21 && player->ammo[ammotype] < 0)
    player->ammo[ammotype] = 0;
}

//
// P_FireWeapon.
//

int lastshottic; // killough 3/22/98

static void P_FireWeapon(player_t *player)
{
  statenum_t newstate;

  if (!P_CheckAmmo(player))
    return;

  P_SetMobjState(player->mo, S_PLAY_ATK1);
  newstate = weaponinfo[player->readyweapon].atkstate;
  P_SetPsprite(player, ps_weapon, newstate);
  if (!(weaponinfo[player->readyweapon].flags & WPF_SILENT))
  {
  P_NoiseAlert(player->mo, player->mo);
  }
  lastshottic = gametic;                       // killough 3/22/98
}

//
// P_DropWeapon
// Player died, so put the weapon away.
//

void P_DropWeapon(player_t *player)
{
  P_SetPsprite(player, ps_weapon, weaponinfo[player->readyweapon].downstate);
}

//
// P_ApplyBobbing
// Bob the weapon based on movement speed.
//

static void P_ApplyBobbing(int *sx, int *sy, fixed_t bob)
{
  int angle = (128*leveltime) & FINEMASK;
  *sx = FRACUNIT + FixedMul(bob, finecosine[angle]);
  angle &= FINEANGLES/2-1;
  *sy = WEAPONTOP + FixedMul(bob, finesine[angle]);
}

// [Nugget] Moved here
#define WEAPON_CENTERED 1
#define WEAPON_BOBBING 2
#define WEAPON_HORIZONTAL 3 // [Nugget]

// [Nugget] Bob weapon based on selected style /------------------------------

int bobbing_style;

static void P_NuggetBobbing(player_t* player)
{
  pspdef_t *psp = player->psprites;

  if ((player->attackdown && STRICTMODE(center_weapon) != WEAPON_BOBBING) // [FG] not attacking means idle
      || !psp->state || psp->state->misc1 || player->switching)
  {
    return;
  }

  // Extended weapon bobbing percentage setting
  const fixed_t bob = player->bob * weapon_bobbing_pct / 100;

  const int angle = (128*leveltime) & FINEMASK;

  // `sx` - Default, differs in a few styles
  psp->sx2 = ((1 - STRICTMODE(sx_fix)) * FRACUNIT) + FixedMul(bob, finecosine[angle]);

  // `sy` - Used for all styles, their specific values are added to this one right after
  psp->sy2 = WEAPONTOP + abs(psp->dy); // Squat weapon down on impact

  // Bobbing Styles, ported from Zandronum
  switch (STRICTMODE(bobbing_style))
  {
    case BOBSTYLE_VANILLA:
      psp->sy2 += FixedMul(bob, finesine[angle & (FINEANGLES/2 - 1)]);
      break;

    case BOBSTYLE_INVVANILLA:
      psp->sy2 += bob - FixedMul(bob, finesine[angle & (FINEANGLES/2 - 1)]);
      break;

    case BOBSTYLE_ALPHA:
      psp->sx2 = FixedMul(bob, finesine[angle]);
      psp->sy2 += FixedMul(bob, finesine[angle & (FINEANGLES/2 - 1)]);
      break;

    case BOBSTYLE_INVALPHA:
      psp->sx2 = FixedMul(bob, finesine[angle]);
      psp->sy2 += bob - FixedMul(bob, finesine[angle & (FINEANGLES/2 - 1)]);
      break;

    case BOBSTYLE_SMOOTH:
      psp->sy2 += (bob - FixedMul(bob, finecosine[angle*2 & (FINEANGLES - 1)])) / 2;
      break;

    case BOBSTYLE_INVSMOOTH:
      psp->sy2 += (bob + FixedMul(bob, finecosine[angle*2 & (FINEANGLES - 1)])) / 2;
      break;

    case BOBSTYLE_QUAKE:
      psp->sx2 = 0;
      psp->sy2 += FixedMul(bob, finesine[angle & (FINEANGLES/2 - 1)]);
      break;
  }
}

// [Nugget] -----------------------------------------------------------------/

//
// A_WeaponReady
// The player can fire the weapon
// or change to another weapon at this time.
// Follows after getting weapon up,
// or after previous attack/fire sequence.
//

void A_WeaponReady(player_t *player, pspdef_t *psp)
{
  // get out of attack state
  if (player->mo->state == &states[S_PLAY_ATK1]
      || player->mo->state == &states[S_PLAY_ATK2] )
    P_SetMobjState(player->mo, S_PLAY);

  if (player->readyweapon == wp_chainsaw && psp->state == &states[S_SAW])
    S_StartSoundPitch(player->mo, sfx_sawidl, PITCH_HALF);

  // check for change
  //  if player is dead, put the weapon away

  if (player->pendingweapon != wp_nochange || !player->health)
    {
      // [Nugget] Weapon-switch interruption
      if (CASUALPLAY(weapswitch_interruption)
          && player->pendingweapon == player->readyweapon && player->health)
      {
        player->pendingweapon = wp_nochange;
      }
      else {
        // change weapon (pending weapon should already be validated)
        statenum_t newstate = weaponinfo[player->readyweapon].downstate;
        P_SetPsprite(player, ps_weapon, newstate);
        return;
      }
    }
  else
    player->switching = weapswitch_none;

  // check for fire
  //  the missile launcher and bfg do not auto fire

  if (player->cmd.buttons & BT_ATTACK)
    {
      if (!player->attackdown || !(weaponinfo[player->readyweapon].flags & WPF_NOAUTOFIRE))
        {
          player->attackdown = true;
          P_FireWeapon(player);
          return;
        }
    }
  else
    player->attackdown = false;

  P_ApplyBobbing(&psp->sx, &psp->sy, player->bob);
  
  // [Nugget] Calculate `sx2` and `sy2` alongside `sx` and `sy`
  if (!always_bob) { P_NuggetBobbing(player); }
}

//
// A_ReFire
// The player can re-fire the weapon
// without lowering it entirely.
//

void A_ReFire(player_t *player, pspdef_t *psp)
{
  // check for fire
  //  (if a weaponchange is pending, let it go through instead)

  if ( (player->cmd.buttons & BT_ATTACK)
       && (player->pendingweapon == wp_nochange
           // [Nugget] Weapon-switch interruption
           || (CASUALPLAY(weapswitch_interruption)
               && player->pendingweapon == player->readyweapon))
       && player->health)
    {
      player->refire++;
      P_FireWeapon(player);
    }
  else
    {
      player->refire = 0;
      P_CheckAmmo(player);
    }
}

boolean boom_weapon_state_injection;

void A_CheckReload(player_t *player, pspdef_t *psp)
{
  if (!P_CheckAmmo(player) && mbf21)
  {
    // cph 2002/08/08 - In old Doom, P_CheckAmmo would start the weapon lowering
    // immediately. This was lost in Boom when the weapon switching logic was
    // rewritten. But we must tell Doom that we don't need to complete the
    // reload frames for the weapon here. G_BuildTiccmd will set ->pendingweapon
    // for us later on.
    boom_weapon_state_injection = true;
    P_SetPsprite(player, ps_weapon, weaponinfo[player->readyweapon].downstate);
  }
}

//
// A_Lower
// Lowers current weapon,
//  and changes weapon at bottom.
//

void A_Lower(player_t *player, pspdef_t *psp)
{
  // [Nugget] Weapon-switch interruption
  if (CASUALPLAY(weapswitch_interruption)
      && player->pendingweapon == player->readyweapon && player->health)
  {
    switch_interrupted = true;
    P_BringUpWeapon(player);
    return;
  }

  // [Nugget] Double speed with Fast Weapons
  const int speed = (player->cheats & CF_FASTWEAPS) ? LOWERSPEED*2 : LOWERSPEED;

  psp->sy += speed;
  psp->sy2 += speed; // [Nugget]

  // Is already down.
  if (psp->sy < WEAPONBOTTOM)
    return;

  // Player is dead.
  if (player->playerstate == PST_DEAD)
    {
      psp->sy = psp->sy2 = WEAPONBOTTOM; // [Nugget]
      return;      // don't bring weapon back up
    }

  // The old weapon has been lowered off the screen,
  // so change the weapon and start raising it

  if (!player->health)
    {      // Player is dead, so keep the weapon off screen.
      P_SetPsprite(player,  ps_weapon, S_NULL);
      return;
    }

  if (player->pendingweapon < NUMWEAPONS || !mbf21)
  {
    player->lastweapon = player->readyweapon;
    player->readyweapon = player->pendingweapon;
  }

  P_BringUpWeapon(player);
}

//
// A_Raise
//

void A_Raise(player_t *player, pspdef_t *psp)
{
  // [Nugget] Weapon-switch interruption
  if (CASUALPLAY(weapswitch_interruption) && player->pendingweapon != wp_nochange)
  {
    switch_interrupted = true;
    P_SetPsprite(player, ps_weapon, weaponinfo[player->readyweapon].downstate);
    return;
  }

  statenum_t newstate;
  // [Nugget] Double speed with Fast Weapons
  const int speed = (player->cheats & CF_FASTWEAPS) ? RAISESPEED*2 : RAISESPEED;

  psp->sy -= speed;
  psp->sy2 -= speed; // [Nugget]

  if (psp->sy > WEAPONTOP)
    return;

  psp->sy = psp->sy2 = WEAPONTOP; // [Nugget]

  // The weapon has been raised all the way,
  //  so change to the ready state.

  newstate = weaponinfo[player->readyweapon].readystate;

  P_SetPsprite(player, ps_weapon, newstate);
}

// Weapons now recoil, amount depending on the weapon.              // phares
//                                                                  //   |
// The P_SetPsprite call in each of the weapon firing routines      //   V
// was moved here so the recoil could be synched with the
// muzzle flash, rather than the pressing of the trigger.
// The BFG delay caused this to be necessary.

static void A_FireSomething(player_t* player,int adder)
{
  P_SetPsprite(player, ps_flash,
               weaponinfo[player->readyweapon].flashstate+adder);

  // killough 3/27/98: prevent recoil in no-clipping mode
  if (!(player->mo->flags & MF_NOCLIP))
    if (weapon_recoil && (demo_version >= DV_MBF || !compatibility))
      P_Thrust(player, ANG180 + player->mo->angle,
               2048*recoil_values[player->readyweapon].thrust);          // phares
}

//
// A_GunFlash
//

void A_GunFlash(player_t *player, pspdef_t *psp)
{
  P_SetMobjState(player->mo, S_PLAY_ATK2);

  A_FireSomething(player,0);                                      // phares
}

//
// WEAPON ATTACKS
//

static angle_t saved_angle;

static void SavePlayerAngle(player_t *player)
{
  saved_angle = player->mo->angle;
}

static void AddToTicAngle(player_t *player)
{
  player->ticangle += player->mo->angle - saved_angle;
}

//
// A_Punch
//

// [Nugget]
#define ANG20 (ANG60/3)
#define ANG2 (ANG20/10)

void A_Punch(player_t *player, pspdef_t *psp)
{
  angle_t angle;
  int t, slope, damage = (P_Random(pr_punch)%10+1)<<1;
  int range;

  // [Nugget] MDK Fist, basically a sniper of absurdly high damage
  if (player->cheats & CF_SAITAMA)
  {
    int i = 10;
    boolean once = true;

    // Alt Fire, basically an overpowered BFG
    if (M_InputGameActive(input_strafe)) { i = once = 0; }

    do {
      // killough 8/2/98: make autoaiming prefer enemies
      const int mask = (demo_version < DV_MBF) ? 0 : MF_FRIEND;
      angle = player->mo->angle + ANG20 - (ANG2 * i);

      if (vertical_aiming == VERTAIM_DIRECT)
      {
        slope = player->slope;
      }
      else {
        slope = P_AimLineAttack(player->mo, angle, 16*64*FRACUNIT * NOTCASUALPLAY(comp_longautoaim+1), mask);

        if (!linetarget && vertical_aiming == VERTAIM_DIRECTAUTO)
        { slope = player->slope; }
      }

      if (player->cheats & CF_BOOMCAN) { boomshot = true; }

      P_LineAttack(player->mo, angle, MISSILERANGE, slope, 1000000);
    } while (++i < 21 && !once);

    return;
  }

  if (player->powers[pw_strength])
    damage *= 10;

  angle = player->mo->angle;

  // killough 5/5/98: remove dependence on order of evaluation:
  t = P_Random(pr_punchangle);
  angle += (t - P_Random(pr_punchangle))<<18;

  range = (mbf21 ? player->mo->info->meleerange : MELEERANGE);

  // [Nugget] Direct Vertical Aiming for melee
  if (vertical_aiming == VERTAIM_DIRECT)
  {
    slope = player->slope;
    P_AimLineAttack(player->mo, player->mo->angle, range, 0);
  }
  else {
    // killough 8/2/98: make autoaiming prefer enemies
    if (demo_version < DV_MBF ||
        (slope = P_AimLineAttack(player->mo, angle, range, MF_FRIEND),
         !linetarget))
    {
      slope = P_AimLineAttack(player->mo, angle, range, 0);

      if (!linetarget && vertical_aiming == VERTAIM_DIRECTAUTO)
      { slope = player->slope; }
    }
  }

  P_LineAttack(player->mo, angle, range, slope, damage);

  if (!linetarget)
    return;

  S_StartSoundEx(player->mo, sfx_punch);

  if (CASUALPLAY(comp_nomeleesnap)) { return; } // [Nugget]

  // turn to face target

  SavePlayerAngle(player);
  player->mo->angle = R_PointToAngle2(player->mo->x, player->mo->y,
                                      linetarget->x, linetarget->y);
  AddToTicAngle(player);
}

//
// A_Saw
//

void A_Saw(player_t *player, pspdef_t *psp)
{
  int slope, damage = 2*(P_Random(pr_saw)%10+1);
  int range;
  angle_t angle = player->mo->angle;

  // killough 5/5/98: remove dependence on order of evaluation:
  int t = P_Random(pr_saw);

  angle += (t - P_Random(pr_saw))<<18;

  // Use meleerange + 1 so that the puff doesn't skip the flash
  range = (mbf21 ? player->mo->info->meleerange : MELEERANGE) + 1;

  // [Nugget] Direct Vertical Aiming for melee
  if (vertical_aiming == VERTAIM_DIRECT)
  {
    slope = player->slope;
    P_AimLineAttack(player->mo, player->mo->angle, range, 0);
  }
  else {
    // killough 8/2/98: make autoaiming prefer enemies
    if (demo_version < DV_MBF ||
        (slope = P_AimLineAttack(player->mo, angle, range, MF_FRIEND),
         !linetarget))
    {
      slope = P_AimLineAttack(player->mo, angle, range, 0);

      if (!linetarget && vertical_aiming == VERTAIM_DIRECTAUTO)
      { slope = player->slope; }
    }
  }

  P_LineAttack(player->mo, angle, range, slope, damage);

  A_Recoil(player);

  if (!linetarget)
    {
      S_StartSoundPitchEx(player->mo, sfx_sawful, PITCH_HALF);
      return;
    }

  S_StartSoundPitchEx(player->mo, sfx_sawhit, PITCH_HALF);

  if (CASUALPLAY(comp_nomeleesnap)) { return; } // [Nugget]

  // turn to face target

  angle = R_PointToAngle2(player->mo->x, player->mo->y,
                          linetarget->x, linetarget->y);

  SavePlayerAngle(player);
  if (angle - player->mo->angle > ANG180)
    if ((signed int) (angle - player->mo->angle) < -ANG90/20)
      player->mo->angle = angle + ANG90/21;
    else
      player->mo->angle -= ANG90/20;
  else
    if (angle - player->mo->angle > ANG90/20)
      player->mo->angle = angle - ANG90/21;
    else
      player->mo->angle += ANG90/20;
  AddToTicAngle(player);

  player->mo->flags |= MF_JUSTATTACKED;
}

//
// A_FireMissile
//

void A_FireMissile(player_t *player, pspdef_t *psp)
{
  P_SubtractAmmo(player, 1);
  P_SpawnPlayerMissile(player->mo, MT_ROCKET);
}

//
// A_FireBFG
//

void A_FireBFG(player_t *player, pspdef_t *psp)
{
  P_SubtractAmmo(player, BFGCELLS);
  P_SpawnPlayerMissile(player->mo, MT_BFG);
}

//
// A_FireOldBFG
//
// This function emulates Doom's Pre-Beta BFG
// By Lee Killough 6/6/98, 7/11/98, 7/19/98, 8/20/98
//
// This code may not be used in other mods without appropriate credit given.
// Code leeches will be telefragged.

void A_FireOldBFG(player_t *player, pspdef_t *psp)
{
  int type = MT_PLASMA1;

  if (weapon_recoil && !(player->mo->flags & MF_NOCLIP))
    P_Thrust(player, ANG180 + player->mo->angle,
             512*recoil_values[wp_plasma].thrust);

  if (weapon_recoilpitch && (leveltime & 2))
  {
    player->recoilpitch = recoil_values[wp_plasma].pitch * ANG1;
  }

  P_SubtractAmmo(player, 1);

  player->extralight = 2;

  do
    {
      mobj_t *th, *mo = player->mo;
      angle_t an = mo->angle;
      angle_t an1 = ((P_Random(pr_bfg)&127) - 64) * (ANG90/768) + an;
      angle_t an2 = ((P_Random(pr_bfg)&127) - 64) * (ANG90/640) + ANG90;
      fixed_t slope = 0; // [Nugget]

      // [Nugget] Vertical aiming;
      // Taken outside of code block after this one
      // to allow direct vertical aiming in Beta
      if (vertical_aiming == VERTAIM_DIRECT)
      {
        slope = player->slope;
      }
      else
      if (autoaim || !beta_emulation)
	{
	  // killough 8/2/98: make autoaiming prefer enemies
	  int mask = MF_FRIEND;
	  // [Nugget] Moved vertical aiming code above
	  do
	    {
	      // [Nugget] Double Autoaim range
	      slope = P_AimLineAttack(mo, an, 16*64*FRACUNIT * NOTCASUALPLAY(comp_longautoaim+1), mask);
	      if (!linetarget)
		slope = P_AimLineAttack(mo, an += 1<<26, 16*64*FRACUNIT * NOTCASUALPLAY(comp_longautoaim+1), mask);
	      if (!linetarget)
		slope = P_AimLineAttack(mo, an -= 2<<26, 16*64*FRACUNIT * NOTCASUALPLAY(comp_longautoaim+1), mask);
	      if (!linetarget)
		an = mo->angle,
		// [Nugget] Vertical aiming
		slope = (vertical_aiming == VERTAIM_DIRECTAUTO) ? player->slope : 0;
	    }
	  while (mask && (mask=0, !linetarget));     // killough 8/2/98
	}

      an1 += an - mo->angle;
      // sf: despite killough's infinite wisdom.. even
      // he is prone to mistakes. seems negative numbers
      // won't survive a bitshift!
      if (slope < 0)
        an2 -= tantoangle[-slope >> DBITS];
      else
        an2 += tantoangle[slope >> DBITS];

      th = P_SpawnMobj(mo->x, mo->y,
		       mo->z + 62*FRACUNIT - player->psprites[ps_weapon].sy,
		       type);
      P_SetTarget(&th->target, mo);
      th->angle = an1;
      th->momx = finecosine[an1>>ANGLETOFINESHIFT] * 25;
      th->momy = finesine[an1>>ANGLETOFINESHIFT] * 25;
      th->momz = finetangent[an2>>ANGLETOFINESHIFT] * 25;
      // [FG] suppress interpolation of player missiles for the first tic
      th->interp = -1;
      P_CheckMissileSpawn(th);
    }
  while ((type != MT_PLASMA2) && (type = MT_PLASMA2)); //killough: obfuscated!
}

//
// A_FirePlasma
//

void A_FirePlasma(player_t *player, pspdef_t *psp)
{
  P_SubtractAmmo(player, 1);
  A_FireSomething(player, P_Random(pr_plasma) & 1);

  // killough 7/11/98: emulate Doom's beta version, which alternated fireballs
  P_SpawnPlayerMissile(player->mo, beta_emulation ?
                       player->refire&1 ? MT_PLASMA2 : MT_PLASMA1 : MT_PLASMA);
}

//
// P_BulletSlope
// Sets a slope so a near miss is at aproximately
// the height of the intended target
//

fixed_t bulletslope;

static void P_BulletSlope(mobj_t *mo)
{
  angle_t an = mo->angle;    // see which target is to be aimed at

  // killough 8/2/98: make autoaiming prefer enemies
  int mask = demo_version < DV_MBF ? 0 : MF_FRIEND;

  if (vertical_aiming == VERTAIM_DIRECT) // [Nugget] Vertical aiming
  {
    bulletslope = mo->player->slope;
  }
  else
  do
    {
      // [Nugget] Double Autoaim range
      bulletslope = P_AimLineAttack(mo, an, 16*64*FRACUNIT * NOTCASUALPLAY(comp_longautoaim+1), mask);
      if (!linetarget)
        bulletslope = P_AimLineAttack(mo, an += 1<<26, 16*64*FRACUNIT * NOTCASUALPLAY(comp_longautoaim+1), mask);
      if (!linetarget)
        bulletslope = P_AimLineAttack(mo, an -= 2<<26, 16*64*FRACUNIT * NOTCASUALPLAY(comp_longautoaim+1), mask);
      // [Nugget] Vertical aiming
      if (!linetarget && vertical_aiming == VERTAIM_DIRECTAUTO)
        bulletslope = mo->player->slope;
    }
  while (mask && (mask=0, !linetarget));  // killough 8/2/98
}

//
// P_GunShot
//

void P_GunShot(mobj_t *mo, boolean accurate)
{
  int damage = 5*(P_Random(pr_gunshot)%3+1);
  angle_t angle = mo->angle;

  if (!accurate)
    {  // killough 5/5/98: remove dependence on order of evaluation:
      int t = P_Random(pr_misfire);
      angle += (t - P_Random(pr_misfire))<<18;
    }

  // [Nugget] Explosive hitscan cheat
  if (mo->player && mo->player->cheats & CF_BOOMCAN) { boomshot = true; }

  P_LineAttack(mo, angle, MISSILERANGE, bulletslope, damage);
}

//
// A_FirePistol
//

void A_FirePistol(player_t *player, pspdef_t *psp)
{
  S_StartSoundPistol(player->mo, sfx_pistol);

  P_SetMobjState(player->mo, S_PLAY_ATK2);
  P_SubtractAmmo(player, 1);

  A_FireSomething(player,0);                                      // phares
  A_Recoil(player);
  P_BulletSlope(player->mo);
  P_GunShot(player->mo, !player->refire);
}

//
// A_FireShotgun
//

void A_FireShotgun(player_t *player, pspdef_t *psp)
{
  int i;

  S_StartSoundShotgun(player->mo, sfx_shotgn);
  P_SetMobjState(player->mo, S_PLAY_ATK2);

  P_SubtractAmmo(player, 1);

  A_FireSomething(player,0);                                      // phares
  A_Recoil(player);

  P_BulletSlope(player->mo);

  for (i=0; i<7; i++)
    P_GunShot(player->mo, false);
}

//
// A_FireShotgun2
//

void A_FireShotgun2(player_t *player, pspdef_t *psp)
{
  int i;

  S_StartSoundSSG(player->mo, sfx_dshtgn);
  P_SetMobjState(player->mo, S_PLAY_ATK2);
  P_SubtractAmmo(player, 2);

  A_FireSomething(player,0);                                      // phares
  A_Recoil(player);

  P_BulletSlope(player->mo);

  for (i=0; i<20; i++)
    {
      int damage = 5*(P_Random(pr_shotgun)%3+1);
      angle_t angle = player->mo->angle;
      // killough 5/5/98: remove dependence on order of evaluation:
      int t = P_Random(pr_shotgun);
      angle += (t - P_Random(pr_shotgun))<<19;
      t = P_Random(pr_shotgun);

      // [Nugget] Explosive hitscan cheat
      if (player->cheats & CF_BOOMCAN) { boomshot = true; }

      P_LineAttack(player->mo, angle, MISSILERANGE, bulletslope +
                   ((t - P_Random(pr_shotgun))<<5), damage);
    }
}

boolean comp_cgundblsnd; // [Nugget]

//
// A_FireCGun
//

void A_FireCGun(player_t *player, pspdef_t *psp)
{
  // [Nugget] /===============================================================
  
  // Fix "Chaingun sound without ammo" bug
  if (!(strictmode || comp_cgundblsnd))
    if (!player->ammo[weaponinfo[player->readyweapon].ammo])
      return;

  // Use DSCHGUN if available ------------------------------------------------
  
  static int sound = -1;
  
  if (sound == -1)
  { sound = ((W_CheckNumForName("dschgun") > -1) ? sfx_chgun : sfx_pistol); }
  
  // [Nugget] ===============================================================/
  
  S_StartSoundCGun(player->mo, !strictmode ? sound : sfx_pistol); // [Nugget]

  if (!player->ammo[weaponinfo[player->readyweapon].ammo])
    return;

  // killough 8/2/98: workaround for beta chaingun sprites missing at bottom
  // The beta did not have fullscreen, and its chaingun sprites were chopped
  // off at the bottom for some strange reason. So we lower the sprite if
  // fullscreen is in use.
  {
    if (beta_emulation && screenblocks>=11)
      player->psprites[ps_weapon].sy = FRACUNIT*48;
  }

  P_SetMobjState(player->mo, S_PLAY_ATK2);
  P_SubtractAmmo(player, 1);

  A_FireSomething(player,psp->state - &states[S_CHAIN1]);           // phares
  A_Recoil(player);

  P_BulletSlope(player->mo);

  P_GunShot(player->mo, !player->refire);
}

void A_Light0(player_t *player, pspdef_t *psp)
{
  player->extralight = 0;
}

void A_Light1 (player_t *player, pspdef_t *psp)
{
  player->extralight = 1;
}

void A_Light2 (player_t *player, pspdef_t *psp)
{
  player->extralight = 2;
}

//
// A_BFGSpray
// Spawn a BFG explosion on every monster in view
//

void A_BFGSpray(mobj_t *mo)
{
  int i;
  int shake = 0; // [Nugget] Explosion shake effect

  for (i=0 ; i<40 ; i++)  // offset angles from its attack angle
    {
      int j, damage;
      angle_t an = mo->angle - ANG90/2 + ANG90/40*i;

      // mo->target is the originator (player) of the missile

      // killough 8/2/98: make autoaiming prefer enemies
      if (demo_version < DV_MBF || 
          (P_AimLineAttack(mo->target, an, 16*64*FRACUNIT, MF_FRIEND), 
           !linetarget))
        P_AimLineAttack(mo->target, an, 16*64*FRACUNIT, 0);

      // [Nugget] Hitscan trails
      if (P_GetShowHitscanTrails() == 2)
      {
        // To-do: these trails don't account for collision with walls

        const int range = 16*64;

        const fixed_t srcx = mo->target->x,
                      srcy = mo->target->y,
                      srcz = mo->target->z + (mo->target->height>>1) + 8*FRACUNIT;

        fixed_t destx, desty, slope, distance;

        if (linetarget)
        {
          destx = linetarget->x;
          desty = linetarget->y;
          distance = P_AproxDistance(destx - srcx, desty - srcy);
          slope = FixedDiv(linetarget->z + (linetarget->height >> 2) - srcz, distance);
        }
        else {
          destx = srcx + range * finecosine[an >> ANGLETOFINESHIFT];
          desty = srcy + range * finesine[an >> ANGLETOFINESHIFT];
          distance = P_AproxDistance(destx - srcx, desty - srcy);
          slope = 0;
        }

        P_SpawnHitscanTrail(srcx, srcy, srcz, an, slope, range * FRACUNIT, distance);
      }

      if (!linetarget)
        continue;

      P_SpawnMobj(linetarget->x, linetarget->y,
                  linetarget->z + (linetarget->height>>2), MT_EXTRABFG);

      for (damage=j=0; j<15; j++)
        damage += (P_Random(pr_bfg)&7) + 1;

      P_DamageMobj(linetarget, mo->target, mo->target, damage);

      shake++; // [Nugget] Explosion shake effect
    }

  // [Nugget] Explosion shake effect
  if (mo->target->player == &players[displayplayer])
  { R_ExplosionShake(mo->target->x, mo->target->y, 2*shake, 16*64); }
}

//
// A_BFGsound
//

void A_BFGsound(player_t *player, pspdef_t *psp)
{
  S_StartSoundBFG(player->mo, sfx_bfg);
}

//
// P_SetupPsprites
// Called at start of level for each player.
//

void P_SetupPsprites(player_t *player)
{
  int i;

  // remove all psprites
  for (i=0; i<NUMPSPRITES; i++)
    player->psprites[i].state = NULL;

  // spawn the gun
  player->pendingweapon = player->readyweapon;
  P_BringUpWeapon(player);
}

// [Nugget - ceski] Weapon Inertia /------------------------------------------

boolean weapon_inertia;
int weapon_inertia_scale_pct;

#define EASE_SCALE(x, y) (FRACUNIT - (FixedDiv(FixedMul(FixedDiv((x) << FRACBITS, (y) << FRACBITS), (fixed_t) weapon_inertia_scale), FRACUNIT)))
#define EASE_OUT(x, y) ((x) - FixedMul((x), FixedMul((y), (y))))
#define MAX_DELTA (SCREENWIDTH << FRACBITS)

fixed_t weapon_inertia_scale;

static void WeaponInertiaHorizontal(player_t* player, pspdef_t *psp)
{
  angle_t angle;
  boolean clockwise;

  if (player->mo->oldangle > player->mo->angle)
  {
    angle = player->mo->oldangle - player->mo->angle;
    clockwise = (angle <= ANG180);
    if (!clockwise)
      angle = ANGLE_MAX - angle;
  }
  else
  {
    angle = player->mo->angle - player->mo->oldangle;
    clockwise = (angle > ANG180);
    if (clockwise)
      angle = ANGLE_MAX - angle;
  }

  angle >>= ANGLETOFINESHIFT;
  if (angle > 0)
  {
    const fixed_t scale = EASE_SCALE(angle, FINEANGLES);
    fixed_t delta = EASE_OUT(MAX_DELTA, scale);
    delta = MIN(delta, MAX_DELTA);
    psp->wix += clockwise ? -delta : delta;
  }

  if (psp->wix != 0)
  {
    psp->wix >>= 1;
    if (abs(psp->wix) < FRACUNIT)
      psp->wix = 0;
  }
}

static void WeaponInertiaVertical(player_t* player, pspdef_t *psp)
{
  const fixed_t pitch = (player->pitch - player->oldpitch) >> ANGLETOFINESHIFT;

  if (pitch != 0)
  {
    const fixed_t scale = EASE_SCALE(abs(pitch), FINEANGLES);
    fixed_t delta = EASE_OUT(MAX_DELTA, scale);
    delta = MIN(delta, MAX_DELTA);
    psp->wiy += pitch < 0 ? -delta : delta;
  }

  if (psp->wiy != 0)
  {
    psp->wiy >>= 1;
    if (abs(psp->wiy) < FRACUNIT)
      psp->wiy = 0;
  }

  if (psp->wiy != 0)
  {
    const fixed_t min = WEAPONTOP - (screenblocks < 11 ? (WEAPONTOP >> 1) : 0);
    if (psp->sy2 + psp->wiy < min)
      psp->wiy = min - psp->sy2;
  }
}

static void P_NuggetWeaponInertia(player_t *player, pspdef_t *psp)
{
  if (STRICTMODE(weapon_inertia))
  {
    if (!psp->state || player->switching || player->attackdown || psp->state->misc1)
    {
      psp->wix = 0;
      psp->wiy = 0;
      return;
    }

    WeaponInertiaHorizontal(player, psp);

    if (mouselook || padlook || player->pitch || psp->wiy)
      WeaponInertiaVertical(player, psp);
  }
}

void P_NuggetResetWeaponInertia(void)
{
  if (gamestate == GS_LEVEL && playeringame[displayplayer])
  {
    pspdef_t *psp = &players[displayplayer].psprites[ps_weapon];
    psp->wix = psp->wiy = 0;
  }
}

// [Nugget] -----------------------------------------------------------------/

//
// P_MovePsprites
// Called every tic by player thinking routine.
//

// [Nugget] Moved weapon-alignment macros above

void P_MovePsprites(player_t *player)
{
  pspdef_t *psp = player->psprites;
  const int center_weapon_strict = STRICTMODE(center_weapon);
  int i;

  // a null state means not active
  // drop tic count and possibly change state
  // a -1 tic count never changes

  for (i=0; i<NUMPSPRITES; i++, psp++)
    if (psp->state && psp->tics != -1 && !--psp->tics)
      P_SetPsprite(player, i, psp->state->nextstate);

  player->psprites[ps_flash].sx = player->psprites[ps_weapon].sx;
  player->psprites[ps_flash].sy = player->psprites[ps_weapon].sy;

  // [FG] centered weapon sprite
  psp = &player->psprites[ps_weapon];

  // [Nugget] Calculate sx2 and sy2 separately from sx and sy
  if (always_bob || (player->attackdown && center_weapon_strict == WEAPON_BOBBING))
  { P_NuggetBobbing(player); }

  if (psp->state)
  {
    if (!weapon_bobbing_pct)
    {
      static fixed_t last_sy = WEAPONTOP;

      psp->sx2 = (1 - STRICTMODE(sx_fix))*FRACUNIT; // [Nugget] Correct first person sprite centering

      if (!psp->state->misc1 && !player->switching)
      {
        last_sy = psp->sy2;
        psp->sy2 = WEAPONTOP + abs(psp->dy); // [Nugget] Squat weapon down on impact
      }
      else if (player->switching == weapswitch_lowering)
      {
        // We want to move smoothly from where we were
        psp->sy2 -= (last_sy - WEAPONTOP);
      }
    }
    else if (center_weapon_strict) // [Nugget] Removed `uncapped` check
    {
      // [FG] don't center during lowering and raising states
      if (psp->state->misc1 || player->switching)
      {
      }
      // [FG] center the weapon sprite horizontally and push up vertically
      else if (player->attackdown && center_weapon_strict & WEAPON_CENTERED) // [Nugget] Horizontal weapon centering
      {
        psp->sx2 = (1 - STRICTMODE(sx_fix)) * FRACUNIT; // [Nugget] Correct first person sprite centering
        if (center_weapon_strict == WEAPON_CENTERED) // [Nugget] Horizontal weapon centering
          psp->sy2 = WEAPONTOP;
      }
    }
  }

  // [Nugget]: [crispy] squat down weapon sprite a bit after hitting the ground
  if (psp->dy)
  {
    if (psp->dy > 24*FRACUNIT) { psp->dy = 24*FRACUNIT; }
    else                       { psp->dy -= FRACUNIT; }

    if (psp->dy < 0) { psp->dy = 0; }
  }

  P_NuggetWeaponInertia(player, psp);

  player->psprites[ps_flash].sx2 = player->psprites[ps_weapon].sx2;
  player->psprites[ps_flash].sy2 = player->psprites[ps_weapon].sy2;
  player->psprites[ps_flash].dy  = player->psprites[ps_weapon].dy;
  player->psprites[ps_flash].wix = player->psprites[ps_weapon].wix;
  player->psprites[ps_flash].wiy = player->psprites[ps_weapon].wiy;
}

//
// [XA] New mbf21 codepointers
//

//
// A_WeaponProjectile
// A parameterized player weapon projectile attack. Does not consume ammo.
//   args[0]: Type of actor to spawn
//   args[1]: Angle (degrees, in fixed point), relative to calling player's angle
//   args[2]: Pitch (degrees, in fixed point), relative to calling player's pitch; approximated
//   args[3]: X/Y spawn offset, relative to calling player's angle
//   args[4]: Z spawn offset, relative to player's default projectile fire height
//
void A_WeaponProjectile(player_t *player, pspdef_t *psp)
{
  int type, angle, pitch, spawnofs_xy, spawnofs_z;
  mobj_t *mo;
  int an;

  if (!mbf21 || !psp->state || !psp->state->args[0])
    return;

  type        = psp->state->args[0] - 1;
  angle       = psp->state->args[1];
  pitch       = psp->state->args[2];
  spawnofs_xy = psp->state->args[3];
  spawnofs_z  = psp->state->args[4];

  mo = P_SpawnPlayerMissile(player->mo, type);
  if (!mo)
    return;

  // adjust angle
  mo->angle += (angle_t)(((int64_t)angle << 16) / 360);
  an = mo->angle >> ANGLETOFINESHIFT;
  mo->momx = FixedMul(mo->info->speed, finecosine[an]);
  mo->momy = FixedMul(mo->info->speed, finesine[an]);

  // adjust pitch (approximated, using Doom's ye olde
  // finetangent table; same method as autoaim)
  mo->momz += FixedMul(mo->info->speed, DegToSlope(pitch));

  // adjust position
  an = (player->mo->angle - ANG90) >> ANGLETOFINESHIFT;
  mo->x += FixedMul(spawnofs_xy, finecosine[an]);
  mo->y += FixedMul(spawnofs_xy, finesine[an]);
  mo->z += spawnofs_z;

  // set tracer to the player's autoaim target,
  // so player seeker missiles prioritizing the
  // baddie the player is actually aiming at. ;)
  mo->tracer = linetarget;
}

//
// A_WeaponBulletAttack
// A parameterized player weapon bullet attack. Does not consume ammo.
//   args[0]: Horizontal spread (degrees, in fixed point)
//   args[1]: Vertical spread (degrees, in fixed point)
//   args[2]: Number of bullets to fire; if not set, defaults to 1
//   args[3]: Base damage of attack (e.g. for 5d3, customize the 5); if not set, defaults to 5
//   args[4]: Attack damage modulus (e.g. for 5d3, customize the 3); if not set, defaults to 3
//
void A_WeaponBulletAttack(player_t *player, pspdef_t *psp)
{
  int hspread, vspread, numbullets, damagebase, damagemod;
  int i, damage, angle, slope;

  if (!mbf21 || !psp->state)
    return;

  hspread    = psp->state->args[0];
  vspread    = psp->state->args[1];
  numbullets = psp->state->args[2];
  damagebase = psp->state->args[3];
  damagemod  = psp->state->args[4];

  P_BulletSlope(player->mo);

  for (i = 0; i < numbullets; i++)
  {
    damage = (P_Random(pr_mbf21) % damagemod + 1) * damagebase;
    angle = (int)player->mo->angle + P_RandomHitscanAngle(pr_mbf21, hspread);
    slope = bulletslope + P_RandomHitscanSlope(pr_mbf21, vspread);

    // [Nugget] Explosive hitscan cheat
    if (player->cheats & CF_BOOMCAN) { boomshot = true; }

    P_LineAttack(player->mo, angle, MISSILERANGE, slope, damage);
  }

  A_Recoil(player);
}

//
// A_WeaponMeleeAttack
// A parameterized player weapon melee attack.
//   args[0]: Base damage of attack (e.g. for 2d10, customize the 2); if not set, defaults to 2
//   args[1]: Attack damage modulus (e.g. for 2d10, customize the 10); if not set, defaults to 10
//   args[2]: Berserk damage multiplier (fixed point); if not set, defaults to 1.0 (no change).
//   args[3]: Sound to play if attack hits
//   args[4]: Range (fixed point); if not set, defaults to player mobj's melee range
//
void A_WeaponMeleeAttack(player_t *player, pspdef_t *psp)
{
  int damagebase, damagemod, zerkfactor, hitsound, range;
  angle_t angle;
  int t, slope, damage;

  if (!mbf21 || !psp->state)
    return;

  damagebase = psp->state->args[0];
  damagemod  = psp->state->args[1];
  zerkfactor = psp->state->args[2];
  hitsound   = psp->state->args[3];
  range      = psp->state->args[4];

  if (range == 0)
    range = player->mo->info->meleerange;

  damage = (P_Random(pr_mbf21) % damagemod + 1) * damagebase;
  if (player->powers[pw_strength])
    damage = (damage * zerkfactor) >> FRACBITS;

  // slight randomization; weird vanillaism here. :P
  angle = player->mo->angle;

  t = P_Random(pr_mbf21);
  angle += (t - P_Random(pr_mbf21))<<18;

  // [Nugget] Direct Vertical Aiming for melee
  if (vertical_aiming == VERTAIM_DIRECT)
  {
    slope = player->slope;
    P_AimLineAttack(player->mo, player->mo->angle, range, 0);
  }
  else {
    // make autoaim prefer enemies
    slope = P_AimLineAttack(player->mo, angle, range, MF_FRIEND);
    if (!linetarget)
    {
      slope = P_AimLineAttack(player->mo, angle, range, 0);

      if (!linetarget && vertical_aiming == VERTAIM_DIRECTAUTO)
      { slope = player->slope; }
    }
  }

  // attack, dammit!
  P_LineAttack(player->mo, angle, range, slope, damage);

  A_Recoil(player);

  // missed? ah, welp.
  if (!linetarget)
    return;

  // un-missed!
  S_StartSoundEx(player->mo, hitsound);

  if (CASUALPLAY(comp_nomeleesnap)) { return; } // [Nugget]

  // turn to face target

  SavePlayerAngle(player);
  player->mo->angle = R_PointToAngle2(player->mo->x, player->mo->y, linetarget->x, linetarget->y);
  AddToTicAngle(player);
}

//
// A_WeaponSound
// Plays a sound. Usable from weapons, unlike A_PlaySound
//   args[0]: ID of sound to play
//   args[1]: If 1, play sound at full volume (may be useful in DM?)
//
void A_WeaponSound(player_t *player, pspdef_t *psp)
{
  if (!mbf21 || !psp->state)
    return;

  S_StartSoundOrigin(player->mo, (psp->state->args[1] ? NULL : player->mo),
                     psp->state->args[0]);
}

//
// A_WeaponAlert
// Alerts monsters to the player's presence. Handy when combined with WPF_SILENT.
//
void A_WeaponAlert(player_t *player, pspdef_t *psp)
{
  if (!mbf21)
    return;

  P_NoiseAlert(player->mo, player->mo);
}

//
// A_WeaponJump
// Jumps to the specified state, with variable random chance.
// Basically the same as A_RandomJump, but for weapons.
//   args[0]: State number
//   args[1]: Chance, out of 255, to make the jump
//
void A_WeaponJump(player_t *player, pspdef_t *psp)
{
  if (!mbf21 || !psp->state)
    return;

  if (P_Random(pr_mbf21) < psp->state->args[1])
    P_SetPspritePtr(player, psp, psp->state->args[0]);
}

//
// A_ConsumeAmmo
// Subtracts ammo from the player's "inventory". 'Nuff said.
//   args[0]: Amount of ammo to consume. If zero, use the weapon's ammo-per-shot amount.
//
void A_ConsumeAmmo(player_t *player, pspdef_t *psp)
{
  int amount;
  ammotype_t type;

  if (!mbf21)
    return;

  if (player->cheats & CF_INFAMMO) { return; } // [Nugget] Yikes!

  // don't do dumb things, kids
  type = weaponinfo[player->readyweapon].ammo;
  if (!psp->state || type == am_noammo)
        return;

  // use the weapon's ammo-per-shot amount if zero.
  // to subtract zero ammo, don't call this function. ;)
  if (psp->state->args[0] != 0)
    amount = psp->state->args[0];
  else
    amount = weaponinfo[player->readyweapon].ammopershot;

  // subtract ammo, but don't let it get below zero
  if (player->ammo[type] >= amount)
    player->ammo[type] -= amount;
  else
    player->ammo[type] = 0;
}

//
// A_CheckAmmo
// Jumps to a state if the player's ammo is lower than the specified amount.
//   args[0]: State to jump to
//   args[1]: Minimum required ammo to NOT jump. If zero, use the weapon's ammo-per-shot amount.
//
void A_CheckAmmo(player_t *player, pspdef_t *psp)
{
  int amount;
  ammotype_t type;

  if (!mbf21)
    return;

  type = weaponinfo[player->readyweapon].ammo;
  if (!psp->state || type == am_noammo)
    return;

  if (psp->state->args[1] != 0)
    amount = psp->state->args[1];
  else
    amount = weaponinfo[player->readyweapon].ammopershot;

  if (player->ammo[type] < amount)
    P_SetPspritePtr(player, psp, psp->state->args[0]);
}

//
// A_RefireTo
// Jumps to a state if the player is holding down the fire button
//   args[0]: State to jump to
//   args[1]: If nonzero, skip the ammo check
//
void A_RefireTo(player_t *player, pspdef_t *psp)
{
  if (!mbf21 || !psp->state)
    return;

  if ((psp->state->args[1] || P_CheckAmmo(player))
  &&  (player->cmd.buttons & BT_ATTACK)
  &&  (player->pendingweapon == wp_nochange && player->health))
    P_SetPspritePtr(player, psp, psp->state->args[0]);
}

//
// A_GunFlashTo
// Sets the weapon flash layer to the specified state.
//   args[0]: State number
//   args[1]: If nonzero, don't change the player actor state
//
void A_GunFlashTo(player_t *player, pspdef_t *psp)
{
  if (!mbf21 || !psp->state)
    return;

  if(!psp->state->args[1])
    P_SetMobjState(player->mo, S_PLAY_ATK2);

  P_SetPsprite(player, ps_flash, psp->state->args[0]);
}

//----------------------------------------------------------------------------
//
// $Log: p_pspr.c,v $
// Revision 1.13  1998/05/07  00:53:36  killough
// Remove dependence on order of evaluation
//
// Revision 1.12  1998/05/05  16:29:17  phares
// Removed RECOIL and OPT_BOBBING defines
//
// Revision 1.11  1998/05/03  22:35:21  killough
// Fix weapons switch bug again, beautification, headers
//
// Revision 1.10  1998/04/29  10:01:55  killough
// Fix buggy weapons switch code
//
// Revision 1.9  1998/03/28  18:01:38  killough
// Prevent weapon recoil in no-clipping mode
//
// Revision 1.8  1998/03/23  03:28:29  killough
// Move weapons changes to G_BuildTiccmd()
//
// Revision 1.7  1998/03/10  07:14:47  jim
// Initial DEH support added, minus text
//
// Revision 1.6  1998/02/24  08:46:27  phares
// Pushers, recoil, new friction, and over/under work
//
// Revision 1.5  1998/02/17  05:59:41  killough
// Use new RNG calling sequence
//
// Revision 1.4  1998/02/15  02:47:54  phares
// User-defined keys
//
// Revision 1.3  1998/02/09  03:06:15  killough
// Add player weapon preference options
//
// Revision 1.2  1998/01/26  19:24:18  phares
// First rev with no ^Ms
//
// Revision 1.1.1.1  1998/01/19  14:03:00  rand
// Lee's Jan 19 sources
//
//----------------------------------------------------------------------------
