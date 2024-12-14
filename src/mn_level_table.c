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

#include <stdint.h>
#include <string.h>

#include "doomdef.h"
#include "doomstat.h"
#include "doomtype.h"
#include "m_array.h"
#include "m_misc.h"
#include "mn_menu.h"
#include "mn_internal.h"
#include "st_widgets.h"
#include "wad_stats.h"

#define LT_X_MARGIN         16
#define LT_LEFT_X           (LT_X_MARGIN)
#define LT_RIGHT_X          (SCREENWIDTH - LT_X_MARGIN)

#define LT_STATS_SKILL_X    92
#define LT_STATS_KILLS_X    182
#define LT_STATS_ITEMS_X    251
#define LT_STATS_SECRETS_X  (LT_RIGHT_X)

#define LT_TIMES_TIME_X     136
#define LT_TIMES_MAX_TIME_X 220
#define LT_TIMES_SK5_TIME_X (LT_RIGHT_X)

#define LT_SUMMARY_X        (SCREENWIDTH / 2)
#define LT_SUMMARY_Y        (M_Y + M_SPC * 2)

#define LT_MAX_ROWS         14
#define LT_SCROLL_BUFFER    3

typedef enum
{
    format_maps,
    format_skill,
    format_stat,
    format_time,
} value_format_t;

typedef enum
{
    context_levels,
    context_summary,
} value_context_t;

typedef struct
{
    int map_count, maps_completed;
    int timed, max_timed, sk5_timed;
    int max_kills, max_items, max_secrets;
    int best_skill;
    int best_kills, best_items, best_secrets;
    int best_time, best_max_time, best_sk5_time;
} summary_t;

typedef struct
{
    char *text;
    int64_t flags;
} formatted_value_t;

static int scroll_pos;
static summary_t summary;

setup_tab_t level_table_tabs[] = {
    {"stats"},
    {"times"},
    {"summary"},
    
    {NULL}
};

setup_menu_t *level_table[lt_page_max] = {NULL};

// Utility functions
//===============================================

inline boolean LT_IsLevelsPage(int page)
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

static void StringPrintTime(char **dest, int tics)
{
    if (tics >= 60 * 60 * TICRATE)
    {
        M_StringPrintF(dest, "%d:%02d:%05.2f", tics / TICRATE / 60 / 60,
                       (tics % (60 * 60 * TICRATE)) / TICRATE / 60,
                       (float)(tics % (60 * TICRATE)) / TICRATE);
    }
    else if (tics >= 0)
    {
        M_StringPrintF(dest, "%d:%05.2f", tics / TICRATE / 60,
                       (float)(tics % (60 * TICRATE)) / TICRATE);
    }
    else
    {
        *dest = M_StringDuplicate("- : --");
    }
}

static void InsertLastItem(setup_menu_t **menu)
{
    setup_menu_t item = {NULL, S_SKIP | S_END};
    array_push(*menu, item);
}

// Building
//===============================================

// Level Table
// -----------

static void LevelsInsertWadName(setup_menu_t **menu, char *wad_name)
{
    setup_menu_t item = {wad_name, S_SKIP | S_LEFTJUST | S_TITLE,
                         SCREENWIDTH / 2 - MN_GetPixelWidth(wad_name) / 2,
                         M_SPC};
    array_push(*menu, item);
}

static void LevelsInsertRow(setup_menu_t **menu, char *text, int map_i,
                            boolean display_stats)
{
    extern void LT_Warp(void);

    int64_t flags = S_LEFTJUST | S_TITLE | S_FUNC2;
    if (display_stats)
    {
        flags |= S_LTBL_MAP;
    }

    setup_menu_t item =
        {text, flags, LT_LEFT_X, M_SPC, .action = LT_Warp, .var.map_i = map_i};
    array_push(*menu, item);
}

static void InsertResetButton(setup_menu_t** menu)
{
    setup_menu_t item = {NULL, S_RESET, X_BUTTON, Y_BUTTON};
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

            LevelsInsertRow(page, M_StringDuplicate(ms->lump), i,
                            TRACKING_WAD_STATS && !wad_index);
        }

        if (TRACKING_WAD_STATS)
        {
            InsertResetButton(page);
        }

        InsertLastItem(page);

        if (p == 0 && !TRACKING_WAD_STATS)
        {
            break;
        }
    }
}

// WAD summary
// -----------

