//
//  Copyright (C) 2024 by Xemonix
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

#include "mn_level_table.h"

#include "doomdef.h"
#include "doomstat.h"
#include "m_array.h"
#include "m_misc.h"
#include "m_swap.h"
#include "mn_menu.h"
#include "mn_setup.h"
#include "r_defs.h"
#include "v_video.h"
#include "w_wad.h"
#include "wad_stats.h"
#include "z_zone.h"

setup_tab_t level_table_tabs[] = {
    {"stats"},
    {"times"},
    {"summary"},
    
    {NULL}
};

setup_menu_t *level_table[lt_page_max] = {NULL};

// ====================================
// UTILS

boolean LT_IsLevelsPage(int page)
{
    switch (page)
    {
        case lt_page_stats:
        case lt_page_times:
            return true;
        default:
            return false;
    }
}

static void FreeMText(const char *m_text)
{
    union
    {
        const char *c;
        char *s;
    } text;

    text.c = m_text;
    free(text.s);
}

static void StringPrintTime(char **dest, int tics)
{
    if (tics >= 0)
    {
        M_StringPrintF(dest, "%d:%05.2f", tics / TICRATE / 60,
                       (float)(tics % (60 * TICRATE)) / TICRATE);
    }
    else
    {
        *dest = M_StringDuplicate("- : --");
    }
}

// ====================================
// BUILDING & INITIALIZING

static void InsertLastItem(setup_menu_t **menu)
{
    setup_menu_t item = {NULL, S_SKIP | S_END};
    array_push(*menu, item);
}

// -----------
// Level Table

#define LTBL_X 16

static void LevelsInsertWadName(setup_menu_t **menu, const char *wad_name)
{
    setup_menu_t item = {wad_name, S_SKIP | S_LEFTJUST | S_TITLE,
                         SCREENWIDTH / 2 - MN_GetPixelWidth(wad_name) / 2,
                         M_SPC};
    array_push(*menu, item);
}

static void LevelsInsertRow(setup_menu_t **menu, const char *text, int map_i,
                            boolean display_stats)
{
    extern void LT_Warp(void);

    int64_t flags = S_LEFTJUST | S_TITLE | S_FUNCTION;
    if (display_stats)
    {
        flags |= S_LTBL_MAP;
    }

    setup_menu_t item =
        {text, flags, LTBL_X, M_SPC, .action = LT_Warp, .var.map_i = map_i};
    array_push(*menu, item);
}

static void LevelsBuild(void)
{
    for (int p = 0; LT_IsLevelsPage(p); ++p)
    {
        setup_menu_t **page = &level_table[p];

        int last_wad_index = -1;
        for (int i = 0; i < array_size(wad_stats.maps); ++i)
        {
            const map_stats_t *ms = &wad_stats.maps[i];
            if (ms->episode == -1)
            {
                continue;
            }

            const int wad_index = ms->wad_index;

            if (!wad_stats.one_wad && last_wad_index != wad_index)
            {
                last_wad_index = wad_index;

                LevelsInsertWadName(page, M_StringDuplicate(ms->wad_name));
            }

            LevelsInsertRow(page, M_StringDuplicate(ms->lump), i, !wad_index);
        }

        InsertLastItem(page);
    }
}

// -----------
// WAD Summary

typedef struct
{
    int map_count;
    int completed;

    int timed;
    int max_timed;
    int sk5_timed;

    int max_kills;
    int max_items;
    int max_secrets;

    int best_skill;
    int best_time;
    int best_max_time;
    int best_sk5_time;

    int best_kills;
    int best_items;
    int best_secrets;
} summary_t;

static summary_t wad_summary;

static void SummaryCalculate(void)
{
    memset(&wad_summary, 0, sizeof(wad_summary));

    wad_summary.best_skill = sk_nightmare + 2;
    for (int i = 0; i < array_size(wad_stats.maps); ++i)
    {
        map_stats_t *ms = &wad_stats.maps[i];

        if (ms->wad_index)
        {
            break;
        }

        if (ms->episode == -1)
        {
            continue;
        }

        ++wad_summary.map_count;

        if (!ms->best_skill)
        {
            continue;
        }

        ++wad_summary.completed;

        wad_summary.best_skill = MIN(ms->best_skill, wad_summary.best_skill);

        wad_summary.max_kills += ms->max_kills;
        wad_summary.max_items += ms->max_items;
        wad_summary.max_secrets += ms->max_secrets;

        wad_summary.best_kills += ms->best_kills;
        wad_summary.best_items += ms->best_items;
        wad_summary.best_secrets += ms->best_secrets;

        if (ms->best_time >= 0)
        {
            ++wad_summary.timed;
            wad_summary.best_time += ms->best_time;
        }

        if (ms->best_max_time >= 0)
        {
            ++wad_summary.max_timed;
            wad_summary.best_max_time += ms->best_max_time;
        }

        if (ms->best_sk5_time >= 0)
        {
            ++wad_summary.sk5_timed;
            wad_summary.best_sk5_time += ms->best_sk5_time;
        }
    }
}

