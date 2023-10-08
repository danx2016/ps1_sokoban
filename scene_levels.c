#include <stdbool.h>
#include <stdio.h>

#include "mem.h"
#include "menu.h"
#include "gfx.h"
#include "audio.h"
#include "input.h"
#include "game.h"
#include "scene_levels.h"

static bool is_activated;

static Menu *levels_menu;
static uint8_t *levels_menu_options[GAME_LAST_LEVEL];
static uint8_t *levels_menu_header = "";
static uint8_t *levels_menu_footer = "";

Scene* scene_levels_create(void)
{
    Scene *scene_levels = scene_create();

    scene_levels->init = scene_levels_init;
    scene_levels->on_enter = scene_levels_on_enter;
    scene_levels->on_exit = scene_levels_on_exit;
    scene_levels->on_transition_finished = scene_levels_on_transition_finished;
    scene_levels->fixed_update = scene_levels_fixed_update;
    scene_levels->render_background = NULL;
    scene_levels->render = scene_levels_render;

    return scene_levels;
}

static void game_option_selected(uint32_t selected_item)
{
    printf("menu option selected %d ! \n", selected_item);
    if (selected_item <= game_last_cleared_level)
    {
        audio_play_sound(SOUND_ID_START);
        game_current_level = selected_item + 1;
        scene_switch_to(SCENE_ID_LEVEL);
    }
    else
    {
        audio_play_sound(SOUND_ID_ERROR);
    }
    
}

void scene_levels_init(Scene *scene)
{
    // "  LEVEL 000"
    for (int i = 0; i < GAME_LAST_LEVEL; i++)
    {
        levels_menu_options[i] = mem_alloc(11);
        //sprintf(levels_menu_options[i], "[ LEVEL %03d", i + 1);
    }
    levels_menu = menu_create(60, 48, 15, 20, levels_menu_header, levels_menu_footer, game_option_selected, NULL, GAME_LAST_LEVEL, levels_menu_options);
    levels_menu->visible_rows = 10;
}

void scene_levels_on_enter(Scene *scene)
{
    is_activated = false;

    for (int i = 0; i < GAME_LAST_LEVEL; i++)
    {
        if (i <= game_last_cleared_level)
        {
            sprintf(levels_menu_options[i], "] LEVEL %03d", i + 1);
        }
        else
        {
            sprintf(levels_menu_options[i], "[+LEVEL %03d", i + 1);
        }
    }

    menu_clear();
    menu_push(levels_menu);
    levels_menu->selected_option_index = 0;
    levels_menu->visible_top_row = 0;
}

void scene_levels_on_exit(Scene *scene)
{
}

void scene_levels_on_transition_finished(Scene *scene)
{
    is_activated = true;
}

void scene_levels_fixed_update(Scene *scene)
{
    if (!is_activated)
    {
        return;
    }

    if (input_is_action_just_pressed(ACTION_SELECT))
    {
        audio_play_sound(SOUND_ID_MOVE);
        scene_switch_to(SCENE_ID_TITLE);
    }

    menu_fixed_update();
}

void scene_levels_render(Scene *scene)
{
    gfx_use_font();
    gfx_set_offsets(0, 0);
    gfx_draw_text("-LEVELS-", 92, 40);
    gfx_draw_text(";", 120, 56);
    gfx_draw_text(":", 120, 192);
    gfx_draw_text("SELECT>BACK           START>OK", 8, 216);

    menu_render();     
}
