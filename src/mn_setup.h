//
//  Copyright (C) 1999 by
//  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
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

#ifndef __MN_SETUP__
#define __MN_SETUP__

#include "doomdef.h"
#include "doomtype.h"

typedef struct
{
    short x;
    short y;
    short w;
    short h;
} mrect_t;

typedef enum
{
    MENU_NULL,
    MENU_UP,
    MENU_DOWN,
    MENU_LEFT,
    MENU_RIGHT,
    MENU_BACKSPACE,
    MENU_ENTER,
    MENU_ESCAPE,
    MENU_CLEAR
} menu_action_t;

typedef enum
{
    null_mode,
    mouse_mode,
    pad_mode,
    key_mode
} menu_input_mode_t;

extern menu_input_mode_t menu_input, old_menu_input;
void MN_ResetMouseCursor(void);

extern boolean setup_active;
extern boolean set_lvltbl_active; // [Cherry]
extern short whichSkull; // which skull to draw (he blinks)
extern int saved_screenblocks;

extern boolean default_verify;
extern int64_t warning_about_changes;
extern int print_warning_about_changes;

// /--- [Cherry] For use in mn_level_table.c ----------------------------------

struct setup_menu_s;

typedef struct
{
    const char *text;
    mrect_t rect;
    int flags;
} setup_tab_t;

// Establish the message colors to be used

#define CR_TITLE   CR_GOLD
#define CR_SET     CR_GREEN
#define CR_ITEM    CR_NONE
#define CR_HILITE  CR_NONE // CR_ORANGE
#define CR_SELECT  CR_GRAY
#define CR_ALT_COL CR_GRAY // [Cherry]

#define M_SPC    9
#define M_Y      (29 + M_SPC)
#define M_Y_WARN (SCREENHEIGHT - 15)
#define M_TAB_Y  22

extern char menu_buffer[66];
extern const char *default_skill_strings[];

void MN_BlinkingArrowRight(struct setup_menu_s *s);
void MN_DrawItem(struct setup_menu_s *s, int accum_y);
void MN_DrawMenuStringEx(int64_t flags, int x, int y, int color);

// ---- [Cherry] -------------------------------------------------------------/

void MN_InitDefaults(void);
void MN_UpdateFreeLook(void);
extern const char *gamma_strings[];
void MN_ResetGamma(void);
void MN_DrawDelVerify(void);
void MN_SizeDisplay(int choice);

boolean MN_SetupCursorPostion(int x, int y);
boolean MN_SetupMouseResponder(int x, int y);
boolean MN_SetupResponder(menu_action_t action, int ch);

void MN_SetNextMenuAlt(ss_types type);
boolean MN_PointInsideRect(mrect_t *rect, int x, int y);
void MN_ClearMenus(void);
void MN_Back(void);

// [FG] alternative text for missing menu graphics lumps
void MN_DrawTitle(int x, int y, const char *patch, const char *alttext);
void MN_DrawStringCR(int cx, int cy, byte *cr1, byte *cr2, const char *ch);
int MN_StringWidth(const char *string);
int MN_StringHeight(const char *string);

void MN_General(int choice);
void MN_KeyBindings(int choice);
void MN_Compat(int choice);
void MN_StatusBar(int choice);
void MN_Automap(int choice);
void MN_Weapons(int choice);
void MN_Enemy(int choice);
void MN_LevelTable(int choice);

void MN_DrawGeneral(void);
void MN_DrawKeybnd(void);
void MN_DrawCompat(void);
void MN_DrawStatusHUD(void);
void MN_DrawAutoMap(void);
void MN_DrawWeapons(void);
void MN_DrawEnemy(void);
void MN_DrawLevelTable(void);

extern int resolution_scale;
extern int midi_player_menu;
extern const char *midi_player_string;

/////////////////////////////
//
// The following #defines are for the m_flags field of each item on every
// Setup Screen. They can be OR'ed together where appropriate

#define S_HILITE      0x00000001 // Cursor is sitting on this item
#define S_SELECT      0x00000002 // We're changing this item
#define S_TITLE       0x00000004 // Title item
                      
