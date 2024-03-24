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

#include <ctype.h>
#include <stdio.h>

#include "d_iwad.h"
#include "d_main.h"
#include "doomstat.h"
#include "i_system.h"
#include "m_array.h"
#include "m_io.h"
#include "m_misc.h"
#include "w_wad.h"
#include "z_zone.h"

#include "ws_wadstats.h"

#define DATA_DIR_LIMIT 9
static const char* cherry_data_root = "cherry_doom_data";
static char* data_dir_strings[DATA_DIR_LIMIT];

static char* base_data_dir;
static char* wad_data_dir;

static const char* filename = "stats.txt";
static const int current_version = 2;
static map_stats_t* current_map_stats;

wad_stats_t wad_stats;

static void InitWadDataDir(void) {
  int i;
  const int iwad_index = 0;
  int pwad_index = 1;
  char* str;

  for (i = 0; i < array_size(wadfiles); ++i)
  {
    const char* start;
    char* result;
    int length;

    start = M_BaseName(wadfiles[i].name);

    length = strlen(start) - 4;

    if (length > 0 && !strcasecmp(start + length, ".wad"))
    {
      int dir_index;

      if (wadfiles[i].src == source_iwad)
        dir_index = iwad_index;
      else if (wadfiles[i].src == source_pwad)
        dir_index = pwad_index;
      else
        dir_index = -1;

      if (dir_index >= 0 && dir_index < DATA_DIR_LIMIT)
      {
        data_dir_strings[dir_index] = Z_Malloc(length + 1, PU_STATIC, NULL);
        strncpy(data_dir_strings[dir_index], start, length);
        data_dir_strings[dir_index][length] = '\0';

        for (result = data_dir_strings[dir_index]; *result; ++result)
          *result = tolower(*result);

        if (dir_index == pwad_index)
          pwad_index++;
      }
    }
  }

  if (!base_data_dir)
  {
    const char* parent_directory = M_getenv("DOOMDATADIR");

    if (!parent_directory)
      parent_directory = D_DoomPrefDir();

    M_StringPrintF(&base_data_dir, "%s/%s", parent_directory, cherry_data_root);
    NormalizeSlashes(base_data_dir);

    M_MakeDirectory(base_data_dir);
  }

  str = Z_Malloc(strlen(base_data_dir) + 1, PU_STATIC, NULL);
  strncpy(str, base_data_dir, strlen(base_data_dir) + 1);

  for (i = 0; i < DATA_DIR_LIMIT; ++i)
    if (data_dir_strings[i])
    {
      M_StringCatF(&str, "/%s", data_dir_strings[i]);
      NormalizeSlashes(str);

      M_MakeDirectory(str);
    }

  wad_data_dir = str;
}

char* DataDir(void) {
  if (!wad_data_dir)
    InitWadDataDir();

  return wad_data_dir;
}

static void WS_EnsureMapCount(int count) {
  wad_stats.map_count = count;

  if (wad_stats.maps_size < wad_stats.map_count)
  {
    int old_size;

    old_size = wad_stats.maps_size;
    while (wad_stats.maps_size < wad_stats.map_count)
      wad_stats.maps_size = wad_stats.maps_size ? wad_stats.maps_size * 2 : 32;
    wad_stats.maps = Z_Realloc(wad_stats.maps, sizeof(*wad_stats.maps) * wad_stats.maps_size, PU_STATIC, NULL);
    memset(wad_stats.maps + old_size,
           0, (wad_stats.maps_size - old_size) * sizeof(*wad_stats.maps));
  }
}

static const char* WS_WadStatsPath(void) {
  static char* path;

  if (!path)
  {
    const char* data_dir = DataDir();
    M_StringPrintF(&path, "%s/%s", data_dir, filename);
    NormalizeSlashes(path);
  }

  return path;
}

static boolean WS_MapStatsExist(const char* lump) {
  int i;

  for (i = 0; i < wad_stats.maps_size && wad_stats.maps[i].lump[0]; ++i)
    if (!strncasecmp(wad_stats.maps[i].lump, lump, 8))
      return true;

  return false;
}

static int C_DECL dicmp_map_stats(const void* a, const void* b) {
  const map_stats_t* m1 = (const map_stats_t *)a;
  const map_stats_t* m2 = (const map_stats_t *)b;

  if (m1->episode == -1 && m2->episode == -1)
    return m1->lump[0] - m2->lump[0];

  if (m1->episode == -1)
    return 1;

  if (m2->episode == -1)
    return -1;

  return (m1->episode == m2->episode) ?
    (m1->map == m2->map) ?
    m1->lump[0] - m2->lump[0] : m1->map - m2->map : m1->episode - m2->episode;
}

