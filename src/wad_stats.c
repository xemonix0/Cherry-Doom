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

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "d_iwad.h"
#include "d_main.h"
#include "doomstat.h"
#include "i_printf.h"
#include "i_system.h"
#include "m_array.h"
#include "m_io.h"
#include "m_misc.h"
#include "p_mobj.h"
#include "p_tick.h"
#include "w_wad.h"
#include "z_zone.h"

#define DATA_DIR_DEPTH_LIMIT 9

static const char *cherry_data_root = "cherry_doom_data";
static char *data_dir_names[DATA_DIR_DEPTH_LIMIT];
static const char *wad_stats_filename = "stats.txt";

static map_stats_t *current_map_stats;

static const int current_version = 2;

wad_stats_t wad_stats = {0};

static char *InitWadDataDir(void)
{
    static char *base_data_dir = NULL;
    const int iwad_index = 0;
    int pwad_index = 1;
    int current_dir_index;

    for (int i = 0; i < array_size(wadfiles); ++i)
    {
        const char *name = M_BaseName(wadfiles[i].name);

        if (!M_StringCaseEndsWith(name, ".wad"))
        {
            continue;
        }

        switch (wadfiles[i].src)
        {
            case source_iwad:
                current_dir_index = iwad_index;
                break;
            case source_pwad:
                current_dir_index = pwad_index;
                break;
            default:
                continue;
        }

        if (current_dir_index >= DATA_DIR_DEPTH_LIMIT)
        {
            continue;
        }

        const size_t length = strlen(name) - 3;
        data_dir_names[current_dir_index] = malloc(length);
        M_StringCopy(data_dir_names[current_dir_index], name, length);
        M_StringToLower(data_dir_names[current_dir_index]);

        if (current_dir_index == pwad_index)
        {
            ++pwad_index;
        }
    }

    if (!base_data_dir)
    {
        const char *parent_directory = M_getenv("DOOMDATADIR");

        if (!parent_directory)
        {
            parent_directory = D_DoomPrefDir();
        }

        M_StringPrintF(&base_data_dir, "%s/%s", parent_directory,
                       cherry_data_root);
        M_MakeDirectory(base_data_dir);
    }

    char *wad_data_dir = M_StringDuplicate(base_data_dir);

    for (int i = 0; i < DATA_DIR_DEPTH_LIMIT; ++i)
    {
        if (data_dir_names[i])
        {
            M_StringConcatF(&wad_data_dir, "/%s", data_dir_names[i]);
            M_MakeDirectory(wad_data_dir);
        }
    }

    return wad_data_dir;
}

static const char *WadDataDir(void)
{
    static char *wad_data_dir;

    if (!wad_data_dir)
    {
        wad_data_dir = InitWadDataDir();
    }

    return wad_data_dir;
}

static const char *WadStatsPath(void)
{
    static char *wad_stats_path;

    if (!wad_stats_path)
    {
        const char *wad_data_dir = WadDataDir();
        M_StringPrintF(&wad_stats_path, "%s/%s", wad_data_dir,
                       wad_stats_filename);
    }

    return wad_stats_path;
}

static boolean MapStatsExist(const char *lump)
{
    for (int i = 0; i < array_size(wad_stats.maps) && *wad_stats.maps[i].lump;
         ++i)
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

    return (ms1->episode == -1 && ms2->episode == -1)
               ? strncasecmp(ms1->lump, ms2->lump, 8)
           : ms1->episode == -1 ? 1
           : ms2->episode == -1 ? -1
           : ms1->episode == ms2->episode
               ? ms1->map == ms2->map
                     ? strncasecmp(ms1->lump, ms2->lump, 8)
                     : (ms1->map > ms2->map) - (ms1->map < ms2->map)
               : (ms1->episode > ms2->episode) - (ms1->episode < ms2->episode);
}

static void CreateWadStats(void)
{
    boolean pwad_maps = false;

    for (int i = numlumps - 1; i > 0; --i)
    {
        int episode, map;

        if (pwad_maps && lumpinfo[i].source == source_iwad)
        {
            break;
        }

        if (lumpinfo[i].source != source_iwad
            && lumpinfo[i].source != source_pwad)
        {
            continue;
        }

        if (strncasecmp(lumpinfo[i].name, "THINGS", 8) != 0)
        {
            continue;
        }

        const char *map_name = lumpinfo[i - 1].name;
        if (MapStatsExist(map_name))
        {
            continue;
        }

        if (lumpinfo[i - 1].source == source_pwad)
        {
            pwad_maps = true;
        }

        map_stats_t ms = {0};
        array_push(wad_stats.maps, ms);

        map_stats_t *p = &wad_stats.maps[array_size(wad_stats.maps) - 1];
        strcpy(p->lump, map_name);

        if (sscanf(map_name, "MAP%d", &map) == 1)
        {
            p->map = map;
            p->episode = 1;
        }
        else if (sscanf(map_name, "E%dM%d", &episode, &map) == 2)
        {
            p->episode = episode;
            p->map = map;
        }
        else
        {
            p->episode = -1;
            p->map = -1;
        }

        p->best_time = -1;
        p->best_max_time = -1;
        p->best_sk5_time = -1;
        p->max_kills = -1;
        p->max_items = -1;
        p->max_secrets = -1;
    }

    qsort(wad_stats.maps, array_size(wad_stats.maps), sizeof(*wad_stats.maps),
          CompareMapStats);
}

