#include <stdint.h>

#include "mem.h"
#include "gfx.h"
#include "scene.h"

typedef enum
{
    TRANSITION_FINISHED,
    TRANSITION_FADE_IN,
    TRANSITION_FADE_OUT
} Transition_State;

uint32_t scene_frame_count = 0;

static Scene *scenes[SCENE_MAX_NUM];
static size_t scene_last_index = 0;
static Scene *current_scene = NULL;
static Scene *next_scene = NULL;
static uint32_t transition_state = TRANSITION_FINISHED;

static int16_t fade_value;

Scene *scene_create()
{
    Scene *scene = (Scene*) mem_alloc(sizeof(Scene));
    return scene;
}

void scene_add(size_t scene_id, Scene *scene)
{
    scenes[scene_id] = scene;

    if (scene_id > scene_last_index)
    {
        scene_last_index = scene_id;
    }
}

void scene_init_all()
{
    for (int i = 0; i <= scene_last_index; i++)
    {
        scenes[i]->init();
    }
}

void scene_switch_to(size_t scene_id)
{
    next_scene = scenes[scene_id];
    transition_state = TRANSITION_FADE_IN;
    fade_value = 0;
}

void scene_fixed_update()
{
    scene_frame_count++;
    
    switch (transition_state)
    {
        case TRANSITION_FINISHED:
        {
            if (current_scene != NULL)
            {
                current_scene->fixed_update();
            }
            break;
        }
        
        case TRANSITION_FADE_IN:
        {
            fade_value += 6;
            if (fade_value >= 255)
            {
                fade_value = 255;
                transition_state = TRANSITION_FADE_OUT;
                if (current_scene != NULL)
                {
                    current_scene->on_exit();
                }
                current_scene = next_scene;
                current_scene->on_enter();
                next_scene = NULL;
            }
            break;
        }

        case TRANSITION_FADE_OUT:
        {
            fade_value -= 6;
            if (fade_value <= 0) {
                fade_value = 0;
                transition_state = TRANSITION_FINISHED;
                current_scene->on_transition_finished();
            }
            break;
        }
    }
}

void scene_render()
{
    if (current_scene != NULL)
    {
        if (current_scene->render_background != NULL)
        {
            current_scene->render_background();
        }

        current_scene->render();
        if (transition_state != TRANSITION_FINISHED) {
            gfx_use_font(); // workaaround to be able to use sub blend mode
            uint8_t c = (fade_value > 255) ? 255 : (uint8_t) fade_value;
            gfx_fill_rect(0, 0, 256, 256, c, c, c, 2);
        }
    }
}
