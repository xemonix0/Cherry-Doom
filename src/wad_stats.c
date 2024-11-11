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

#include "wad_stats.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "d_main.h"
#include "d_think.h"
#include "doomdef.h"
#include "doomstat.h"
#include "doomtype.h"
#include "hu_stuff.h"
#include "i_printf.h"
#include "m_array.h"
#include "m_config.h"
#include "m_io.h"
#include "m_misc.h"
#include "mn_menu.h"
#include "p_mobj.h"
#include "p_tick.h"
#include "w_wad.h"

#define CURRENT_VERSION_STRING "1"

static const char *DATA_ROOT = "cherry_doom_data";
static const char *STATS_FILENAME = "stats.txt";

static char *stats_path;
static int wad_index;
static map_stats_t *current_map_stats;

char *wad_stats_fail = NULL;
wad_stats_t wad_stats = {0};

// CVARs

boolean lt_enable_tracking;
boolean lt_track_continuous;
boolean lt_reset_on_higher_skill;
statsformat_t lt_stats_format;

#define CAN_WATCH_MAP (lt_enable_tracking && !notracking && wad_stats.maps)
#define TRACKING      (CAN_WATCH_MAP && current_map_stats)

// File I/O Operations
//====================

static char *InitBaseDataDir(void)
{
    const char *parent_directory = M_getenv("DOOMDATADIR");
    if (!parent_directory)
    {
        parent_directory = D_DoomPrefDir();
    }

    char *base_data_dir =
        M_StringJoin(parent_directory, DIR_SEPARATOR_S, DATA_ROOT);
    M_MakeDirectory(base_data_dir);

    return base_data_dir;
}

static char **DataDirNames(void)
{
    char **data_dir_names = NULL;

    for (int i = 0; i < array_size(wadfiles); ++i)
    {
        if (!wadfiles[i].contains_maps)
        {
            continue;
        }

        const char *filename = M_BaseName(wadfiles[i].name);
        size_t length = strlen(filename) - 3;
        char *name = malloc(length);
        M_StringCopy(name, filename, length);
        M_StringToLower(name);

        array_push(data_dir_names, name);
    }

    return data_dir_names;
}

static char *InitDataDir(void)
{
    char *data_dir = InitBaseDataDir();
    char **data_dir_names = DataDirNames();
    for (int i = 0; i < array_size(data_dir_names); ++i)
    {
        data_dir = M_StringJoin(data_dir, DIR_SEPARATOR_S, data_dir_names[i]);
        M_MakeDirectory(data_dir);
    }

    // Cleanup
    for (int i = 0; i < array_size(data_dir_names); i++)
    {
        if (data_dir_names[i])
        {
            free(data_dir_names[i]);
        }
    }
    array_free(data_dir_names);

    return data_dir;
}

static const char *StatsPath(void)
{
    if (!stats_path)
    {
        char *data_dir = InitDataDir();

        stats_path = M_StringJoin(data_dir, DIR_SEPARATOR_S, STATS_FILENAME);

        I_Printf(VB_INFO, " stats file: %s", stats_path);

        free(data_dir);
    }

    return stats_path;
}

// Map Stats Operations
//=====================

static map_stats_t *MapStats(int episode, int map)
{
    for (int i = 0; i < array_size(wad_stats.maps); ++i)
    {
        map_stats_t *ms = &wad_stats.maps[i];

        if (ms->wad_index)
        {
            break;
        }

        if (ms->episode == episode && ms->map == map)
        {
            return ms;
        }
    }

    return NULL;
}

static boolean MapStatsExist(const char *lump)
{
    for (int i = 0; i < array_size(wad_stats.maps); ++i)
    {
        if (!strncasecmp(wad_stats.maps[i].lump, lump, 8))
        {
            return true;
        }
    }

    return false;
}