static void SummaryBuild(void)
{
    setup_menu_t **page = &level_table[lt_page_summary];

    SummaryCalculate();

    // See SummaryDraw
    InsertLastItem(page);
}

// --------------
// Initialization

static void Reset(void)
{
    for (int i = 0; i < lt_page_max; ++i)
    {
        for (int j = 0; j < array_size(level_table[i]); ++j)
        {
            if (level_table[i][j].m_text)
            {
                FreeMText(level_table[i][j].m_text);
            }
        }

        array_clear(level_table[i]);
    }
}

void LT_Build(void)
{
    Reset();

    LevelsBuild();
    SummaryBuild();
}

// ====================================
// DRAWING

// Used for formatting
typedef enum
{
    statf_maps,
    statf_skill,
    statf_generic,
    statf_time,
} lt_statf_t;

// -----------
// Level Table

// lt_page_stats
enum
{
    stats_skill_x,
    stats_kills_x,
    stats_items_x,
    stats_secrets_x,

    stats_max_x,
};

static const int stats_columns_x[] = {92, 182, 251, 304};

// lt_page_times
enum
{
    times_time_x,
    times_max_time_x,
    times_sk5_time_x,

    times_max_x,
};

static const int times_columns_x[] = {136, 220, 304};

static const int max_rows = 14;
static int scroll_pos = 0;

#define SCRL_X      (SCREENWIDTH - 9)
#define SCRL_UP_Y   (M_TAB_Y + M_SPC)
#define SCRL_DOWN_Y (M_Y_WARN - 5)

#define SCRL_UP     0x01
#define SCRL_DOWN   0x02
static int scroll_indicators = 0x00;

static void UpdateScrollingIndicators(int rows)
{
    scroll_indicators = (rows - max_rows - scroll_pos - 1) > 0
                            ? (scroll_indicators | SCRL_DOWN)
                            : (scroll_indicators & ~SCRL_DOWN);
    scroll_indicators = scroll_pos ? (scroll_indicators | SCRL_UP)
                                   : (scroll_indicators & ~SCRL_UP);
}

static char *LevelsFormatGenericStat(int a, int b)
{
    char *str = NULL;

    switch (level_table_stats_format ? level_table_stats_format
                                     : hud_stats_format)
    {
        case STATSFORMAT_RATIO:
            M_StringPrintF(&str, "%d/%d", a, b);
            break;
        case STATSFORMAT_BOOLEAN:
            M_StringPrintF(&str, "%s", (a >= b) ? "YES" : "NO");
            break;
        case STATSFORMAT_PERCENTAGE:
            M_StringPrintF(&str, "%d%%", (!b) ? 100 : a * 100 / b);
            break;
        case STATSFORMAT_REMAINING:
            M_StringPrintF(&str, "%d", b - a);
            break;
    }

    return str;
}

static setup_menu_t LevelsFormatStat(lt_statf_t type, boolean done, int a,
                                     int b)
{
    char *text = NULL;
    int64_t flags = 0;

    if (done)
    {
        switch (type)
        {
            case statf_skill:
                M_StringPrintF(&text, "%s", default_skill_strings[a]);
                if (a >= sk_hard + 1)
                {
                    flags |= S_ALT_COL;
                }
                break;
            case statf_generic:
                M_StringPrintF(&text, "%s", LevelsFormatGenericStat(a, b));
                if (a == b)
                {
                    flags |= S_ALT_COL;
                }
                break;
            case statf_time:
                StringPrintTime(&text, a);
                flags |= S_ALT_COL;
                break;
            default:
                break;
        }
    }
    else if (type == statf_time)
    {
        StringPrintTime(&text, -1);
    }
    else
    {
        text = M_StringDuplicate("-");
    }

    setup_menu_t item = {text, flags};
    return item;
}