static boolean LoadWadStats(void)
{
    const char *path = WadStatsPath();
    char *buffer;
    if (M_ReadFileToString(path, &buffer) == -1)
    {
        return false;
    }

    boolean success = true;
    char **lines = M_StringSplit(buffer, "\n\r");
    if (!lines || !lines[0])
    {
        I_Error("Encountered invalid wad stats: %s", path);
    }

    if (strncmp(lines[0], "1", 1) && strncmp(lines[0], "2", 1))
    {
        I_Error("Encountered unsupported wad stats version: %s", path);
    }

    if (sscanf(lines[1], "%d", &wad_stats.kill_check) != 1)
    {
        I_Error("Encountered invalid wad stats: %s", path);
    }

    for (int i = 2; lines[i] && *lines[i]; ++i)
    {
        map_stats_t ms = {0};

        int values = sscanf(
            lines[i], "%8s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
            ms.lump, &ms.episode, &ms.map, &ms.best_skill, &ms.best_time,
            &ms.best_max_time, &ms.best_sk5_time, &ms.total_exits,
            &ms.total_kills, &ms.best_kills, &ms.best_items, &ms.best_secrets,
            &ms.max_kills, &ms.max_items, &ms.max_secrets, &ms.best_attempts,
            &ms.total_attempts);

        if (values != 15 && values != 17)
        {
            I_Error("Encountered invalid wad stats: %s", path);
        }

        if (values == 15)
        {
            ms.best_attempts = 0;
            ms.total_attempts = 0;
        }

        ms.session_attempts = 0;

        array_push(wad_stats.maps, ms);
    }

    free(lines);
    Z_Free(buffer);

    return true;
}

void WS_SaveWadStats(void)
{
    if (!wad_stats.maps || !array_size(wad_stats.maps))
    {
        return;
    }

    const char *path = WadStatsPath();
    FILE *file = M_fopen(path, "wb");
    if (!file)
    {
        I_Printf(VB_WARNING,
                 "WS_SaveWadStats: Failed to save wad stats file \"%s\".",
                 path);
        return;
    }

    fprintf(file, "%d\n", current_version);
    fprintf(file, "%d\n", wad_stats.kill_check);

    for (int i = 0; i < array_size(wad_stats.maps); ++i)
    {
        map_stats_t *ms = &wad_stats.maps[i];
        fprintf(file, "%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                ms->lump, ms->episode, ms->map, ms->best_skill, ms->best_time,
                ms->best_max_time, ms->best_sk5_time, ms->total_exits,
                ms->total_kills, ms->best_kills, ms->best_items,
                ms->best_secrets, ms->max_kills, ms->max_items, ms->max_secrets,
                ms->best_attempts, ms->total_attempts);
    }

    fclose(file);
}

static map_stats_t *MapStats(int episode, int map)
{
    for (int i = 0; i < array_size(wad_stats.maps); ++i)
    {
        if (wad_stats.maps[i].episode == episode
            && wad_stats.maps[i].map == map)
        {
            return &wad_stats.maps[i];
        }
    }

    return NULL;
}

void WS_WatchEnterMap(void)
{
    if (!wad_stats.maps)
    {
        return;
    }

    current_map_stats = MapStats(gameepisode, gamemap);

    if (!current_map_stats)
    {
        sessionattempts = -1;
        bestattempts = -1;
        totalattempts = -1;

        return;
    }

    if (current_map_stats->best_attempts)
    {
        bestattempts = current_map_stats->best_attempts;
    }
    else
    {
        bestattempts = -1;
    }

    sessionattempts = ++current_map_stats->session_attempts;
    totalattempts = ++current_map_stats->total_attempts;
}

void WS_WatchLoadGame(void)
{
    current_map_stats->session_attempts = sessionattempts;
    // HACK: Prevent total attempts from incrementing
    totalattempts = --current_map_stats->total_attempts;
}

void WS_WatchKill(void)
{
    if (!current_map_stats || demoplayback)
    {
        return;
    }

    ++current_map_stats->total_kills;
    ++wad_stats.kill_check;
}

void WS_WatchExitMap(void)
{
    if (!current_map_stats || demoplayback)
    {
        return;
    }

    ++current_map_stats->total_exits;

    if (nomonsters)
    {
        return;
    }

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

    int skill = gameskill + 1;

    if (skill < current_map_stats->best_skill)
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

        current_map_stats->best_skill = skill;
    }

    if (skill < current_map_stats->best_skill && skill != 4)
    {
        return;
    }

    if (levelscompleted == 1)
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

    if (!current_map_stats->best_attempts
        || current_map_stats->session_attempts
               < current_map_stats->best_attempts)
    {
        current_map_stats->best_attempts = current_map_stats->session_attempts;
    }

    current_map_stats->session_attempts = 0;

    current_map_stats->max_kills = totalkills;
    current_map_stats->max_items = totalitems;
    current_map_stats->max_secrets = totalsecret;

    current_map_stats->best_items =
        MAX(current_map_stats->best_items, players[consoleplayer].itemcount);
    current_map_stats->best_secrets = MAX(current_map_stats->best_secrets,
                                          players[consoleplayer].secretcount);
}

void WS_InitWadStats(void)
{
    if (!netgame && !LoadWadStats())
    {
        CreateWadStats();
    }
}
