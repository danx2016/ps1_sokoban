#include <stdbool.h>
#include <rand.h>
#include <string.h>
#include <stdio.h>
#include <libetc.h>

#include "gfx.h"
#include "audio.h"
#include "input.h"
#include "scene_title.h"
#include "menu.h"
#include "game.h"

static uint32_t frames_count;
static bool is_scene_activated;

static uint32_t title_state = 0;

static const uint32_t player_anim_frames[4] = { 0, 1, 0, 2 };
static uint32_t player_anim_frame;
static uint32_t player_anim_offset;
static uint32_t player_tile_id;
static int32_t player_x;
static int32_t player_y;
static bool is_player_pushing_box;
static uint32_t player_state;
static uint32_t box_tile_id;

static Menu *levels_menu;
static uint8_t *title_menu_options[] =
{
    "CONT.FROM LVL 000",
    "LEVELS",
    "OPTIONS",
    "CREDITS"
};

static uint8_t *levels_menu_header = "";
static uint8_t *levels_menu_footer = "";

Scene* scene_title_create(void)
{
    Scene *scene_title = scene_create();

    scene_title->init = scene_title_init;
    scene_title->on_enter = scene_title_on_enter;
    scene_title->on_exit = scene_title_on_exit;
    scene_title->on_transition_finished = scene_title_on_transition_finished;
    scene_title->fixed_update = scene_title_fixed_update;
    scene_title->render_background = NULL;
    scene_title->render = scene_title_render;

    return scene_title;
}

static void game_option_selected(uint32_t selected_item)
{
    //printf("menu option selected %d ! \n", selected_item);
    switch (selected_item)
    {
        case 0:
        {
            game_start_level();
            audio_play_sound(SOUND_ID_SELECT);

            // if all levels were cleared
            if (game_last_cleared_level >= GAME_LAST_LEVEL)
            {
                scene_switch_to(SCENE_ID_ALL_CLEARED);
            }
            // go to level 
            else 
            {
                scene_switch_to(SCENE_ID_LEVEL);
            }
            break;
        }

        case 1:
        {
            audio_play_sound(SOUND_ID_START);
            scene_switch_to(SCENE_ID_LEVELS);
            break;
        }

        case 2:
        {
            game_options_menu->x = 44;
            game_options_menu->y = 150;
            menu_push(game_options_menu);
            audio_play_sound(SOUND_ID_DROP);
            break;
        }

        case 3:
        {
            audio_play_sound(SOUND_ID_START);
            scene_switch_to(SCENE_ID_CREDITS);
            break;
        }
    }
}

void scene_title_init(void)
{
    levels_menu = menu_create(64, 143, 16, 9, levels_menu_header, levels_menu_footer, game_option_selected, NULL, 4, title_menu_options);
}

void scene_title_on_enter(void)
{
    menu_clear();
    menu_push(levels_menu);
    levels_menu->selected_option_index = 0;

    if (game_last_cleared_level < 1)
    {
        strcpy(title_menu_options[0], "START");
    }
    else if (game_last_cleared_level >= GAME_LAST_LEVEL)
    {
        strcpy(title_menu_options[0], "ALL CLEARED!");
        levels_menu->selected_option_index = 1;
    }
    else
    {
        sprintf(title_menu_options[0], "CONT.LVL %03d", (game_last_cleared_level + 1));
    }

    frames_count = 0;
    is_scene_activated = false;    

    player_anim_frame = 0;
    player_anim_offset = 0;
    player_tile_id = 52;
    player_x = 120;
    player_y = 118;
    is_player_pushing_box = false;
    player_state = 0;

    srand(VSync(-1));
    title_state = 0;

    audio_play_music(MUSIC_ID_TITLE, true);
}

void scene_title_on_exit(void)
{
    // do nothing    
}

void scene_title_on_transition_finished(void)
{
    //printf("transition finished !\n");
    is_scene_activated = true;
}

// small title animation sequence
static void fixed_update_player()
{
    player_anim_frame++;
    
    if (player_anim_frame > 280)
    {
        player_tile_id = player_anim_offset + 52;
        player_tile_id += player_anim_frames[((player_anim_frame >> 3) % 4)];
    }

    switch (player_state)
    {
        case 0:
        {
            if (player_anim_frame > 300)
            {
                player_state = 1;
            }
            break;
        }

        case 1:
        {
            player_y++;
            if (player_y >= 200)
            {
                player_y = 200;
                player_state = 2;
                player_anim_offset = 29;
                box_tile_id = (rand() % 5) + 1;
            }
            break;
        }

        case 2:
        {
            player_x--;
            if (player_x < -64)
            {
                is_player_pushing_box = true;
                player_state = 3;
                player_anim_offset = 26;
                box_tile_id = (rand() % 5) + 1;
            }
            break;
        }

        case 3:
        {
            player_x++;
            if (player_x > 288)
            {
                player_state = 2;
                player_anim_offset = 29;
                box_tile_id = (rand() % 5) + 1;
            }
            break;
        }
    }
}

void scene_title_fixed_update(void)
{
    if (!is_scene_activated)
    {
        return;
    }
    
    switch (title_state)
    {
        case 0:
        {
            if (input_is_action_just_pressed(ACTION_START))
            {
                audio_play_sound(SOUND_ID_DROP);
                title_state = 1;
            }
            break;
        }

        // update title menu
        case 1:
        {
            menu_fixed_update();
            break;
        }
    }

    // update small player animation
    fixed_update_player();

    frames_count++;
}

void scene_title_render(void)
{
    gfx_use_tileset(0);
    gfx_draw_tile(0, 32, 48);

    gfx_use_tileset(1);
    gfx_set_offsets(0, 0);
    gfx_draw_tile(player_tile_id, player_x, player_y);

    if (is_player_pushing_box)
    {
        int32_t box_offset_x = player_state == 2 ? -16 : 16;
        gfx_draw_tile(box_tile_id, player_x + box_offset_x, player_y);
    }

    gfx_use_font();

    switch (title_state)
    {
        case 0:
        {
            if (is_scene_activated && (frames_count % 50) < 30)
            {
                gfx_draw_text("PUSH START BUTTON", 60, 168);    
            }
            break;
        }

        // render menu
        case 1:
        {
            menu_render();    
            break;
        }
    }

    gfx_draw_text("@2023 danx2016", 72, 224);
}
