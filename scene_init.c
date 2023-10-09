#include <stdio.h>
#include <memory.h>
#include <string.h>

#include "scene_init.h"
#include "gfx.h"
#include "audio.h"
#include "input.h"
#include "mem_card.h"
#include "menu.h"
#include "game.h"

static uint32_t title_time;
static uint32_t init_state;

static bool is_yes_no_menu_activated;
static Menu *yes_no_menu;
static uint8_t *yes_no_menu_options[] =
{
    "YES",
    "NO"
};
static uint8_t *yes_no_header = "";
static uint8_t *yes_no_footer = "";

static bool is_which_mc_menu_activated;
static Menu *which_mc_menu;
static uint8_t *which_mc_menu_options[] =
{
    "MEMORY CARD 1",
    "MEMORY CARD 2"
};
static uint8_t *which_mc_header = "";
static uint8_t *which_mc_footer = "";

static bool mem_card_file_ok;

Scene* scene_init_create(void)
{
    Scene *scene_init = scene_create();

    scene_init->init = scene_init_init;
    scene_init->on_enter = scene_init_on_enter;
    scene_init->on_exit = scene_init_on_exit;
    scene_init->on_transition_finished = scene_init_on_transition_finished;
    scene_init->fixed_update = scene_init_fixed_update;
    scene_init->render_background = NULL;
    scene_init->render = scene_init_render;

    return scene_init;
}

static void yes_no_menu_option_selected(uint32_t selected_item)
{
    //printf("yes/no menu option selected %d ! \n", selected_item);
    switch (selected_item)
    {
        case 0:
        {
            //printf("yes\n");
            menu_push(which_mc_menu);
            which_mc_menu->selected_option_index = 0;    
            is_yes_no_menu_activated = false;
            is_which_mc_menu_activated = true;
            break;
        }

        case 1:
        {
            scene_switch_to(SCENE_ID_TITLE);
            break;
        }
    }

    audio_play_sound(SOUND_ID_SELECT);
}

static bool create_mem_card_entry(uint32_t port)
{
    mem_card_create_file(port, GAME_MEM_CARD_FILE_NAME, 1);
    return game_save_mem_card_entry(port, 0);
}

static void which_mc_menu_option_selected(uint32_t selected_item)
{
    //printf("which mc menu option selected %d ! \n", selected_item);
    audio_play_sound(SOUND_ID_SELECT);

    switch (selected_item)
    {
        case 0:
        {
            create_mem_card_entry(MEM_CARD_PORT_1);
            break;
        }

        case 1:
        {
            create_mem_card_entry(MEM_CARD_PORT_2);
            break;
        }
    }

    init_state = 0;
    is_yes_no_menu_activated = false;
    is_which_mc_menu_activated = false;
    mem_card_file_ok = false;
}

void scene_init_init(Scene *scene)
{
    yes_no_menu = menu_create(100, 128, 7, 6, yes_no_header, yes_no_footer, yes_no_menu_option_selected, NULL, 2, yes_no_menu_options);
    which_mc_menu = menu_create(60, 128, 17, 6, which_mc_header, which_mc_footer, which_mc_menu_option_selected, NULL, 2, which_mc_menu_options);
}

void scene_init_on_enter(Scene *scene)
{
    menu_push(yes_no_menu);
    yes_no_menu->selected_option_index = 0;    
    is_yes_no_menu_activated = false;

    which_mc_menu->selected_option_index = 0;    
    is_which_mc_menu_activated = false;

    init_state = 0;
    mem_card_file_ok = false;

    audio_play_music(MUSIC_ID_ALL_LEVELS_CLEARED);
    audio_pause_music();
}

void scene_init_on_exit(Scene *scene)
{
    
}

void scene_init_on_transition_finished(Scene *scene)
{
    
}

static bool check_mem_card(uint32_t port)
{
    if (mem_card_check(port))
    {
        if (mem_card_check_file_exists(port, GAME_MEM_CARD_FILE_NAME))
        {
            uint8_t data_buffer[GAME_MEM_CARD_FILE_SIZE];
            if (mem_card_read_file(port, GAME_MEM_CARD_FILE_NAME, data_buffer, GAME_MEM_CARD_FILE_SIZE))
            {
                int32_t last_cleared_level = *(uint32_t*) (data_buffer + sizeof(Memory_Card_Entry_Header));
                if (last_cleared_level >= 0 && last_cleared_level <= GAME_LAST_LEVEL)
                {
                    mem_card_file_ok = true;
                    game_last_cleared_level = last_cleared_level;
                    return true;
                }
            }
        }
    }
    return false;
}

void scene_init_fixed_update(Scene *scene)
{
    switch (init_state)
    {
        case 0:
            is_yes_no_menu_activated = false;
            is_which_mc_menu_activated = false;

            init_state = 1;
            if (check_mem_card(MEM_CARD_PORT_1))
            {
                title_time = scene_frame_count + 180;
                init_state = 3;
                audio_play_sound(SOUND_ID_START);
            }
            break;

        case 1:
            init_state = 2;
            if (check_mem_card(MEM_CARD_PORT_2))
            {
                title_time = scene_frame_count + 180;
                init_state = 3;
                audio_play_sound(SOUND_ID_START);
            }
            else
            {
                is_yes_no_menu_activated = true;
                audio_play_sound(SOUND_ID_ERROR);
            }
            break;

        // mem card file not found or not connected 
        // ask player to create game entry
        case 2:
            // select button cancel
            if (menu_current == which_mc_menu && input_is_action_just_pressed(ACTION_SELECT))
            {
                audio_play_sound(SOUND_ID_DROP);
                menu_pop();
                yes_no_menu->selected_option_index = 0;    
                is_which_mc_menu_activated = false;
                is_yes_no_menu_activated = true;
            }
            else
            {
                menu_fixed_update();
            }
            break;

        // mem card file loaded and ok
        case 3:
            if (scene_frame_count >= title_time)
            {
                scene_switch_to(SCENE_ID_TITLE);
            }
            break;
    }
}

void scene_init_render(Scene *scene)
{
    if (init_state < 2)
    {
        gfx_draw_font_box(36, 92, 23, 5, 64);
        gfx_use_font();
        gfx_draw_text("CHECKING MEMORY CARD", 48, 108); 
    }
    else if (init_state == 2)
    {
        gfx_draw_font_box(36, 76, 23, 6, 64);
        gfx_use_font();
        gfx_draw_text("GAME FILE NOT FOUND!", 48, 92); 
        gfx_draw_text("CREATE ONE?", 48, 100); 
        menu_render();
    }
    else if (init_state == 3)
    {
        gfx_draw_font_box(36, 92, 23, 5, 64);
        gfx_use_font();
        gfx_draw_text("MEMORY CARD FILE OK", 52, 108); 
    }

    if (is_yes_no_menu_activated)
    {
        gfx_use_font();
        gfx_draw_text("                      START>OK", 8, 216);
    }
    else if (is_which_mc_menu_activated)
    {
        gfx_use_font();
        gfx_draw_text("SELECT>BACK           START>OK", 8, 216);
    }
}