static int CompareMapStats(const void *a, const void *b)
{
    const map_stats_t *ms1 = (const map_stats_t *)a;
    const map_stats_t *ms2 = (const map_stats_t *)b;

    // 1. Sort by WAD index
    if (ms1->wad_index != ms2->wad_index)
    {
        return (ms1->wad_index > ms2->wad_index)
               - (ms1->wad_index < ms2->wad_index);
    }
    // 2. Sort unavailable maps lexicographically
    if (ms1->episode == -1 && ms2->episode == -1)
    {
        return strncasecmp(ms1->lump, ms2->lump, 8);
    }
    // 3. Unavailable maps go last
    if (ms1->episode == -1)
    {
        return 1;
    }
    if (ms2->episode == -1)
    {
        return -1;
    }
    // 4. Sort by episode number
    if (ms1->episode != ms2->episode)
    {
        return (ms1->episode > ms2->episode) - (ms1->episode < ms2->episode);
    }
    // 5. Sort by map number
    if (ms1->map != ms2->map)
    {
        return (ms1->map > ms2->map) - (ms1->map < ms2->map);
    }
    // 6. Sort lexicographically (shouldn't be possible to get here)
    return strncasecmp(ms1->lump, ms2->lump, 8);
}

static map_stats_t CreateMapStats(const char *map_name, char *wad_name)
{
    map_stats_t ms = {0};
    strcpy(ms.lump, map_name);

    if (gamemode == commercial && sscanf(map_name, "MAP%d", &ms.map) == 1)
    {
        ms.episode = 1;
    }
    else if (gamemode != commercial
             && sscanf(map_name, "E%dM%d", &ms.episode, &ms.map) == 2)
    {
    }
    else
    {
        ms.episode = -1;
        ms.map = -1;
    }

    ms.wad_name = wad_name;
    ms.wad_index = wad_index;

    ms.max_kills = ms.max_items = ms.max_secrets = -1;
    ms.best_time = ms.best_max_time = ms.best_sk5_time = -1;

    return ms;
}

static void CreateStats(boolean finish)
{
    char *last_wad_name = NULL;
    wad_index = finish ? wad_index : -1;

    for (int i = numlumps - 1; i > 0; --i)
    {
        const char *lump_name = lumpinfo[i].name;
        if (lumpinfo[i].source == source_other
            || !MN_StartsWithMapIdentifier(lumpinfo[i].name)
            || MapStatsExist(lump_name))
        {
            continue;
        }

        char *wad_name = M_StringDuplicate(W_WadNameForLump(i));
        if (!last_wad_name || strcmp(last_wad_name, wad_name))
        {
            last_wad_name = M_StringDuplicate(wad_name);
            ++wad_index;
        }

        map_stats_t ms = CreateMapStats(lump_name, wad_name);
        array_push(wad_stats.maps, ms);
    }

    qsort(wad_stats.maps, array_size(wad_stats.maps), sizeof(*wad_stats.maps),
          CompareMapStats);
    wad_stats.one_wad = !wad_index;

    // Cleanup
    free(last_wad_name);
}

void WadStats_EraseMapStats(int i)
{
    if (!CAN_WATCH_MAP)
    {
        return;
    }

    map_stats_t *ms = &wad_stats.maps[i];

    if (!ms)
    {
        return;
    }

    ms->total_exits = ms->total_kills = 0;
    ms->best_skill = 0;
    ms->best_kills = ms->best_items = ms->best_secrets = 0;
    ms->max_kills = ms->max_items = ms->max_secrets = -1;
    ms->best_time = ms->best_max_time = ms->best_sk5_time = -1;
}

void WadStats_EraseWadStats(void)
{
    if (!CAN_WATCH_MAP)
    {
        return;
    }

    for (int i = 0; i < array_size(wad_stats.maps); ++i)
    {
        WadStats_EraseMapStats(i);
    }
    wad_stats.kill_check = 0;
}

// Stats Saving & Loading
//=======================

// Loading
// -------

enum
{
    LOAD_SUCCESS,
    LOAD_ERROR_NOTFOUND,
    LOAD_ERROR_VERSION,
    LOAD_ERROR_GENERIC,
};

static int HandleLoadErrors(int err)
{
    const char *path = StatsPath();

    switch (err)
    {
        case LOAD_ERROR_VERSION:
            I_Printf(VB_WARNING,
                     "Encountered unsupported WAD stats version: %s", path);
            wad_stats_fail = "Unsupported WAD stats version!";
            break;
        case LOAD_ERROR_GENERIC:
            I_Printf(VB_WARNING, "Encountered invalid WAD stats: %s", path);
            wad_stats_fail = "Invalid WAD stats found!";
            break;
        case LOAD_ERROR_NOTFOUND:
            return LOAD_ERROR_NOTFOUND;
        default:
            break;
    }

    return LOAD_SUCCESS;
}

static inline boolean CheckStatsVersion(const char *line, const char *str)
{
    return !strncmp(line, str, strlen(str));
}

