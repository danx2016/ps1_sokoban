#include <stdbool.h>
#include <stdio.h>
#include <libetc.h>

#include "res.h"
#include "gfx.h"
#include "audio.h"
#include "input.h"
#include "mem_card.h"
#include "sokoban.h"
#include "menu.h"
#include "scene_level.h"
#include "game.h"

static uint32_t frames_count;
static bool is_scene_activated;

static uint32_t grid_offset_x;
static uint32_t grid_offset_y;

static uint32_t player_anim_frames[4] = { 0, 1, 0, 2 };
static uint32_t player_anim_frame = 0;
static uint32_t player_anim_offset = 0;

static bool is_player_moving = false;
static uint32_t player_moving_rate = 0;
static uint32_t player_moves_count = 0;
static Sokoban_Move player_move;

static bool is_level_cleared;
static int32_t next_level_time;

static bool is_level_menu_activated;
static Menu *level_menu;
static uint8_t *level_menu_options[] =
{
    "RESUME",
    "RESTART",
    "OPTIONS",
    "QUIT"
};

static uint8_t *level_menu_header = "   -MENU-";
static uint8_t *level_menu_footer = "";

static uint8_t hud_level[32];
static uint8_t hud_moves[32];

static bool debug_level_cleared;

Scene* scene_level_create(void)
{
    Scene *scene_level = scene_create();

    scene_level->init = scene_level_init;
    scene_level->on_enter = scene_level_on_enter;
    scene_level->on_exit = scene_level_on_exit;
    scene_level->on_transition_finished = scene_level_on_transition_finished;
    scene_level->fixed_update = scene_level_fixed_update;
    scene_level->render_background = NULL;
    scene_level->render = scene_level_render;

    return scene_level;
}

static void level_menu_option_selected(uint32_t selected_item)
{
    //printf("level menu option selected %d ! \n", selected_item);
    switch (selected_item)
    {
        case 0:
        {
            audio_play_sound(SOUND_ID_MOVE);
            is_level_menu_activated = false;
            break;
        }

        case 1:
        {
            audio_play_sound(SOUND_ID_START);
            scene_switch_to(SCENE_ID_LEVEL);
            break;
        }

        case 2:
        {
            game_options_menu->x = 44;
            game_options_menu->y = 88;
            menu_push(game_options_menu);
            audio_play_sound(SOUND_ID_DROP);
            break;
        }

        case 3:
        {
            audio_play_sound(SOUND_ID_START);
            scene_switch_to(SCENE_ID_TITLE);
            break;
        }
    }
}

void scene_level_init(void)
{
    level_menu = menu_create(80, 88, 12, 9, level_menu_header, level_menu_footer, level_menu_option_selected, NULL, 4, level_menu_options);
}

void scene_level_on_enter(void)
{
    menu_clear();
    menu_push(level_menu);
    level_menu->selected_option_index = 0;    
    is_level_menu_activated = false;

    frames_count = 0;
    is_scene_activated = false;    

    uint8_t level_filename[64];
    sprintf(level_filename, GAME_RES_LEVEL, game_current_level);
    
    Sokoban_Level *level = res_load_sokoban_level(level_filename);
    sokoban_start(level);

    grid_offset_x = (256 - level->cols * 16) / 2;
    grid_offset_y = (256 - level->rows * 16) / 2; 

    player_anim_frame = 0;
    player_anim_offset = 0;
    is_player_moving = false;
    player_moving_rate = 0;
    player_moves_count = 0;

    is_level_cleared = false;

    debug_level_cleared = false;

    audio_play_music(MUSIC_ID_PLAYING, true);
}

void scene_level_on_exit(void)
{
    if (is_level_cleared)
    {
        if (game_current_level > game_last_cleared_level)
        {
            game_last_cleared_level = game_current_level;

            // save to memory card the last cleared level
            bool mem_card_1_ok = mem_card_check(MEM_CARD_PORT_1);
            if (mem_card_1_ok)
            {
                mem_card_1_ok = game_save_mem_card_entry(MEM_CARD_PORT_1, game_last_cleared_level);
            }

            if (!mem_card_1_ok) {
                if (mem_card_check(MEM_CARD_PORT_2))
                {
                    game_save_mem_card_entry(MEM_CARD_PORT_2, game_last_cleared_level);
                } 
            }
        }

        game_current_level++;
    }
}

void scene_level_on_transition_finished(void)
{
    //printf("transition finished !\n");
    is_scene_activated = true;
}

static void handle_input_direction(Sokoban_Direction direction, uint32_t anim_offset)
{
    player_anim_offset = anim_offset;
    if (sokoban_move_start(&player_move, direction))
    {
        is_player_moving = true;
        player_moving_rate = 0;
        
        // update moves counter
        player_moves_count++;
        if (player_moves_count > 999999)
        {
            player_moves_count = 999999;
        }
    }
    else
    {
        audio_play_sound(SOUND_ID_ERROR);
    }
}

static void fixed_update_input()
{
    if (!is_player_moving && input_is_action_just_pressed(ACTION_LEFT))
    {
        handle_input_direction(LEFT, 29);
    }
    else if (!is_player_moving && input_is_action_just_pressed(ACTION_RIGHT))
    {
        handle_input_direction(RIGHT, 26);
    }
    else if (!is_player_moving && input_is_action_just_pressed(ACTION_UP))
    {
        handle_input_direction(UP, 3);
    }
    else if (!is_player_moving && input_is_action_just_pressed(ACTION_DOWN))
    {
        handle_input_direction(DOWN, 0);
    }
    else if (input_is_action_just_pressed(ACTION_START))
    {
        is_level_menu_activated = true;
        level_menu->selected_option_index = 0;
        audio_play_sound(SOUND_ID_DROP);
    }

    // for debugging purposes
    //else if (input_is_action_just_pressed(ACTION_SELECT))
    //{
    //    debug_level_cleared = true;
    //    is_player_moving = true;
    //    player_moving_rate = 16;
    //}
}

