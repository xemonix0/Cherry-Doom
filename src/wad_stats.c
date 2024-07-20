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
#include "mn_menu.h"
#include "m_array.h"
#include "m_io.h"
#include "m_misc.h"
#include "p_mobj.h"
#include "p_tick.h"
#include "w_wad.h"
#include "z_zone.h"

typedef enum
{
    ws_version_dsda,
    ws_version_current,
} ws_version_t;

static const char *cherry_data_root = "cherry_doom_data";
static const char *wad_stats_filename = "stats.txt";

static int wad_index = 0;

static map_stats_t *current_map_stats;

wad_stats_t wad_stats = {0};

static char *InitWadDataDir(void)
{
    static char **data_dir_names;
    static char *base_data_dir = NULL;

    int current_dir_index = 0, pwad_index = 1;

    array_clear(data_dir_names);

    for (int i = 0; i < array_size(wadfiles); ++i)
    {
        if (!W_FileContainsMaps(wadfiles[i].name))
        {
            continue;
        }

        if (wadfiles[i].src == source_pwad)
        {
            current_dir_index = pwad_index;
        }
        else if (i)
        {
            continue;
        }

        const char *filename = M_BaseName(wadfiles[i].name);
        const size_t length = strlen(filename) - 3;

        char *name = malloc(length);
        M_StringCopy(name, filename, length);
        M_StringToLower(name);

        array_push(data_dir_names, name);

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

    for (int i = 0; i < array_size(data_dir_names); ++i)
    {
        M_StringConcatF(&wad_data_dir, "/%s", data_dir_names[i]);
        M_MakeDirectory(wad_data_dir);
    }

    return wad_data_dir;
}

static const char *WadStatsPath(void)
{
    static char *wad_stats_path;

    if (!wad_stats_path)
    {
        const char *wad_data_dir = InitWadDataDir();

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

    return (ms1->wad_index == ms2->wad_index)
               ? (ms1->episode == -1 && ms2->episode == -1)
                     ? strncasecmp(ms1->lump, ms2->lump, 8)
                 : ms1->episode == -1 ? 1
                 : ms2->episode == -1 ? -1
                 : ms1->episode == ms2->episode
                     ? ms1->map == ms2->map
                           ? strncasecmp(ms1->lump, ms2->lump, 8)
                           : (ms1->map > ms2->map) - (ms1->map < ms2->map)
                     : (ms1->episode > ms2->episode) - (ms1->episode < ms2->episode)
               : (ms1->wad_index > ms2->wad_index) - (ms1->wad_index < ms2->wad_index);
}

static void CreateWadStats(boolean exists)
{
    char *last_wad_name = NULL;
    wad_index = exists ? wad_index : -1;

    for (int i = numlumps - 1; i > 0; --i)
    {
        if (lumpinfo[i].source == source_other)
        {
            continue;
        }

        if (!MN_StartsWithMapIdentifier(lumpinfo[i].name))
        {
            continue;
        }

        const char *map_name = lumpinfo[i].name;

        if (MapStatsExist(map_name))
        {
            continue;
        }

        map_stats_t ms = {0};
        array_push(wad_stats.maps, ms);

        map_stats_t *p = &wad_stats.maps[array_size(wad_stats.maps) - 1];
        strcpy(p->lump, map_name);

        int episode, map;
        if (gamemode == commercial && sscanf(map_name, "MAP%d", &map) == 1)
        {
            p->map = map;
            p->episode = 1;
        }
        else if (gamemode != commercial && sscanf(map_name, "E%dM%d", &episode, &map) == 2)
        {
            p->episode = episode;
            p->map = map;
        }
        else
        {
            p->episode = -1;
            p->map = -1;
        }

        char *wad_name = M_StringDuplicate(W_WadNameForLump(i));

        if (!last_wad_name || strcmp(last_wad_name, wad_name))
        {
            last_wad_name = wad_name;
            ++wad_index;
        }

        p->wad_name = wad_name;
        p->wad_index = wad_index;

        p->max_kills = -1;
        p->max_items = -1;
        p->max_secrets = -1;

        p->best_time = -1;
        p->best_max_time = -1;
        p->best_sk5_time = -1;
    }

    qsort(wad_stats.maps, array_size(wad_stats.maps), sizeof(*wad_stats.maps),
          CompareMapStats);

    wad_stats.one_wad = !wad_index;
}

#define CURRENT_VERSION_STRING "CH1"

static ws_version_t stats_version;

char *wad_stats_fail;

static int InvalidWadStats(const char *path)
{
    I_Printf(VB_WARNING, "Encountered invalid WAD stats: %s", path);
    wad_stats_fail = "Invalid WAD stats found!";
    return -1;
}

static boolean CheckStatsVersion(const char *line, const char *str,
                                 ws_version_t ver)
{
    if (!strncmp(line, str, strlen(str)))
    {
        stats_version = ver;

        return true;
    }

    return false;
}

static int LoadWadStats(void)
{
    char *buffer = NULL;
    char **lines = NULL;
    int ret = 1;

    while (true)
    {
        const char *path = WadStatsPath();
        if (M_ReadFileToString(path, &buffer) == -1)
        {
            ret = 0;
            break;
        }

        lines = M_StringSplit(buffer, "\n\r");
        int i = 0;
        if (!lines || !lines[i])
        {
            ret = InvalidWadStats(path);
            break;
        }

        if (!(CheckStatsVersion(lines[i], "1", ws_version_dsda)
              || CheckStatsVersion(lines[i], "2", ws_version_dsda)
              || CheckStatsVersion(lines[i], CURRENT_VERSION_STRING,
                                   ws_version_current)))
        {
            I_Printf(VB_WARNING,
                     "Encountered unsupported WAD stats version: %s", path);
            wad_stats_fail = "Unsupported WAD stats version!";
            ret = -1;
            break;
        }

        ++i;

        int kill_check = 0; // Unused, required for version validation
        if (stats_version == ws_version_dsda
            && sscanf(lines[i++], "%d", &kill_check) != 1)
        {
            ret = InvalidWadStats(path);
            break;
        }

        char *last_wad_name = NULL;

        for (; lines[i] && *lines[i]; ++i)
        {
            map_stats_t ms = {0};

            int values = sscanf(
                lines[i], "%8s %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                ms.lump, &ms.episode, &ms.map, &ms.best_skill, &ms.best_time,
                &ms.best_max_time, &ms.best_sk5_time, &ms.total_exits,
                &ms.total_kills, &ms.best_kills, &ms.best_items,
                &ms.best_secrets, &ms.max_kills, &ms.max_items,
                &ms.max_secrets);

            char *wad_name = M_StringDuplicate(W_WadNameForLump(W_GetNumForName(ms.lump)));

            if (values != 15)
            {
                ret = InvalidWadStats(path);
                break;
            }

            ms.wad_name = M_StringDuplicate(wad_name);
            ms.wad_index = wad_index;

            array_push(wad_stats.maps, ms);
        }

        if (ret == -1)
        {
            break;
        }

        // Go through missing maps from all the WADs and add dummy items for
        // them, so that the player can still warp to them from the level table
        CreateWadStats(true);

        break;
    }

    free(lines);
    Z_Free(buffer);

    return ret;
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
                 "WS_SaveWadStats: Failed to save WAD stats file \"%s\".",
                 path);
        return;
    }

    fprintf(file, CURRENT_VERSION_STRING "\n");

    for (int i = 0; i < array_size(wad_stats.maps); ++i)
    {
        map_stats_t *ms = &wad_stats.maps[i];

        fprintf(file, "%s %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                ms->lump, ms->episode, ms->map, ms->best_skill, ms->best_time,
                ms->best_max_time, ms->best_sk5_time, ms->total_exits,
                ms->total_kills, ms->best_kills, ms->best_items,
                ms->best_secrets, ms->max_kills, ms->max_items,
                ms->max_secrets);
    }

    fclose(file);
}

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

void WS_WatchEnterMap(void)
{
    if (!wad_stats.maps)
    {
        return;
    }

    current_map_stats = MapStats(gameepisode, gamemap);
}

void WS_UnwatchMap(void)
{
    if (!wad_stats.maps || !current_map_stats)
    {
        return;
    }

    current_map_stats = NULL;
}

void WS_WatchKill(void)
{
    if (!wad_stats.maps || !current_map_stats)
    {
        return;
    }

    ++current_map_stats->total_kills;
}

void WS_WatchExitMap(void)
{
    if (!wad_stats.maps || !current_map_stats)
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

void WS_InitWadStats(void)
{
    if (!netgame && !LoadWadStats())
    {
        CreateWadStats(false);
    }
}
