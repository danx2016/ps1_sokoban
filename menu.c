#include <memory.h>
#include <string.h>

#include "mem.h"
#include "gfx.h"
#include "audio.h"
#include "input.h"
#include "menu.h"
#include "scene.h"

static Menu *menus_stack[MENU_MAX_NUM];
static int32_t menus_stack_index = 0;

Menu *menu_current = NULL;

Menu *menu_create(int32_t x, int32_t y, uint32_t cols, uint32_t rows, uint8_t *header, uint8_t *footer, void (*on_option_selected)(uint32_t selected_item), void (*fixed_update)(void), size_t num_options, uint8_t *menu_options[])
{
    Menu *menu = (Menu*) mem_alloc(sizeof(Menu) + num_options * sizeof(uint8_t*));
    menu->x = x;
    menu->y = y;
    menu->cols = cols;
    menu->rows = rows;
    menu->visible_rows = num_options;
    menu->visible_top_row = 0;
    menu->header = header;
    menu->footer = footer;
    menu->selected_option_index = 0;
    menu->on_menu_option_selected = on_option_selected;
    menu->fixed_update = fixed_update;
    menu->num_options = num_options;
    for (uint32_t i = 0; i < num_options; i++)
    {
        menu->options[i] = menu_options[i];
    }
    return menu;
}

void menu_clear(void)
{
    for (uint32_t i = 0; i < MENU_MAX_NUM; i++)
    {
        menus_stack[i] = NULL;
    }
    menu_current = NULL;
    menus_stack_index = 0;
}

void menu_push(Menu *menu)
{
    menus_stack[menus_stack_index++] = menu;    
    menu_current = menu;

    // precalculate font location
    menu->font_x = menu->x + 2 * GFX_BOX_FONT_W;
    int32_t box_total_h = menu->rows * GFX_BOX_FONT_H;
    int32_t font_total_h = menu->visible_rows * (GFX_BOX_FONT_H + 4) - 4;
    menu->font_y = menu->y + (box_total_h - font_total_h) / 2;
}

void menu_pop(void)
{
    menus_stack_index--;
    menus_stack[menus_stack_index] = NULL;
    menu_current = menus_stack[menus_stack_index - 1];
}

void menu_fixed_update(void)
{
    if (menu_current == NULL)
    {
        return;
    }
    
    bool moved = false;

    if (input_is_action_just_pressed(ACTION_UP))
    {
        menu_current->selected_option_index--;
        if (menu_current->selected_option_index < 0)
        {
            menu_current->selected_option_index = menu_current->num_options - 1;
            menu_current->visible_top_row = menu_current->selected_option_index - menu_current->visible_rows + 1;
        }
        if (menu_current->selected_option_index < menu_current->visible_top_row)
        {
            menu_current->visible_top_row = menu_current->selected_option_index;
        }
        moved = true;
    }

    if (input_is_action_just_pressed(ACTION_DOWN))
    {
        menu_current->selected_option_index++;
        if (menu_current->selected_option_index > menu_current->num_options - 1)
        {
            menu_current->selected_option_index = 0;
            menu_current->visible_top_row = 0;
        }
        if (menu_current->selected_option_index > menu_current->visible_top_row + menu_current->visible_rows - 1)
        {
            menu_current->visible_top_row = menu_current->selected_option_index - menu_current->visible_rows + 1;
        }
        moved = true;
    }

    if (moved)
    {
        audio_play_sound(SOUND_ID_DROP);
    }

    if (input_is_action_just_pressed(ACTION_START))
    {
        menu_current->on_menu_option_selected(menu_current->selected_option_index);
    }

    if (input_is_action_just_pressed(ACTION_SELECT))
    {
        if (menus_stack_index > 1)
        {
            audio_play_sound(SOUND_ID_MOVE);
            menu_pop();
        }
    }

    if (menu_current->fixed_update != NULL)
    {
        menu_current->fixed_update();
    }
}

void menu_render(void)
{
    if (menu_current == NULL)
    {
        return;
    }
    
    gfx_draw_font_box(menu_current->x, menu_current->y, menu_current->cols, menu_current->rows, 64);
    
    gfx_use_font();
    
    #define MENU_FONT_LEADING (GFX_BOX_FONT_H + 4)

    uint32_t ox = menu_current->font_x;
    uint32_t oy = menu_current->font_y;
    
    gfx_draw_text(menu_current->header, menu_current->x, menu_current->y - GFX_BOX_FONT_H);

    int32_t last_row = menu_current->visible_top_row + menu_current->visible_rows;
    if (menu_current->num_options < last_row)
    {
        last_row = menu_current->num_options;
    }

    for (int32_t i = menu_current->visible_top_row; i < last_row; i++)
    {
        bool is_disabled = strchr((const int8_t*) menu_current->options[i], MENU_DISABLED_CHAR) != NULL;
        if (is_disabled)
        {
            gfx_draw_text_rgb(menu_current->options[i], ox, oy, 64, 64, 64);
        }
        else
        {
            gfx_draw_text(menu_current->options[i], ox, oy);
        }

        if (i == menu_current->selected_option_index && (scene_frame_count % 30) < 20)
        {
            gfx_draw_text(">", menu_current->x + 8, oy);            
            gfx_draw_text("<", menu_current->x + (menu_current->cols - 2) * 8, oy);            
        }

        oy += GFX_BOX_FONT_H + 4;
    }

    // draw footer
    oy += GFX_BOX_FONT_H + 4;
    gfx_draw_text(menu_current->footer, menu_current->x, oy);            
}
