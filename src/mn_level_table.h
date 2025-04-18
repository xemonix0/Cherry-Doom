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

#ifndef __MN_LEVEL_TABLE__
#define __MN_LEVEL_TABLE__

#include "doomtype.h"
#include "mn_internal.h"

enum
{
    lt_page_stats,
    lt_page_times,
    lt_page_summary,

    lt_page_max,
};

extern setup_tab_t level_table_tabs[];
extern setup_menu_t *level_table[lt_page_max];

boolean LT_IsLevelsPage(int page);

void LT_RecalculateSummary(void);
void LT_Reset(void);
void LT_Build(void);
void LT_Draw(setup_menu_t *menu, int page);

boolean LT_HandleKeyboardScroll(setup_menu_t *menu, setup_menu_t *item);
boolean LT_HandleMouseScroll(setup_menu_t *menu, int inc);
void LT_UpdateScrollIndicators(setup_menu_t *menu);

#endif