static void LevelsDrawStat(lt_statf_t type, boolean done, int a, int b, int x,
                           int accum_y, int64_t additional_flags)
{
    setup_menu_t item = LevelsFormatStat(type, done, a, b);

    const int64_t flags = item.m_flags | additional_flags;
    const int color = flags & S_ALT_COL  ? CR_ALT_COL
                      : flags & S_SELECT ? CR_SELECT
                                         : CR_ITEM;
    M_snprintf(menu_buffer, sizeof(menu_buffer), "%s", item.m_text);
    MN_DrawMenuStringEx(flags, x - (MN_GetPixelWidth(menu_buffer) + 4), accum_y,
                        color);

    FreeMText(item.m_text);
}

static void LevelsDrawRow(setup_menu_t *item, int accum_y, int page)
{
    MN_DrawItem(item, accum_y);

    const int64_t flags = item->m_flags;

    const map_stats_t *ms = &wad_stats.maps[item->var.map_i];
    const boolean done = ms->best_skill;

    const int64_t additional_flags = flags & (S_HILITE | S_SELECT);
    switch (page)
    {
        case lt_page_stats:
            LevelsDrawStat(statf_skill, done,
                           ms->best_skill, 0,
                           stats_columns_x[stats_skill_x], accum_y,
                           additional_flags);
            LevelsDrawStat(statf_generic, done,
                           ms->best_kills, ms->max_kills,
                           stats_columns_x[stats_kills_x], accum_y,
                           additional_flags);
            LevelsDrawStat(statf_generic, done,
                           ms->best_items, ms->max_items,
                           stats_columns_x[stats_items_x], accum_y,
                           additional_flags);
            LevelsDrawStat(statf_generic, done,
                           ms->best_secrets, ms->max_secrets,
                           stats_columns_x[stats_secrets_x], accum_y,
                           additional_flags);
            break;
        case lt_page_times:
            LevelsDrawStat(statf_time, done,
                           ms->best_time, 0,
                           times_columns_x[times_time_x], accum_y,
                           additional_flags);
            LevelsDrawStat(statf_time, done && ms->best_max_time >= 0,
                           ms->best_max_time, 0,
                           times_columns_x[times_max_time_x], accum_y,
                           additional_flags);
            LevelsDrawStat(statf_time, done && ms->best_sk5_time >= 0,
                           ms->best_sk5_time, 0,
                           times_columns_x[times_sk5_time_x], accum_y,
                           additional_flags);
            break;
        default:
            break;
    }

    menu_buffer[0] = '\0';
    MN_BlinkingArrowRight(item);
    const int arrow_x =
        ((page == lt_page_stats) ? stats_columns_x[stats_max_x - 1]
                                 : times_columns_x[times_max_x - 1])
        - 4;
    MN_DrawMenuStringEx(flags, arrow_x, accum_y,
                        flags & S_SELECT ? CR_SELECT : CR_TITLE);
}

static void LevelsDrawHeader(const char *text, int x, int y)
{
    M_snprintf(menu_buffer, sizeof(menu_buffer), "%s", text);
    MN_DrawMenuStringEx(0, x - (MN_GetPixelWidth(menu_buffer) + 4), y,
                        CR_TITLE);
}

