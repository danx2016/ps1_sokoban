#include <stdbool.h>
#include <stdio.h>

#include "gfx.h"
#include "audio.h"
#include "input.h"
#include "scene_credits.h"

static bool is_activated;
static int32_t page;

Scene* scene_credits_create(void)
{
    Scene *scene_credits = scene_create();

    scene_credits->init = scene_credits_init;
    scene_credits->on_enter = scene_credits_on_enter;
    scene_credits->on_exit = scene_credits_on_exit;
    scene_credits->on_transition_finished = scene_credits_on_transition_finished;
    scene_credits->fixed_update = scene_credits_fixed_update;
    scene_credits->render_background = NULL;
    scene_credits->render = scene_credits_render;

    return scene_credits;
}

void scene_credits_init(void)
{
    // do nothing
}

void scene_credits_on_enter(void)
{
    is_activated = false;
    page = 0;
}

void scene_credits_on_exit(void)
{
    // do nothing
}

void scene_credits_on_transition_finished(void)
{
    is_activated = true;
}

void scene_credits_fixed_update(void)
{
    if (!is_activated)
    {
        return;
    }

    if (input_is_action_just_pressed(ACTION_UP))
    {
        page--;
        if (page < 0) page = 0;
    }
    else if (input_is_action_just_pressed(ACTION_DOWN))
    {
        page++;
        if (page > 1) page = 1;
    }

    if (input_is_action_just_pressed(ACTION_SELECT))
    {
        audio_play_sound(SOUND_ID_MOVE);
        scene_switch_to(SCENE_ID_TITLE);
    }
}

void scene_credits_render(void)
{
    gfx_draw_font_box(20, 44, 27, 21, 64);
    gfx_use_font();
    gfx_set_offsets(0, 0);

    gfx_draw_text("-CREDITS-", 92, 32); 

    uint8_t page_str[3] = "_/2";
    sprintf(page_str, "%d/2", (page + 1));
    gfx_draw_text(page_str, 208, 32); 

    if (page == 0)
    {
        const uint16_t offset_x = 28;
        uint16_t offset_y = 56;
        gfx_draw_text("PROGRAMMING", offset_x, offset_y); offset_y += 8;
        gfx_draw_text(" by danx2016", offset_x, offset_y); offset_y += 16;

        gfx_draw_text("GRAPHICS", offset_x, offset_y);  offset_y += 8;
        gfx_draw_text(" \"Sokoban {100t Tiles}\"", offset_x, offset_y);  offset_y += 8;
        gfx_draw_text(" by Kenny", offset_x, offset_y);  offset_y += 8;
        gfx_draw_text(" Public Domain / CC0", offset_x, offset_y);  offset_y += 8;
        gfx_draw_text(" opengameart.org/content/", offset_x, offset_y); offset_y += 8;
        gfx_draw_text("    sokoban-100-tiles", offset_x, offset_y); offset_y += 16;

        gfx_draw_text("FONT", offset_x, offset_y); offset_y += 8;
        gfx_draw_text(" \"8x8 Font\"", offset_x, offset_y); offset_y += 8;
        gfx_draw_text(" by Min", offset_x, offset_y); offset_y += 8;
        gfx_draw_text(" Public Domain / CC0", offset_x, offset_y); offset_y += 8;
        gfx_draw_text(" opengameart.org/content/", offset_x, offset_y); offset_y += 8;
        gfx_draw_text("    8x8-font", offset_x, offset_y); offset_y += 8;
    }
    else if (page == 1)
    {
        const uint16_t offset_x = 28;
        uint16_t offset_y = 56;
        gfx_draw_text("LEVELS", offset_x, offset_y); offset_y += 8;
        gfx_draw_text(" \"696\"", offset_x, offset_y); offset_y += 8;
        gfx_draw_text(" by Dries de Clercq", offset_x, offset_y); offset_y += 16;

        gfx_draw_text("MUSICS", offset_x, offset_y); offset_y += 8;
        gfx_draw_text(" \"Fanfare\"", offset_x, offset_y); offset_y += 8;
        gfx_draw_text(" \"Ending Theme\"", offset_x, offset_y); offset_y += 8;
        gfx_draw_text(" \"Radical Victory\"", offset_x, offset_y); offset_y += 8;
        gfx_draw_text(" \"Peppy Pop Song\"", offset_x, offset_y); offset_y += 8;
        gfx_draw_text(" by krumel {crumb}#0723", offset_x, offset_y); offset_y += 8;
        gfx_draw_text(" github.com/DeerTears/", offset_x, offset_y); offset_y += 8;
        gfx_draw_text("    GB-Studio-Community-", offset_x, offset_y); offset_y += 8;
        gfx_draw_text("       Assets", offset_x, offset_y); offset_y += 8;
    }

    gfx_draw_text("SELECT>BACK        ;:NEXT/PREV", 8, 216);
}