static void WS_CreateWadStats(void) {
  int i;
  const char* map_name;
  int map_count = 0;
  boolean any_pwad_map = false;

  for (i = numlumps - 1; i > 0; --i)
  {
    int episode, map;
    map_stats_t* ms;

    if (any_pwad_map && lumpinfo[i].source == source_iwad)
      break;

    if (lumpinfo[i].source != source_iwad &&
        lumpinfo[i].source != source_pwad)
      continue;

    if (strncasecmp(lumpinfo[i].name, "THINGS", 8) &&
        strncasecmp(lumpinfo[i].name, "TEXTMAP", 8))
      continue;

    map_name = lumpinfo[i - 1].name;
    if (WS_MapStatsExist(map_name))
      continue;

    if (lumpinfo[i - 1].source == source_pwad)
      any_pwad_map = true;

    map_count += 1;
    WS_EnsureMapCount(map_count);
    ms = &wad_stats.maps[map_count - 1];
    memset(ms, 0, sizeof(*wad_stats.maps));

    strcpy(ms->lump, map_name);

    if (sscanf(map_name, "MAP%d", &map) == 1)
    {
      ms->episode = 1;
      ms->map = map;
    }
    else if (sscanf(map_name, "E%dM%d", &episode, &map) == 2)
    {
      ms->episode = episode;
      ms->map = map;
    }
    else
    {
      ms->episode = -1;
      ms->map = -1;
    }

    ms->best_time = -1;
    ms->best_max_time = -1;
    ms->best_sk5_time = -1;
    ms->max_kills = -1;
    ms->max_items = -1;
    ms->max_secrets = -1;
  }

  qsort(wad_stats.maps, wad_stats.map_count, sizeof(*wad_stats.maps), dicmp_map_stats);
}

static void WS_LoadWadStats(void) {
  const char* path;
  char* buffer;
  char** lines;
  int version;
  int map_count = 0;
  int i;

  path = WS_WadStatsPath();

  if (M_ReadFileToString(path, &buffer) != -1)
  {
    const char* line_ending = "\n\r";
    int substring_count = 2;
    char* p = buffer;

    while (*p)
      if (*p++ == *line_ending)
        ++substring_count;

    lines = Z_Calloc(substring_count, sizeof(*lines), PU_STATIC, NULL);

    if (lines)
    {
      char* token;
      int i = 0;

      token = strtok(buffer, line_ending);
      while (token)
      {
        lines[i++] = token;
        token = strtok(NULL, line_ending);
      }
    }

    if (lines)
    {
      if (!lines[0] || !lines[1])
        I_Error("Encountered invalid wad stats: %s", path);

      if (sscanf(lines[0], "%d", &version) != 1)
        I_Error("Encountered invalid wad stats: %s", path);

      if (version > current_version)
        I_Error("Encountered unsupported wad stats version: %s", path);

      if (sscanf(lines[1], "%d", &wad_stats.total_kills) != 1)
        I_Error("Encountered invalid wad stats: %s", path);

      for (i = 2; lines[i] && *lines[i]; ++i)
      {
        map_stats_t ms;

        int values = sscanf(
          lines[i], "%8s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
          ms.lump, &ms.episode, &ms.map,
          &ms.best_skill, &ms.best_time, &ms.best_max_time, &ms.best_sk5_time,
          &ms.total_exits, &ms.total_kills,
          &ms.best_kills, &ms.best_items, &ms.best_secrets,
          &ms.max_kills, &ms.max_items, &ms.max_secrets,
          &ms.best_attempts, &ms.total_attempts
        );

        if (values == 15 || values == 17)
        {
          if (values == 15)
          {
            ms.best_attempts = 0;
            ms.total_attempts = 0;
          }

          ms.session_attempts = 0;
          map_count += 1;
          WS_EnsureMapCount(map_count);
          wad_stats.maps[map_count - 1] = ms;
        }
      }

      Z_Free(lines);
    }

    Z_Free(buffer);
  }
}

