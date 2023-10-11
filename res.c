#include <stdio.h>
#include <libapi.h>
#include <libcd.h>

#include "mem.h"
#include "res.h"
#include "sokoban.h"

void res_init(void)
{
    _96_remove();
    while (!CdInit());
}

#define log(msg, arg) printf(msg, arg)
// #define log(msg, arg)

void* res_load(uint8_t* res_file_name)
{
    log("===> loading resource %s .. ", res_file_name);
    
    CdlFILE file;
    uint8_t result;
    if (CdSearchFile(&file, res_file_name))
    {
        uint32_t sectors_count = (file.size + 2047) / 2048;
        void* res_buffer = mem_alloc(sectors_count * 2048);
        CdControl(CdlSetloc, (uint8_t*) &file.pos, &result);
        CdRead(sectors_count, (uint32_t*) res_buffer, CdlModeSpeed);
        CdReadSync(0, &result);
    
        log("OK\n", 0);
    
        return res_buffer; 
    }
    
    log("ERROR!\n", 0);

    return NULL;
}

static void load_tim_image_to_vram(uint8_t *tim_data, TIM_IMAGE *tim_info)
{
    OpenTIM((uint32_t*) tim_data);
    ReadTIM(tim_info);
    LoadImage(tim_info->prect, tim_info->paddr);
    DrawSync(0);
    if (tim_info->mode & 0x8){
        LoadImage(tim_info->crect, tim_info->caddr);
        DrawSync(0);
    }
}

GFX_TIM_Info *res_load_tim_image(uint8_t *filename)
{
    uint8_t *tim_data = res_load(filename);
    TIM_IMAGE tim_info;
    OpenTIM((uint32_t*) tim_data);
    ReadTIM(&tim_info);
    load_tim_image_to_vram(tim_data, &tim_info);
    GFX_TIM_Info *tim_info_copy = (GFX_TIM_Info*) mem_alloc(sizeof(GFX_TIM_Info));
    tim_info_copy->mode = tim_info.mode;
    tim_info_copy->caddr = *tim_info.caddr;
    tim_info_copy->crect = *tim_info.crect;
    tim_info_copy->paddr = *tim_info.paddr;
    tim_info_copy->prect = *tim_info.prect;
    mem_free(tim_data);
    return tim_info_copy;
}

Sokoban_Level *res_load_sokoban_level(uint8_t *filename)
{
    uint8_t *level_data = (uint8_t*) res_load(filename);

    uint8_t cols = 100 * (level_data[0] - '0') + 10 * (level_data[1] - '0') + (level_data[2] - '0');
    uint8_t rows = 100 * (level_data[4] - '0') + 10 * (level_data[5] - '0') + (level_data[6] - '0');
    
    Sokoban_Level *level = mem_alloc(sizeof(Sokoban_Level) + (cols * rows));

    level->cols = cols;
    level->rows = rows;
    
    size_t index = 0;
    uint8_t *next_tile = &level_data[7];
    
    while (index < (cols * rows))
    {
        switch (*next_tile++)
        {
            case ' ':
            {
                level->grid[index++] = SOKOBAN_TILE_ID_EMPTY;
                break;
            }

            case '.':
            {
                level->grid[index++] = SOKOBAN_TILE_ID_TARGET;
                break;
            }

            case '+':
            {
                level->grid[index] = SOKOBAN_TILE_ID_PLAYER | SOKOBAN_TILE_ID_TARGET;
                level->player_col = index % cols;
                level->player_row = index / cols;
                index++;
                break;
            }

            case '$':
            {
                level->grid[index++] = SOKOBAN_TILE_ID_BOX;
                break;
            }

            case '#':
            {
                level->grid[index++] = SOKOBAN_TILE_ID_WALL;
                break;
            }

            case '*':
            {
                level->grid[index++] = SOKOBAN_TILE_ID_BOX | SOKOBAN_TILE_ID_TARGET; 
                break;
            }

            case '@': 
            {
                level->grid[index] = SOKOBAN_TILE_ID_PLAYER; 
                level->player_col = index % cols;
                level->player_row = index / cols;
                index++;
                break;
            }

            case '_': 
            {
                level->grid[index++] = SOKOBAN_TILE_ID_IGNORE;
                break;
            }
        }
    }
    
    // after converting the level, level_data can be freed 
    mem_free(level_data);

    return level;
}