static void LevelsDraw(setup_menu_t *current_menu, int current_page)
{
    int accum_y = M_Y;

    switch (current_page)
    {
        case lt_page_stats:
            LevelsDrawHeader("SKILL", stats_columns_x[stats_skill_x],    accum_y);
            LevelsDrawHeader("K",     stats_columns_x[stats_kills_x],    accum_y);
            LevelsDrawHeader("I",     stats_columns_x[stats_items_x],    accum_y);
            LevelsDrawHeader("S",     stats_columns_x[stats_secrets_x],  accum_y);
            break;
        case lt_page_times:
            LevelsDrawHeader("TIME",         times_columns_x[times_time_x],     accum_y);
            LevelsDrawHeader("100% TIME",    times_columns_x[times_max_time_x], accum_y);
            LevelsDrawHeader("SKILL 5 TIME", times_columns_x[times_sk5_time_x], accum_y);
            break;
        default:
            break;
    }

    accum_y += M_SPC;

    int rows = 0;
    for (setup_menu_t *src = current_menu; !(src->m_flags & S_END); ++src)
    {
        boolean skip = false;
        if (rows - scroll_pos < 0 || rows - scroll_pos > max_rows)
        {
            skip = true;
        }

        if (!(src->m_flags & S_DIRECT))
        {
            ++rows;
        }

        if (skip)
        {
            // prevent mouse interaction with skipped entries
            mrect_t *rect = &src->rect;
            rect->x = 0;
            rect->y = 0;
            rect->w = 0;
            rect->h = 0;

            continue;
        }

        if (src->m_flags & S_LTBL_MAP)
        {
            LevelsDrawRow(src, accum_y, current_page);
        }

        if (src->m_flags & S_SHOWDESC)
        {
            MN_DrawItem(src, accum_y);
        }

        if (!(src->m_flags & S_DIRECT))
        {
            accum_y += src->m_y;
        }
    }

    if (scroll_indicators & SCRL_UP)
    {
        patch_t *patch = W_CacheLumpName("SCRLUP", PU_CACHE);

        int x = SCRL_X - SHORT(patch->width) / 2;
        int y = SCRL_UP_Y;

        V_DrawPatch(x, y, patch);
    }

    if (scroll_indicators & SCRL_DOWN)
    {
        patch_t *patch = W_CacheLumpName("SCRLDOWN", PU_CACHE);

        int x = SCRL_X - SHORT(patch->width) / 2;
        int y = SCRL_DOWN_Y;

        V_DrawPatch(x, y, patch);
    }
}

// -----------
// WAD Summary

#define LT_SUM_X (SCREENWIDTH / 2)
#define LT_SUM_Y (M_Y + M_SPC * 2)

static char *SummaryFormatGenericStat(boolean done, int a, int b)
{
    char *str = NULL;

    switch (level_table_stats_format ? level_table_stats_format
                                     : hud_stats_format)
    {
        case STATSFORMAT_RATIO:
            M_StringPrintF(&str, "%d", a);
            if (done)
            {
                M_StringConcatF(&str, " / %d", b);
            }
            break;
        case STATSFORMAT_BOOLEAN:
            M_StringPrintF(&str, "%s", (done && a >= b) ? "YES" : "NO");
            break;
        case STATSFORMAT_PERCENTAGE:
            if (done)
            {
                M_StringPrintF(&str, "%d%%", (!b) ? 100 : a * 100 / b);
            }
            else
            {
                M_StringPrintF(&str, "N/A");
            }
            break;
        case STATSFORMAT_REMAINING:
            if (done)
            {
                M_StringPrintF(&str, "%d", b - a);
            }
            else
            {
                M_StringPrintF(&str, "N/A");
            }
            break;
    }

    return str;
}

static setup_menu_t SummaryFormatStat(lt_statf_t type, boolean done, int a,
                                      int b)
{
    char *text = NULL;
    int64_t flags = 0;

    switch (type)
    {
        case statf_maps:
            if (a > 0)
            {
                M_StringPrintF(&text, "%d", a);

                if (done)
                {
                    M_StringConcatF(&text, " / %d", b);

                    if (a == b)
                    {
                        flags |= S_ALT_COL;
                    }
                }
            }
            else
            {
                text = M_StringDuplicate("-");
            }
            break;
        case statf_skill:
            if (done)
            {
                M_StringPrintF(&text, "%s", default_skill_strings[a]);

                if (a >= sk_hard + 1)
                {
                    flags |= S_ALT_COL;
                }
            }
            else
            {
                text = M_StringDuplicate("-");
            }
            break;
        case statf_generic:
            if (a > 0)
            {
                M_StringPrintF(&text, "%s", SummaryFormatGenericStat(done, a, b));

                if (done && a == b)
                {
                    flags |= S_ALT_COL;
                }
            }
            else
            {
                text = M_StringDuplicate("-");
            }
            break;
        case statf_time:
            if (a > 0)
            {
                StringPrintTime(&text, a);
                if (done)
                {
                    flags |= S_ALT_COL;
                }
            }
            else
            {
                StringPrintTime(&text, -1);
            }
            break;
    }

    setup_menu_t item = {text, flags};
    return item;
}

