#include <stdio.h>

#include "mem.h"
#include "gfx.h"
#include "input.h"
#include "mem_card.h"
#include "audio.h"
#include "res.h"
#include "sokoban.h"
#include "menu.h"
#include "game.h"
#include "scene.h"
#include "scene_init.h"
#include "scene_title.h"
#include "scene_level.h"
#include "scene_levels.h"
#include "scene_credits.h"
#include "scene_all_cleared.h"

Menu *game_options_menu;
static uint8_t *game_options[] =
{
    "MUSIC VOLUME'<5>",
    "  SFX VOLUME'<5>"
};
static uint8_t *game_options_header = "";
static uint8_t *game_options_footer = "SELECT>BACK";

static void game_option_selected(uint32_t selected_item)
{
    printf("game option selected %d ! \n", selected_item);
    switch (selected_item)
    {
        case 0:
            break;

        case 1:
            break;
    }
}

static void game_option_fixed_update()
{
    bool changed = false;
    bool error = false;
    switch (game_options_menu->selected_option_index)
    {
        // music volume
        case 0:
            if (input_is_action_just_pressed(ACTION_RIGHT))
            {
                changed = true;
                audio_music_volume++;
                if (audio_music_volume > 9) 
                {
                    audio_music_volume = 9;
                    error = true;
                }
            }
            if (input_is_action_just_pressed(ACTION_LEFT))
            {
                changed = true;
                audio_music_volume--;
                if (audio_music_volume < 0) 
                {
                    audio_music_volume = 0;
                    error = true;
                }
            }
            break;

        // sfx volume
        case 1:
            if (input_is_action_just_pressed(ACTION_RIGHT))
            {
                changed = true;
                audio_sfx_volume++;
                if (audio_sfx_volume > 9)
                {
                    audio_sfx_volume = 9;
                    audio_play_sound(SOUND_ID_ERROR);
                    error = true;
                }
            }
            if (input_is_action_just_pressed(ACTION_LEFT))
            {
                changed = true;
                audio_sfx_volume--;
                if (audio_sfx_volume < 0)
                {
                    audio_sfx_volume = 0;
                    audio_play_sound(SOUND_ID_ERROR);
                    error = true;
                }
            }
            break;
    } 

    if (changed && error)
    {
        audio_play_sound(SOUND_ID_ERROR);
    }
    else if (changed)
    {
        audio_set_music_volume(audio_music_volume);
        audio_set_sfx_volume(audio_sfx_volume);
        audio_play_sound(SOUND_ID_DROP);
    }

    game_options[0][14] = audio_music_volume + '0';
    game_options[1][14] = audio_sfx_volume + '0';
}

void game_init()
{
    game_options_menu = menu_create(44, 150, 21, 6, game_options_header, game_options_footer, game_option_selected, game_option_fixed_update, 2, game_options);
}

int32_t game_last_cleared_level = 0;
int32_t game_current_level = 0;

void game_start_level()
{
    game_current_level = game_last_cleared_level + 1;
    if (game_current_level > GAME_LAST_LEVEL)
    {
        game_current_level = GAME_LAST_LEVEL;
    }
}

static void init_all()
{
    mem_init();
    res_init();
    gfx_init();
    input_init();
    audio_init();
    mem_card_init();
    game_init();
}

static void load_all_resources()
{
    GFX_TIM_Info *title = res_load_tim_image(GAME_RES_TITLE_IMAGE);
    gfx_add_tileset(0, title, 1, 1);

    GFX_TIM_Info *tileset = res_load_tim_image(GAME_RES_TILESET_IMAGE);
    gfx_add_tileset(1, tileset, 13, 8);

    GFX_TIM_Info *font = res_load_tim_image(GAME_RES_FONT_IMAGE);
    uint8_t *charset = GAME_FONT_CHARSET;
    
    gfx_set_font(font, 12, 14, 8, 8, charset);
    gfx_set_font_box_border_chars(GAME_FONT_BOX_BORDER_CHARS);
}

static void render_common_background(Scene *scene)
{
    static uint32_t frame_count = 0;
    frame_count++;
    gfx_use_tileset(1);    
    gfx_set_offsets(0, 0);
    int32_t tile_offset_x = csin(frame_count << 3) >> 8;
    int32_t tile_offset_y = (frame_count >> 1) % 16;
    for (int32_t row = -1; row < 17; row++)
    {
        for (int32_t col = -1; col < 17; col++)
        {
            gfx_draw_tile(89, col * 16 + tile_offset_x, row * 16 + tile_offset_y);
        }
    }
}

static void init_all_scenes()
{
    Scene *scene_init = scene_init_create();
    scene_init->render_background = render_common_background;
    scene_add(SCENE_ID_INIT, scene_init);

    Scene *scene_title = scene_title_create();
    scene_title->render_background = render_common_background;
    scene_add(SCENE_ID_TITLE, scene_title);

    Scene *scene_level = scene_level_create();
    scene_level->render_background = render_common_background;
    scene_add(SCENE_ID_LEVEL, scene_level);

    Scene *scene_levels = scene_levels_create();
    scene_levels->render_background = render_common_background;
    scene_add(SCENE_ID_LEVELS, scene_levels);

    Scene *scene_credits = scene_credits_create();
    scene_credits->render_background = render_common_background;
    scene_add(SCENE_ID_CREDITS, scene_credits);

    Scene *scene_all_cleared = scene_all_cleared_create();
    scene_all_cleared->render_background = render_common_background;
    scene_add(SCENE_ID_ALL_CLEARED, scene_all_cleared);

    // init all scenes
    scene_init_all();
}

void game_entry_point()
{
    init_all();
    load_all_resources();
    init_all_scenes();

    // start game with title scene
    scene_switch_to(SCENE_ID_INIT);

    // game loop
    while (true)
    {
        input_fixed_update();
        scene_fixed_update();
        scene_render();
        gfx_swap_buffers();
    }
}