void scene_level_fixed_update(void)
{
    if (!is_scene_activated)
    {
        return;
    }

    // options menu
    if (is_level_menu_activated)
    {
        if (input_is_action_just_pressed(ACTION_SELECT) && menu_current == level_menu)
        {
            is_level_menu_activated = false;
            audio_play_sound(SOUND_ID_MOVE);
        }
        else
        {
            menu_fixed_update();
        }
    }
    // user input
    else if (!is_level_cleared)
    {
        fixed_update_input();
    }

    // player is moving
    if (is_player_moving)
    {
        player_moving_rate++;
        if (player_moving_rate >= 16)
        {
            is_player_moving = false;
            sokoban_move_commit(&player_move);

            // if box is pushed to target, play sound
            if (player_move.has_box)
            {
                uint32_t tile_id = sokoban->grid[player_move.box_row * sokoban->cols + player_move.box_col];
                if ((tile_id & SOKOBAN_TILE_ID_BOX) && (tile_id & SOKOBAN_TILE_ID_TARGET))
                {
                    audio_play_sound(SOUND_ID_SELECT);
                }
            }

            if (debug_level_cleared || sokoban_is_level_cleared())
            {
                // printf("level cleared :) !\n");
                is_level_cleared = true;
                next_level_time = 0;
                player_anim_offset = 0;
                VSync(16);
                audio_play_music(MUSIC_ID_LEVEL_CLEARED, false);
            }
        }
    }    

    // current level cleared, go to next
    next_level_time++;
    if (is_level_cleared && next_level_time > 360)
    {
        if (game_current_level == GAME_LAST_LEVEL)
        {
            scene_switch_to(SCENE_ID_ALL_CLEARED);
        }
        else
        {
            scene_switch_to(SCENE_ID_LEVEL);
        }
    }

    // update player animation frame
    player_anim_frame++;

    frames_count++;
}

void scene_level_render(void)
{
    gfx_use_tileset(1);
    gfx_set_offsets(grid_offset_x, grid_offset_y);

    size_t player_anim_tile_id = player_anim_offset + 52;
    player_anim_tile_id += player_anim_frames[((player_anim_frame >> 3) % 4)];

    // render player and box while moving    
    if (is_player_moving)
    {
        int32_t dx = (player_move.direction & 3) - 2;
        int32_t dy = ((player_move.direction >> 2) & 3) - 2;

        // draw moving player
        uint32_t px = player_move.player_col * 16 + dx * player_moving_rate;
        uint32_t py = player_move.player_row * 16 + dy * player_moving_rate;
        gfx_draw_tile(player_anim_tile_id, px, py);

        // draw moving box
        if (player_move.has_box)
        {
            uint32_t bx = player_move.box_col * 16 + dx * player_moving_rate;
            uint32_t by = player_move.box_row * 16 + dy * player_moving_rate;
            gfx_draw_tile(6, bx, by);
        }
    }

    // render game grid
    size_t tile_index = 0;
    for (int row = 0; row < sokoban->rows; row++)
    {
        for (int col = 0; col < sokoban->cols; col++)
        {
            uint8_t tile_id = sokoban->grid[tile_index++];
            if ((tile_id & SOKOBAN_TILE_ID_PLAYER) == SOKOBAN_TILE_ID_PLAYER)
            {
                gfx_draw_tile(player_anim_tile_id, col * 16, row * 16); // player
            }

            if ((tile_id & SOKOBAN_TILE_ID_TARGET_OR_BOX) == SOKOBAN_TILE_ID_TARGET_OR_BOX)
            {
                gfx_draw_tile(8, col * 16, row * 16); // blue box
            }
            else if ((tile_id & SOKOBAN_TILE_ID_BOX) == SOKOBAN_TILE_ID_BOX)
            {
                gfx_draw_tile(6, col * 16, row * 16); // brown box
            }
            else if ((tile_id & SOKOBAN_TILE_ID_TARGET) == SOKOBAN_TILE_ID_TARGET)
            {
                // blink target green : target gray
                gfx_draw_tile((player_anim_frame >> 3) & 1 ? 51 : 77, col * 16, row * 16);
            }

            if ((tile_id & SOKOBAN_TILE_ID_WALL) == SOKOBAN_TILE_ID_WALL)
            {
                gfx_draw_tile(85, col * 16, row * 16); // wall
            }
            else if ((tile_id & SOKOBAN_TILE_ID_IGNORE) != SOKOBAN_TILE_ID_IGNORE)
            {
                gfx_draw_tile(88, col * 16, row * 16); // floor
            }
        }
    } 

    // hud
    gfx_use_font();
    sprintf(hud_level, "LEVEL'%03d", game_current_level);
    sprintf(hud_moves, "MOVES'%06d", player_moves_count);
    gfx_draw_text(hud_level, 8, 24); 
    gfx_draw_text(hud_moves, 152, 24); 

    if (!is_level_menu_activated)
    {
        if (!is_level_cleared)
        {
            gfx_draw_text("START>OPEN MENU", 128, 224); 
        }
    }
    // options menu
    else
    {
        gfx_fill_rect(0, 0, 256, 256, 32, 32, 32, 1);
        menu_render();
    }

    if (is_level_cleared)
    {
        gfx_draw_font_box(36, 92, 23, 8, 64);
        gfx_use_font();
        gfx_draw_text("CONGRATULATIONS =}", 56, 108); 
        gfx_draw_text("LEVEL CLEARED!", 72, 132); 
    }
}
