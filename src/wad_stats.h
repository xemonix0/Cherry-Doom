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

    int   episode;
    int   map;

    int   max_kills;
    int   max_items;
    int   max_secrets;

    int   total_exits;
    int   total_kills;

    int   best_skill;
    int   best_kills;
    int   best_items;
    int   best_secrets;
    int   best_time;
    int   best_max_time;
    int   best_sk5_time;

    int   best_attempts;
    int   session_attempts;
    int   total_attempts;
} map_stats_t;

typedef struct
{
    map_stats_t* maps;
    boolean one_wad;
} wad_stats_t;

extern wad_stats_t wad_stats;
extern char *wad_stats_fail;

void WS_InitWadStats(void);
void WS_SaveWadStats(void);

void WS_WatchEnterMap(void);
void WS_UnwatchMap(void);
void WS_WatchLoadGame(void);
void WS_WatchKill(void);
void WS_WatchExitMap(void);

#endif
