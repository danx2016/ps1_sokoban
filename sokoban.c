#include <memory.h>

#include "mem.h"
#include "sokoban.h"

static Sokoban_Level *sokoban_template = NULL;
Sokoban_Level *sokoban = NULL;

void sokoban_start(Sokoban_Level *_sokoban)
{
    if (sokoban_template != NULL)
    {
        mem_free(sokoban_template);
    }

    if (sokoban != NULL)
    {
        mem_free(sokoban);
    }

    sokoban_template = _sokoban;
    sokoban = mem_alloc(sizeof(Sokoban_Level) + (_sokoban->cols * _sokoban->rows));
    sokoban_reset();
}

void sokoban_reset(void)
{
    size_t size = sizeof(Sokoban_Level) + (sokoban_template->cols * sokoban_template->rows);
    memcpy(sokoban, sokoban_template, size);
}

static uint8_t get_grid_tile_id(size_t col, size_t row)
{
    if (col < 0 || row < 0 || col > sokoban->cols - 1 || row > sokoban->rows - 1)
    {
        return SOKOBAN_TILE_ID_EMPTY;
    }
    
    return sokoban->grid[row * sokoban->cols + col];
}

static void set_grid_tile_id(size_t col, size_t row, uint8_t tile_type, bool set)
{
    uint8_t tile_id = sokoban->grid[row * sokoban->cols + col];
    sokoban->grid[row * sokoban->cols + col] = set ? (tile_id | tile_type) : (tile_id & ~tile_type);
}

static bool is_grid_tile_type_of(uint8_t tile_id, uint32_t tile_type)
{
    if (tile_type == SOKOBAN_TILE_ID_EMPTY && tile_id == 0) 
    {
        return true;
    }

    return (tile_id & tile_type);
}

Sokoban_Move *sokoban_move_start(Sokoban_Move *move, Sokoban_Direction direction)
{
    int32_t dx = (direction & 3) - 2;
    int32_t dy = ((direction >> 2) & 3) - 2;

    uint8_t tile_id_1 = get_grid_tile_id(sokoban->player_col + dx, sokoban->player_row + dy);
    uint8_t tile_id_2 = get_grid_tile_id(sokoban->player_col + 2 * dx, sokoban->player_row + 2 * dy);
    
    bool is_player_movable = !is_grid_tile_type_of(tile_id_1, SOKOBAN_TILE_ID_BOX) &&
                             !is_grid_tile_type_of(tile_id_1, SOKOBAN_TILE_ID_WALL);

    bool is_box_movable = is_grid_tile_type_of(tile_id_1, SOKOBAN_TILE_ID_BOX) && 
                          (!is_grid_tile_type_of(tile_id_2, SOKOBAN_TILE_ID_BOX) &&
                           !is_grid_tile_type_of(tile_id_2, SOKOBAN_TILE_ID_WALL));

    // can move
    if (is_player_movable || is_box_movable)
    {
        move->has_box = false;

        // next tile is box
        if (is_box_movable)
        {
            set_grid_tile_id(sokoban->player_col + dx, sokoban->player_row + dy, SOKOBAN_TILE_ID_BOX, false);
            //sokoban_set_grid_tile_id(sokoban->player_col + 2 * dx, sokoban->player_row + 2 * dy, SOKOBAN_TILE_ID_BOX, true);
            move->has_box = true;
            move->box_col = sokoban->player_col + dx;
            move->box_row = sokoban->player_row + dy;
        }
        
        // move player
        set_grid_tile_id(sokoban->player_col, sokoban->player_row, SOKOBAN_TILE_ID_PLAYER, false);
        //sokoban->player_col += dx;
        //sokoban->player_row += dy;
        //sokoban_set_grid_tile_id(sokoban->player_col, sokoban->player_row, SOKOBAN_TILE_ID_PLAYER, true);
        move->player_col = sokoban->player_col;
        move->player_row = sokoban->player_row;
        move->direction = direction;
    }
    else
    {
        return NULL;
    }
}

void sokoban_move_commit(Sokoban_Move *move)
{
    int32_t dx = (move->direction & 3) - 2;
    int32_t dy = ((move->direction >> 2) & 3) - 2;

    // move box
    if (move->has_box)
    {
        move->box_col += dx;
        move->box_row += dy;
        set_grid_tile_id(move->box_col, move->box_row, SOKOBAN_TILE_ID_BOX, true);
    }

    // move player
    move->player_col += dx;
    move->player_row += dy;
    sokoban->player_col += dx;
    sokoban->player_row += dy;
    set_grid_tile_id(sokoban->player_col, sokoban->player_row, SOKOBAN_TILE_ID_PLAYER, true);
}

bool sokoban_is_level_cleared()
{
    for (int i = 0; i < sokoban->cols * sokoban->cols; i++)
    {
        uint8_t tile_id = sokoban->grid[i];
        if (is_grid_tile_type_of(tile_id, SOKOBAN_TILE_ID_TARGET))
        {
            if (!is_grid_tile_type_of(tile_id, SOKOBAN_TILE_ID_BOX))
            {
                return false;            
            }
        }
    }
    return true;
}