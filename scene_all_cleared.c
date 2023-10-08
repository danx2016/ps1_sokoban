#include <stdbool.h>

#include "gfx.h"
#include "audio.h"
#include "input.h"
#include "scene_all_cleared.h"

static bool is_scene_activated;

Scene* scene_all_cleared_create(void)
{
    Scene *scene_all_cleared = scene_create();

    scene_all_cleared->init = scene_all_cleared_init;
    scene_all_cleared->on_enter = scene_all_cleared_on_enter;
    scene_all_cleared->on_exit = scene_all_cleared_on_exit;
    scene_all_cleared->on_transition_finished = scene_all_cleared_on_transition_finished;
    scene_all_cleared->fixed_update = scene_all_cleared_fixed_update;
    scene_all_cleared->render_background = NULL;
    scene_all_cleared->render = scene_all_cleared_render;

    return scene_all_cleared;
}

void scene_all_cleared_init(Scene *scene)
{
}

void scene_all_cleared_on_enter(Scene *scene)
{
    is_scene_activated = false;
    audio_play_music(MUSIC_ID_ALL_LEVELS_CLEARED);
}

void scene_all_cleared_on_exit(Scene *scene)
{
}

void scene_all_cleared_on_transition_finished(Scene *scene)
{
    is_scene_activated = true;
}

void scene_all_cleared_fixed_update(Scene *scene)
{
    if (!is_scene_activated)
    {
        return;
    }

    if (input_is_action_just_pressed(ACTION_SELECT))
    {
        audio_play_sound(SOUND_ID_MOVE);
        scene_switch_to(SCENE_ID_TITLE);
    }
}

void scene_all_cleared_render(Scene *scene)
{
    gfx_use_tileset(1);
    gfx_draw_tile(72, 120, 64);

    gfx_draw_font_box(36, 76, 23, 11, 64);
    gfx_use_font();
    gfx_draw_text("ALL AVAILABLE LEVELS", 48, 96); 
    gfx_draw_text(" HAVE BEEN CLEARED!", 48, 112); 
    gfx_draw_text(" CONGRATULATIONS :}", 48, 136);     

    gfx_draw_text("SELECT>TITLE", 124, 164);
}