void WS_SaveWadStats(void) {
  const char* path;
  FILE* file;
  int i;

  if (!wad_stats.map_count)
    return;

  path = WS_WadStatsPath();

  file = M_fopen(path, "wb");
  if (!file)
  {
    printf("WS_SaveWadStats: Failed to save wad stats file \"%s\".", path);
    return;
  }

  fprintf(file, "%d\n", current_version);
  fprintf(file, "%d\n", wad_stats.total_kills);

  for (i = 0; i < wad_stats.map_count; ++i)
  {
    map_stats_t* ms;

    ms = &wad_stats.maps[i];
    fprintf(file, "%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
            ms->lump, ms->episode, ms->map,
            ms->best_skill, ms->best_time, ms->best_max_time, ms->best_sk5_time,
            ms->total_exits, ms->total_kills,
            ms->best_kills, ms->best_items, ms->best_secrets,
            ms->max_kills, ms->max_items, ms->max_secrets,
            ms->best_attempts, ms->total_attempts);
  }

  fclose(file);
}

static map_stats_t* WS_MapStats(int episode, int map) {
  int i;

  for (i = 0; i < wad_stats.map_count; ++i)
    if (wad_stats.maps[i].episode == episode && wad_stats.maps[i].map == map)
      return &wad_stats.maps[i];

  return NULL;
}

void WS_WadStatsEnterMap(void) {
  current_map_stats = WS_MapStats(gameepisode, gamemap);
    
  if (current_map_stats)
  {
    if (current_map_stats->best_attempts)
    {
      bestattempts  = current_map_stats->best_attempts;
    }
    else
    {
      bestattempts  = -1;
    }
    sessionattempts = ++current_map_stats->session_attempts;
    totalattempts   = ++current_map_stats->total_attempts;
    return;
  }

  sessionattempts = -1;
  bestattempts    = -1;
  totalattempts   = -1;
}

void WS_WadStatsLoadGame(void)
{
  current_map_stats->session_attempts = sessionattempts;
  // Prevent total attempts from incrementing
  totalattempts = --current_map_stats->total_attempts;
}

void WS_WadStatsExitMap(int missed_monsters) {
  int skill;

  if (!current_map_stats || demoplayback)
    return;

  if (!nomonsters)
  {
    skill = gameskill + 1;
    if (skill > current_map_stats->best_skill)
    {
      if (current_map_stats->best_skill < 4)
      {
        current_map_stats->best_time = -1;
        current_map_stats->best_max_time = -1;
      }

      current_map_stats->best_skill = skill;
    }

    if (skill >= current_map_stats->best_skill || skill == 4)
    {
      if (levelscompleted == 1)
        if (current_map_stats->best_time == -1 || current_map_stats->best_time > leveltime)
          current_map_stats->best_time = leveltime;

      if (levelscompleted == 1 && skill == 5)
        if (current_map_stats->best_sk5_time == -1 || current_map_stats->best_sk5_time > leveltime)
          current_map_stats->best_sk5_time = leveltime;

      if (!current_map_stats->best_attempts ||
          current_map_stats->session_attempts < current_map_stats->best_attempts)
        current_map_stats->best_attempts = current_map_stats->session_attempts;
      current_map_stats->session_attempts = 0;

      current_map_stats->max_kills = totalkills;
      current_map_stats->max_items = totalitems;
      current_map_stats->max_secrets = totalsecret;

      if (!respawnmonsters)
      {
        if (totalkills - missed_monsters > current_map_stats->best_kills)
          current_map_stats->best_kills = totalkills - missed_monsters;

        if (levelscompleted == 1)
          if (missed_monsters == 0 && players[consoleplayer].secretcount == totalsecret &&
              (current_map_stats->best_max_time == -1 || current_map_stats->best_max_time > leveltime))
            current_map_stats->best_max_time = leveltime;
      }

      if (players[consoleplayer].itemcount > current_map_stats->best_items)
        current_map_stats->best_items = players[consoleplayer].itemcount;

      if (players[consoleplayer].secretcount > current_map_stats->best_secrets)
        current_map_stats->best_secrets = players[consoleplayer].secretcount;
    }
  }

  ++current_map_stats->total_exits;
}

void WS_WadStatsKill(void) {
  if (!current_map_stats || demoplayback)
    return;

  ++current_map_stats->total_kills;
  ++wad_stats.total_kills;
}

void WS_InitWadStats(void) {
  WS_LoadWadStats();

  if (!wad_stats.map_count)
    WS_CreateWadStats();
}