static void SummaryDrawRow(const char *heading, lt_statf_t type, boolean done,
                           int a, int b, int accum_y)
{
    M_snprintf(menu_buffer, sizeof(menu_buffer), "%s", heading);
    MN_DrawMenuStringEx(0, LT_SUM_X - (MN_GetPixelWidth(menu_buffer) + 2),
                        accum_y, CR_TITLE);

    setup_menu_t item = SummaryFormatStat(type, done, a, b);

    const int64_t flags = item.m_flags;
    const int color = flags & S_ALT_COL ? CR_ALT_COL : CR_ITEM;
    M_snprintf(menu_buffer, sizeof(menu_buffer), "%s", item.m_text);
    MN_DrawMenuStringEx(flags, LT_SUM_X + 2, accum_y, color);

    FreeMText(item.m_text);
}

static void SummaryDraw(void)
{
    int accum_y = LT_SUM_Y;

    const int map_count = wad_summary.map_count;
    boolean done = wad_summary.completed == map_count;

    SummaryDrawRow("Maps Completed", statf_maps, true,
                   wad_summary.completed, map_count,
                   accum_y);
    accum_y += M_SPC * 2;

    SummaryDrawRow("Skill", statf_skill, done,
                   wad_summary.best_skill, 0,
                   accum_y);
    accum_y += M_SPC * 2;

    SummaryDrawRow("Kill Completion", statf_generic, done,
                   wad_summary.best_kills, wad_summary.max_kills,
                   accum_y);
    accum_y += M_SPC;
    SummaryDrawRow("Item Completion", statf_generic, done,
                   wad_summary.best_items, wad_summary.max_items,
                   accum_y);
    accum_y += M_SPC;
    SummaryDrawRow("Secret Completion", statf_generic, done,
                   wad_summary.best_secrets, wad_summary.max_secrets,
                   accum_y);
    accum_y += M_SPC * 2;

    SummaryDrawRow("Total Time", statf_time,
                   wad_summary.timed == map_count,
                   wad_summary.best_time, 0,
                   accum_y);
    accum_y += M_SPC;
    SummaryDrawRow("100% Time", statf_time,
                   wad_summary.max_timed == map_count,
                   wad_summary.best_max_time, 0,
                   accum_y);
    accum_y += M_SPC;
    SummaryDrawRow("Nightmare Time", statf_time,
                   wad_summary.sk5_timed == map_count,
                   wad_summary.best_sk5_time, 0,
                   accum_y);
}

// -------------
// Master Drawer

void LT_Draw(setup_menu_t *current_menu, int current_page)
{
    if (LT_IsLevelsPage(current_page))
    {
        LevelsDraw(current_menu, current_page);
    }
    else
    {
        SummaryDraw();
    }
}

// ====================================
// SCROLLING

#define LT_SCROLL_BUFFER 3

void LT_KeyboardScroll(setup_menu_t *current_menu, setup_menu_t *current_item)
{
    if (!set_lvltbl_active)
    {
        return;
    }

    int current_row = 0, rows = 0;
    for (setup_menu_t *src = current_menu; !(src->m_flags & S_END); ++src)
    {
        if (src == current_item)
        {
            current_row = rows;
        }

        if (!(src->m_flags & S_DIRECT))
        {
            ++rows;
        }
    }

    int buffer_i = MIN(LT_SCROLL_BUFFER, rows - current_row - 1);
    int top_buffer_i = MIN(LT_SCROLL_BUFFER, current_row);

    if (rows > max_rows)
    {
        while (current_row - scroll_pos > max_rows - buffer_i)
        {
            ++scroll_pos;
        }
    }

    while (scroll_pos && current_row - scroll_pos < top_buffer_i)
    {
        --scroll_pos;
    }

    UpdateScrollingIndicators(rows);
}

boolean LT_MouseScroll(setup_menu_t *current_menu, int inc)
{
    if (!set_lvltbl_active || menu_input != mouse_mode)
    {
        return false;
    }

    int rows = 0;
    for (setup_menu_t *src = current_menu; !(src->m_flags & S_END); ++src)
    {
        if (!(src->m_flags & S_DIRECT))
        {
            ++rows;
        }
    }

    scroll_pos = MAX(0, MIN(rows - max_rows - 1, scroll_pos + inc));

    UpdateScrollingIndicators(rows);

    return true;
}

void LT_ResetScroll(setup_menu_t *current_menu, int set_item_on)
{
    if (set_lvltbl_active)
    {
        scroll_pos = 0;
        LT_KeyboardScroll(current_menu, current_menu + set_item_on);
    }
}