static void SummaryCalculate(void)
{
    memset(&summary, 0, sizeof(summary));
    summary.best_time = summary.best_max_time = summary.best_sk5_time = -1;

    if (!TRACKING_WAD_STATS)
    {
        return;
    }

    summary.best_skill = sk_nightmare + 2;
    for (int i = 0; i < array_size(wad_stats.maps); ++i)
    {
        map_stats_t *ms = &wad_stats.maps[i];

        if (ms->wad_index || ms->episode == -1)
        {
            continue;
        }

        summary.map_count++;

        if (!ms->best_skill)
        {
            continue;
        }

        summary.maps_completed++;
        summary.best_skill    = MIN(ms->best_skill, summary.best_skill);
        summary.max_kills    += ms->max_kills;
        summary.max_items    += ms->max_items;
        summary.max_secrets  += ms->max_secrets;
        summary.best_kills   += ms->best_kills;
        summary.best_items   += ms->best_items;
        summary.best_secrets += ms->best_secrets;

        if (ms->best_time >= 0)
        {
            summary.timed++;
            summary.best_time += ms->best_time;
        }

        if (ms->best_max_time >= 0)
        {
            summary.max_timed++;
            summary.best_max_time += ms->best_max_time;
        }

        if (ms->best_sk5_time >= 0)
        {
            summary.sk5_timed++;
            summary.best_sk5_time += ms->best_sk5_time;
        }
    }
}

void LT_RecalculateSummary(void)
{
    SummaryCalculate();
}

static void SummaryBuild(void)
{
    setup_menu_t **page = &level_table[lt_page_summary];
    InsertLastItem(page); // See SummaryDraw

    SummaryCalculate();
}

// Initialization
//===============================================

void LT_Reset(void)
{
    for (int i = 0; i < lt_page_max; ++i)
    {
        for (int j = 0; j < array_size(level_table[i]); ++j)
        {
            if (level_table[i][j].m_text)
            {
                free(level_table[i][j].m_text);
            }
        }

        array_clear(level_table[i]);
    }
}

void LT_Build(void)
{
    if (TRACKING_WAD_STATS)
    {
        level_table_tabs[lt_page_times].flags &= ~S_DISABLE;
        level_table_tabs[lt_page_summary].flags &= ~S_DISABLE;
        LevelsBuild();
        SummaryBuild();
    }
    else
    {
        level_table_tabs[lt_page_times].flags |= S_DISABLE;
        level_table_tabs[lt_page_summary].flags |= S_DISABLE;
        LevelsBuild();
    }
}

// Drawing
//===============================================

// Helper functions
// ----------------

static char *FormatStat(int a, int b, boolean known_total)
{
    char *str = NULL;

    switch (lt_stats_format != STATSFORMAT_MATCHHUD ? lt_stats_format
                                                    : hud_stats_format)
    {
        case STATSFORMAT_RATIO:
            M_StringPrintF(&str, "%d", a);
            if (known_total)
            {
                M_StringConcatF(&str, "/%d", b);
            }
            break;
        case STATSFORMAT_BOOLEAN:
            M_StringPrintF(&str, "%s", (known_total && a >= b) ? "YES" : "NO");
            break;
        case STATSFORMAT_PERCENT:
            if (known_total)
            {
                M_StringPrintF(&str, "%d%%", !b ? 100 : a * 100 / b);
            }
            else
            {
                M_StringPrintF(&str, "N/A");
            }
            break;
        case STATSFORMAT_REMAINING:
            if (known_total)
            {
                M_StringPrintF(&str, "%d", b - a);
            }
            else
            {
                M_StringPrintF(&str, "N/A");
            }
            break;
        case STATSFORMAT_COUNT:
            M_StringPrintF(&str, "%d", a);
        default:
            break;
    }

    return str;
}

static formatted_value_t FormatValue(value_context_t context,
                                     value_format_t format, boolean done, int a,
                                     int b)
{
    char *text = NULL;
    int64_t flags = 0;

    switch (format)
    {
        case format_maps:
            if (a)
            {
                M_StringPrintF(&text, "%d", a);
                if (done)
                {
                    M_StringConcatF(&text, "/%d", b);
                    if (a == b) flags |= S_ALT_COL;
                }
            }
            break;
        case format_skill:
            if (done)
            {
                M_StringPrintF(&text, "%s", default_skill_strings[a]);
                if (a >= 4) flags |= S_ALT_COL;
            }
            break;
        case format_stat:
            if ((context == context_levels && done)
                || (context == context_summary && a))
            {
                text = FormatStat(a, b, done);
                if (done && a == b) flags |= S_ALT_COL;
            }
            break;
        case format_time:
            StringPrintTime(&text, a);
            if (done) flags |= S_ALT_COL;
    }

    if (!text)
    {
        text = M_StringDuplicate("-");
    }

    formatted_value_t value = {text, flags};
    return value;
}

// Level Table
// -----------