static int ParseMapLine(const char *line, map_stats_t *ms)
{
    return sscanf(line, "%8s %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                  ms->lump, &ms->episode, &ms->map, &ms->best_skill,
                  &ms->best_time, &ms->best_max_time, &ms->best_sk5_time,
                  &ms->total_exits, &ms->total_kills, &ms->best_kills,
                  &ms->best_items, &ms->best_secrets, &ms->max_kills,
                  &ms->max_items, &ms->max_secrets);
}

static int LoadStatsFromLines(char **lines)
{
    if (!CheckStatsVersion(lines[0], "1") && !CheckStatsVersion(lines[0], "2"))
    {
        return LOAD_ERROR_VERSION;
    }

    if (sscanf(lines[1], "%d", &wad_stats.kill_check) != 1)
    {
        return LOAD_ERROR_GENERIC;
    }

    for (int i = 2; lines[i] && *lines[i]; ++i)
    {
        map_stats_t ms = {0};
        if (ParseMapLine(lines[i], &ms) != 15)
        {
            return LOAD_ERROR_GENERIC;
        }

        ms.wad_name =
            M_StringDuplicate(W_WadNameForLump(W_GetNumForName(ms.lump)));
        ms.wad_index = wad_index;
        array_push(wad_stats.maps, ms);
    }

    return LOAD_SUCCESS;
}

static int LoadStats(void)
{
    char *buffer = NULL;
    char **lines = NULL;
    int ret = LOAD_SUCCESS;

    while (ret == LOAD_SUCCESS)
    {
        const char *path = StatsPath();
        if (M_ReadFileToString(path, &buffer) == -1)
        {
            return LOAD_ERROR_NOTFOUND;
        }

        lines = M_StringSplit(buffer, "\n\r");
        if (!lines || !lines[0])
        {
            ret = LOAD_ERROR_GENERIC;
            break;
        }

        ret = LoadStatsFromLines(lines);
        if (ret != LOAD_SUCCESS)
        {
            break;
        }

        // Go through missing maps from all the WADs and add dummy items for
        // them, so that the player can still warp to them from the level table
        CreateStats(true);

        break;
    }

    // Cleanup
    free(lines);
    free(buffer);

    return ret;
}

// Saving
// ------

static void WriteMapLine(FILE *file, const map_stats_t *ms)
{
    fprintf(file, "%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", ms->lump,
            ms->episode, ms->map, ms->best_skill, ms->best_time,
            ms->best_max_time, ms->best_sk5_time, ms->total_exits,
            ms->total_kills, ms->best_kills, ms->best_items, ms->best_secrets,
            ms->max_kills, ms->max_items, ms->max_secrets);
}

void WadStats_Save(void)
{
    if (!CAN_WATCH_MAP)
    {
        return;
    }

    const char *path = StatsPath();
    FILE *file = M_fopen(path, "wb");
    if (!file)
    {
        I_Printf(VB_WARNING,
                 "WadStats_Save: Failed to save WAD stats file \"%s\".",
                 path);
        return;
    }

    fprintf(file, CURRENT_VERSION_STRING "\n");
    fprintf(file, "%d\n", wad_stats.kill_check);

    for (int i = 0; i < array_size(wad_stats.maps); ++i)
    {
        map_stats_t *ms = &wad_stats.maps[i];
        if (ms->wad_index)
        {
            continue;
        }

        WriteMapLine(file, ms);
    }

    fclose(file);
}

// Gameplay Tracking
//==================

void WadStats_WatchMap(void)
{
    if (!CAN_WATCH_MAP)
    {
        return;
    }

    current_map_stats = MapStats(gameepisode, gamemap);
}

void WadStats_UnwatchMap(void)
{
    if (!TRACKING)
    {
        return;
    }

    current_map_stats = NULL;
}

void WadStats_WatchKill(void)
{
    if (!TRACKING)
    {
        return;
    }

    ++wad_stats.kill_check;
    ++current_map_stats->total_kills;
}

static inline int CustomToVanillaSkill(void)
{
    return ((thingspawns == THINGSPAWNS_EASY)
                ? (doubleammo && halfdamage)
                      ? sk_baby
                      : sk_easy
            : (thingspawns == THINGSPAWNS_HARD)
                ? (doubleammo && fastmonsters && respawnmonsters && aggressive)
                      ? sk_nightmare
                      : sk_hard
                : sk_medium)
           + 1;
}

