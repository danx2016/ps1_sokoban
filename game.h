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

#define GAME_MUSIC_TITLE "\\ASSETS\\MUSICS\\FREEOST1.HIT;1"
#define GAME_MUSIC_PLAYING "\\ASSETS\\MUSICS\\FREEOST2.HIT;1"   
#define GAME_MUSIC_VICTORY "\\ASSETS\\MUSICS\\RAD_VICT.HIT;1"   
#define GAME_MUSIC_ALL_CLEARED "\\ASSETS\\MUSICS\\PEPPY.HIT;1"   

extern Menu *game_options_menu;

extern void game_init(void);

extern int32_t game_last_cleared_level;
extern int32_t game_current_level;

extern void game_start_level(void);

extern void game_entry_point(void);

#endif /* _GAME_H */