#define S_CRITEM      0x00000010 // Message color
#define S_RESET       0x00000020 // Reset to Defaults Button
#define S_INPUT       0x00000040 // Composite input
#define S_WEAP        0x00000080 // Weapon #
#define S_NUM         0x00000100 // Numerical item
#define S_SKIP        0x00000200 // Cursor can't land here
#define S_KEEP        0x00000400 // Don't swap key out
#define S_END         0x00000800 // Last item in list (dummy)
#define S_LEVWARN     0x00001000 // killough 8/30/98: Always warn about pending change
#define S_PRGWARN     0x00002000 // killough 10/98: Warn about change until next run
#define S_BADVAL      0x00004000 // killough 10/98: Warn about bad value
#define S_LEFTJUST    0x00008000 // killough 10/98: items which are left-justified
#define S_CREDIT      0x00010000 // killough 10/98: credit
#define S_CHOICE      0x00020000 // [FG] selection of choices
#define S_DISABLE     0x00040000 // Disable item
#define S_COSMETIC    0x00080000 // Don't warn about change, always load from OPTIONS lump
#define S_THERMO      0x00100000 // Thermo bar (default size 8)
#define S_NEXT_LINE   0x00200000 // Two lines menu items
#define S_STRICT      0x00400000 // Disable in strict mode
#define S_BOOM        0x00800000 // Disable if complevel < boom
#define S_VANILLA     0x01000000 // Disable if complevel != vanilla
#define S_ACTION      0x02000000 // Run function call only when change is complete
#define S_THRM_SIZE11 0x04000000 // Thermo bar size 11
#define S_ONOFF       0x08000000 // Alias for S_YESNO
#define S_MBF         0x10000000 // Disable if complevel < mbf
#define S_THRM_SIZE4  0x20000000 // Thermo bar size 4
#define S_PCT         0x40000000 // Show % sign
                              
// [Nugget]                   
#define S_CRITICAL    0x0000000100000000 // Disable during non-casual play
#define S_RES         0x0000000200000000 // Report current resolution
#define S_FUNCTION    0x0000000400000000 // Used only to call a function

// [Cherry]
#define S_LTBL_MAP    0x0000001000000000 // Level table row with a map's info
#define S_ALT_COL     0x0000002000000000 // Alternate color text

// S_SHOWDESC  = the set of items whose description should be displayed
// S_SHOWSET   = the set of items whose setting should be displayed
// S_HASDEFPTR = the set of items whose var field points to default array
// S_DIRECT    = the set of items with direct coordinates

#define S_SHOWDESC                                                       \
    (S_TITLE | S_ONOFF | S_CRITEM | S_RESET | S_INPUT | S_WEAP | S_NUM   \
     | S_CREDIT | S_CHOICE | S_THERMO                                    \
     | S_FUNCTION) // [Nugget]

#define S_SHOWSET \
    (S_ONOFF | S_CRITEM | S_INPUT | S_WEAP | S_NUM | S_CHOICE | S_THERMO)

#define S_HASDEFPTR \
    (S_ONOFF | S_NUM | S_WEAP | S_CRITEM | S_CHOICE | S_THERMO)

#define S_DIRECT (S_RESET | S_END | S_CREDIT)

/////////////////////////////
//
// The setup_group enum is used to show which 'groups' keys fall into so
// that you can bind a key differently in each 'group'.

typedef enum
{
    m_null, // Has no meaning; not applicable
    m_scrn, // A key can not be assigned to more than one action
    m_map,  // in the same group. A key can be assigned to one
            // action in one group, and another action in another.
} setup_group;

/////////////////////////////
//
// phares 4/17/98:
// State definition for each item.
// This is the definition of the structure for each setup item. Not all
// fields are used by all items.
//
// A setup screen is defined by an array of these items specific to
// that screen.
//
// killough 11/98:
//
// Restructured to allow simpler table entries,
// and to Xref with defaults[] array in m_misc.c.
// Moved from m_menu.c to m_menu.h so that m_misc.c can use it.

typedef struct setup_menu_s
{
    char *m_text;        // text to display
                         // [Cherry] removed const
    int64_t m_flags;     // phares 4/17/98: flag bits S_* (defined above)
                         // [Nugget] Made 64-bit
    short m_x;           // screen x position (left is 0)
    short m_y;           // screen y position (top is 0)

    union // killough 11/98: The first field is a union of several types
    {
        void *var;             // generic variable
        char *name;            // name
        struct default_s *def; // default[] table entry
        int map_i;             // [Cherry] wad_stats index
    } var;

    setup_group m_group;  // group
    int input_id;         // composite input
    int strings_id;       // [FG] selection of choices
    void (*action)(void); // killough 10/98: function to call after changing
    mrect_t rect;
} setup_menu_t;

#endif