static int MissedMonsters(void)
{
    int missed_monsters = 0;
    for (thinker_t *th = thinkercap.next; th != &thinkercap; th = th->next)
    {
        if (th->function.p1 != (actionf_p1)P_MobjThinker)
        {
            continue;
        }

        mobj_t *mobj = (mobj_t *)th;

        if (((mobj->flags ^ MF_COUNTKILL) & (MF_FRIEND | MF_COUNTKILL))
            || (mobj->intflags & MIF_SPAWNED_BY_ICON) || mobj->health <= 0)
        {
            continue;
        }

        ++missed_monsters;
    }

    return missed_monsters;
}

void WadStats_WatchExitMap(void)
{
    if (!TRACKING)
    {
        return;
    }

    ++current_map_stats->total_exits;

    if (realnomonsters)
    {
        return;
    }

    const int skill =
        (gameskill == sk_custom) ? CustomToVanillaSkill() : gameskill + 1;
    const int missed_monsters = MissedMonsters();

    if (skill < current_map_stats->best_skill && skill != 4)
    {
        return;
    }

    if (skill > current_map_stats->best_skill)
    {
        if (current_map_stats->best_skill < 4)
        {
            current_map_stats->best_time = -1;
            current_map_stats->best_max_time = -1;
        }

        if (lt_reset_on_higher_skill && skill != 5)
        {
            current_map_stats->best_kills = current_map_stats->best_items =
                current_map_stats->best_secrets = -1;
        }

        current_map_stats->best_skill = skill;
    }

    if (lt_track_continuous || levels_completed == 1)
    {
        if (current_map_stats->best_time == -1
            || current_map_stats->best_time > leveltime)
        {
            current_map_stats->best_time = leveltime;
        }

        if (skill == 5
            && (current_map_stats->best_sk5_time == -1
                || current_map_stats->best_sk5_time > leveltime))
        {
            current_map_stats->best_sk5_time = leveltime;
        }

        if (!respawnmonsters)
        {
            current_map_stats->best_kills = MAX(current_map_stats->best_kills,
                                                totalkills - missed_monsters);

            if (!missed_monsters
                && players[consoleplayer].secretcount == totalsecret
                && (current_map_stats->best_max_time == -1
                    || current_map_stats->best_max_time > leveltime))
            {
                current_map_stats->best_max_time = leveltime;
            }
        }
    }

    current_map_stats->max_kills = totalkills;
    current_map_stats->max_items = totalitems;
    current_map_stats->max_secrets = totalsecret;

    current_map_stats->best_items =
        MAX(current_map_stats->best_items, players[consoleplayer].itemcount);
    current_map_stats->best_secrets = MAX(current_map_stats->best_secrets,
                                          players[consoleplayer].secretcount);
}

// Initialization & Cleanup
//=========================

void WadStats_Init(void)
{
    if (notracking || !lt_enable_tracking // the level table still needs to work
        || HandleLoadErrors(LoadStats()) == LOAD_ERROR_NOTFOUND)
    {
        CreateStats(false);
    }
}

static void FreeWadStats(void)
{
    for (int i = 0; i < array_size(wad_stats.maps); ++i)
    {
        free(wad_stats.maps[i].wad_name);
    }

    array_free(wad_stats.maps);
}

void WadStats_Cleanup(void)
{
    FreeWadStats();

    free(stats_path);
}

void WadStats_BindLevelTableVariables(void)
{
    M_BindBool("lt_enable_tracking", &lt_enable_tracking, NULL, true, ss_none,
               wad_no, "Enable WAD stats tracking");
    M_BindBool("lt_track_continuous", &lt_track_continuous, NULL, true, ss_none,
               wad_no, "Track kills and times for maps that aren't completed from a pistol start");
    M_BindBool("lt_reset_on_higher_skill", &lt_reset_on_higher_skill, NULL, true, ss_none,
               wad_no, "Reset all stats for the current level upon beating the level on a new best skill");
    M_BindNum("lt_stats_format", &lt_stats_format, NULL,
              STATSFORMAT_RATIO, STATSFORMAT_MATCHHUD, NUM_STATSFORMATS - 1,
              ss_none, wad_no,
              "Format of level stats in level table (0 = Match HUD; 1 = Ratio; 2 = Boolean; 3 = Percentage; 4 = Remaining; 5 = Count)");
}
