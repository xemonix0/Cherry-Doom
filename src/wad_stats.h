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

void WS_Init(void);
void WS_Save(void);
void WS_Cleanup(void);

void WS_EraseMapStats(int i);
void WS_EraseWadStats(void);

void WS_WatchMap(void);
void WS_UnwatchMap(void);
void WS_WatchKill(void);
void WS_WatchExitMap(void);

#endif
