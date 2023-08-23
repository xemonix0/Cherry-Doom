//
// Copyright(C) 2023 by Ryan Krafnick
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

#ifndef __WS_WAD_STATS__
#define __WS_WAD_STATS__

typedef struct
{
  char lump[9];
  int episode;
  int map;
  int best_skill;
  int best_time;
  int best_max_time;
  int best_sk5_time;
  int total_exits;
  int total_kills;
  int best_kills;
  int best_items;
  int best_secrets;
  int max_kills;
  int max_items;
  int max_secrets;
  int session_attempts;
  int best_attempts;
  int total_attempts;
} map_stats_t;

typedef struct
{
  int total_kills;
  map_stats_t* maps;
  int maps_size;
  int map_count;
} wad_stats_t;

extern wad_stats_t wad_stats;

void WS_WadStatsEnterMap(void);
void WS_WadStatsLoadGame(void);
void WS_WadStatsExitMap(int missed_monsters);
void WS_WadStatsKill(void);
void WS_SaveWadStats(void);
void WS_InitWadStats(void);

#endif
