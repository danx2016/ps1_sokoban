#ifndef _GAME_H
#define _GAME_H

#include <stdint.h>

#include "menu.h"

#define GAME_LAST_LEVEL 12

#define GAME_MEM_CARD_FILE_NAME "BA_DANX2016_SOKOBAN"
#define GAME_MEM_CARD_FILE_SIZE 640

#define GAME_RES_TITLE_IMAGE "\\ASSETS\\IMAGES\\TITLE.TIM;1"
#define GAME_RES_TILESET_IMAGE "\\ASSETS\\IMAGES\\SOKOBAN.TIM;1"
#define GAME_RES_FONT_IMAGE "\\ASSETS\\IMAGES\\FONT8X8.TIM;1"
#define GAME_FONT_CHARSET " 1234567890-`!``````````={}``|\\'`\"`<,>.?/ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz``````````@`````````````````````````#$%`````````^&*`````````()_```````+:;[]   "
#define GAME_FONT_BOX_BORDER_CHARS "#$%^&*()_"

extern Menu *game_options_menu;

extern void game_init();


extern int32_t game_last_cleared_level;
extern int32_t game_current_level;

extern void game_start_level();

extern void game_entry_point();

#endif /* _GAME_H */