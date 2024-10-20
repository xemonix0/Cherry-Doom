//
// Copyright(C) 2021-2023 by Ryan Krafnick
// Copyright(C) 2024 by Xemonix
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//

#ifndef __WAD_STATS__
#define __WAD_STATS__

#include "doomtype.h"

typedef struct
{
    char *wad_name;
    int   wad_index;

    char  lump[9];
    int   episode, map;

    int   total_exits, total_kills;
    int   max_kills, max_items, max_secrets;
    int   best_skill;
    int   best_kills, best_items, best_secrets;
    int   best_time, best_max_time, best_sk5_time;
} map_stats_t;

typedef struct
{
    map_stats_t* maps;
    int kill_check;
    boolean one_wad;
} wad_stats_t;

extern char *wad_stats_fail;
extern wad_stats_t wad_stats;

// CVARs

extern boolean lt_enable_tracking;
extern boolean lt_track_continuous;
extern boolean lt_reset_on_higher_skill;
extern int lt_stats_format;

void WadStats_Init(void);
void WadStats_Save(void);
void WadStats_Cleanup(void);

void WadStats_EraseMapStats(int i);
void WadStats_EraseWadStats(void);

void WadStats_WatchMap(void);
void WadStats_UnwatchMap(void);
void WadStats_WatchKill(void);
void WadStats_WatchExitMap(void);

void WadStats_BindLevelTableVariables(void);

#endif