static void LevelsDrawValue(value_format_t format, boolean done, int a, int b,
                            int x, int accum_y, int64_t additional_flags)
{
    formatted_value_t value = FormatValue(context_levels, format, done, a, b);
    const int64_t flags = value.flags | additional_flags;
    const int color = flags & S_ALT_COL  ? CR_ALT_COL
                      : flags & S_SELECT ? CR_SELECT
                                         : CR_ITEM;
    M_snprintf(menu_buffer, sizeof(menu_buffer), "%s", value.text);
    MN_DrawMenuStringEx(flags, x - (MN_GetPixelWidth(menu_buffer) + 4), accum_y,
                        color);

    free(value.text);
}

static void LevelsDrawRow(setup_menu_t *src, int y, int page)
{
    const int64_t flags = src->m_flags;

    if (flags & S_SHOWDESC)
    {
        MN_DrawItem(src, y);
    }

    if (!(flags & S_LTBL_MAP))
    {
        return;
    }

    const map_stats_t *ms = &wad_stats.maps[src->var.map_i];
    const boolean done = ms->best_skill;

    const int64_t additional_flags = flags & (S_HILITE | S_SELECT);
    switch (page)
    {
        case lt_page_stats:
            LevelsDrawValue(format_skill, done,
                            ms->best_skill, 0,
                            LT_STATS_SKILL_X, y,
                            additional_flags);
            LevelsDrawValue(format_stat, done,
                            ms->best_kills, ms->max_kills,
                            LT_STATS_KILLS_X, y,
                            additional_flags);
            LevelsDrawValue(format_stat, done,
                            ms->best_items, ms->max_items,
                            LT_STATS_ITEMS_X, y,
                            additional_flags);
            LevelsDrawValue(format_stat, done,
                            ms->best_secrets, ms->max_secrets,
                            LT_STATS_SECRETS_X, y,
                            additional_flags);
            break;
        case lt_page_times:
            LevelsDrawValue(format_time, done,
                            ms->best_time, 0,
                            LT_TIMES_TIME_X, y,
                            additional_flags);
            LevelsDrawValue(format_time, done && ms->best_max_time >= 0,
                            ms->best_max_time, 0,
                            LT_TIMES_MAX_TIME_X, y,
                            additional_flags);
            LevelsDrawValue(format_time, done && ms->best_sk5_time >= 0,
                            ms->best_sk5_time, 0,
                            LT_TIMES_SK5_TIME_X, y,
                            additional_flags);
            break;
        default:
            break;
    }

    menu_buffer[0] = '\0';
    MN_BlinkingArrowRight(src);
    MN_DrawMenuStringEx(flags, LT_RIGHT_X - 4, y,
                        flags & S_SELECT ? CR_SELECT : CR_TITLE);
}

static void LevelsDrawHeader(const char *text, int x, int y)
{
    M_snprintf(menu_buffer, sizeof(menu_buffer), "%s", text);
    MN_DrawMenuStringEx(0, x - (MN_GetPixelWidth(menu_buffer) + 4), y,
                        CR_TITLE);
}

static void LevelsDrawPageHeaders(int page, int y)
{
    switch (page)
    {
        case lt_page_stats:
            LevelsDrawHeader("SKILL", LT_STATS_SKILL_X,    y);
            LevelsDrawHeader("K",     LT_STATS_KILLS_X,    y);
            LevelsDrawHeader("I",     LT_STATS_ITEMS_X,    y);
            LevelsDrawHeader("S",     LT_STATS_SECRETS_X,  y);
            break;
        case lt_page_times:
            LevelsDrawHeader("TIME",         LT_TIMES_TIME_X,     y);
            LevelsDrawHeader("100% TIME",    LT_TIMES_MAX_TIME_X, y);
            LevelsDrawHeader("SKILL 5 TIME", LT_TIMES_SK5_TIME_X, y);
            break;
        default:
            break;
    }
}

static void DrawNoTrackingWarning(int y)
{
    M_snprintf(menu_buffer, sizeof(menu_buffer), "Stats tracking is disabled");
    MN_DrawMenuStringEx(0, SCREENWIDTH / 2 - MN_GetPixelWidth(menu_buffer) / 2,
                        y, CR_NONE);
}

static void LevelsDraw(setup_menu_t *menu, int page)
{
    int accum_y = M_Y;

    if (TRACKING_WAD_STATS)
    {
        LevelsDrawPageHeaders(page, accum_y);
    }
    else
    {
        DrawNoTrackingWarning(accum_y);
    }

    accum_y += M_SPC;

    int rows = 0;
    for (setup_menu_t *src = menu; !(src->m_flags & S_END); ++src)
    {
        boolean skip =
            (rows - scroll_pos < 0 || rows - scroll_pos > LT_MAX_ROWS)
            && !(src->m_flags & S_RESET); // Always draw the reset button

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

        LevelsDrawRow(src, accum_y, page);

        if (!(src->m_flags & S_DIRECT))
        {
            accum_y += src->m_y;
        }
    }

    MN_DrawScrollIndicators();
}

// WAD summary
// -----------

