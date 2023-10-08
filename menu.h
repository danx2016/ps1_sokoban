#ifndef _MENU_H
#define _MENU_H

#include <stddef.h>
#include <stdint.h>

#define MENU_DISABLED_CHAR '+'
#define MENU_MAX_NUM 4

typedef struct 
{
    int32_t x;
    int32_t y;
    int32_t font_x;
    int32_t font_y;
    uint32_t cols;
    uint32_t rows;
    uint32_t visible_rows;
    uint32_t visible_top_row;
    int32_t selected_option_index;
    uint8_t *header;
    uint8_t *footer;
    void (*on_menu_option_selected)(uint32_t selected_item);
    void (*fixed_update)(void);
    size_t num_options;
    int8_t *options[];
} Menu;

extern Menu *menu_current;

extern Menu *menu_create(int32_t x, int32_t y, uint32_t cols, uint32_t rows, uint8_t *header, uint8_t *footer, void (*on_option_selected)(uint32_t selected_item), void (*fixed_update)(void), size_t num_options, uint8_t *menu_options[]);

extern void menu_clear();
extern void menu_push(Menu *menu);
extern void menu_pop();

extern void menu_fixed_update(void);
extern void menu_render(void);

#endif /* _MENU_H */