static void SummaryDrawRow(const char *heading, value_format_t format,
                           boolean done, int a, int b, int y)
{
    // Draw heading
    M_snprintf(menu_buffer, sizeof(menu_buffer), "%s", heading);
    MN_DrawMenuStringEx(0, LT_SUMMARY_X - (MN_GetPixelWidth(menu_buffer) + 2),
                        y, CR_TITLE);

    // Format and draw value
    formatted_value_t value = FormatValue(context_summary, format, done, a, b);
    const int color = value.flags & S_ALT_COL ? CR_ALT_COL : CR_ITEM;
    M_snprintf(menu_buffer, sizeof(menu_buffer), "%s", value.text);
    MN_DrawMenuStringEx(value.flags, LT_SUMMARY_X + 2, y, color);

    free(value.text);
}

static void SummaryDraw(void)
{
    int accum_y = LT_SUMMARY_Y;

    boolean done =
        summary.map_count != 0 && summary.maps_completed == summary.map_count;

    SummaryDrawRow("Maps Completed", format_maps, true,
                   summary.maps_completed, summary.map_count,
                   accum_y);
    accum_y += M_SPC * 2;

    SummaryDrawRow("Skill", format_skill, done,
                   summary.best_skill, 0,
                   accum_y);
    accum_y += M_SPC * 2;

    SummaryDrawRow("Kill Completion", format_stat, done,
                   summary.best_kills, summary.max_kills,
                   accum_y);
    accum_y += M_SPC;
    SummaryDrawRow("Item Completion", format_stat, done,
                   summary.best_items, summary.max_items,
                   accum_y);
    accum_y += M_SPC;
    SummaryDrawRow("Secret Completion", format_stat, done,
                   summary.best_secrets, summary.max_secrets,
                   accum_y);
    accum_y += M_SPC * 2;

    SummaryDrawRow("Total Time", format_time,
                   summary.timed == summary.map_count,
                   summary.best_time, 0,
                   accum_y);
    accum_y += M_SPC;
    SummaryDrawRow("100% Time", format_time,
                   summary.max_timed == summary.map_count,
                   summary.best_max_time, 0,
                   accum_y);
    accum_y += M_SPC;
    SummaryDrawRow("Nightmare Time", format_time,
                   summary.sk5_timed == summary.map_count,
                   summary.best_sk5_time, 0,
                   accum_y);
}

// Master Drawer
// -------------

void LT_Draw(setup_menu_t *menu, int page)
{
    if (LT_IsLevelsPage(page))
    {
        LevelsDraw(menu, page);
    }
    else
    {
        SummaryDraw();
    }
}

// Scrolling
//===============================================

static void UpdateScrollingIndicators(int rows)
{
    scroll_indicators = (rows - LT_MAX_ROWS - scroll_pos - 1) > 0
                            ? (scroll_indicators | scroll_down)
                            : (scroll_indicators & ~scroll_down);
    scroll_indicators = scroll_pos ? (scroll_indicators | scroll_up)
                                   : (scroll_indicators & ~scroll_up);
}

boolean LT_KeyboardScroll(setup_menu_t *menu, setup_menu_t *item)
{
    if (!set_lvltbl_active)
    {
        return false;
    }

    int current_row = 0, rows = 0;
    for (setup_menu_t *src = menu; !(src->m_flags & S_END); ++src)
    {
        if (src == item)
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

    if (rows > LT_MAX_ROWS)
    {
        while (current_row - scroll_pos > LT_MAX_ROWS - buffer_i)
        {
            ++scroll_pos;
        }
    }

    while (scroll_pos && current_row - scroll_pos < top_buffer_i)
    {
        --scroll_pos;
    }

    // Hack for the reset button to act like the first item in the menu
    if (item->m_flags & S_RESET)
    {
        scroll_pos = 0;
    }

    UpdateScrollingIndicators(rows);

    return true;
}

boolean LT_MouseScroll(setup_menu_t *menu, int inc)
{
    if (!set_lvltbl_active || menu_input != mouse_mode)
    {
        return false;
    }

    int rows = 0;
    for (setup_menu_t *src = menu; !(src->m_flags & S_END); ++src)
    {
        if (!(src->m_flags & S_DIRECT))
        {
            ++rows;
        }
    }

    scroll_pos = MAX(0, MIN(rows - LT_MAX_ROWS - 1, scroll_pos + inc));
    UpdateScrollingIndicators(rows);
    return true;
}

void LT_UpdateScrollingIndicators(setup_menu_t *menu)
{
    if (!set_lvltbl_active)
    {
        return;
    }

    int rows = 0;
    for (setup_menu_t *src = menu; !(src->m_flags & S_END); ++src)
    {
        if (!(src->m_flags & S_DIRECT))
        {
            ++rows;
        }
    }

    UpdateScrollingIndicators(rows);
}
