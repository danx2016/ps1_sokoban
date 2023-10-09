#ifndef _SOKOBAN_H
#define _SOKOBAN_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define SOKOBAN_TILE_ID_EMPTY          0
#define SOKOBAN_TILE_ID_WALL           1
#define SOKOBAN_TILE_ID_BOX            2
#define SOKOBAN_TILE_ID_TARGET         4
#define SOKOBAN_TILE_ID_PLAYER         8
#define SOKOBAN_TILE_ID_TARGET_OR_BOX  6
#define SOKOBAN_TILE_ID_IGNORE        16

typedef struct
{
    uint8_t player_col;
    uint8_t player_row;
    uint8_t rows;
    uint8_t cols;
    uint8_t grid[];
} Sokoban_Level;

typedef enum
{
    UP    =  6,
    DOWN  = 14,
    LEFT  =  9,
    RIGHT = 11
} Sokoban_Direction;

typedef struct
{
    uint8_t player_col;
    uint8_t player_row;
    bool has_box;
    uint8_t box_col;
    uint8_t box_row;
    Sokoban_Direction direction;
} Sokoban_Move;

extern Sokoban_Level *sokoban;

extern void sokoban_start(Sokoban_Level *_sokoban);
extern void sokoban_reset(void);
extern Sokoban_Move *sokoban_move_start(Sokoban_Move *move, Sokoban_Direction direction);
extern void sokoban_move_commit(Sokoban_Move *move);
extern bool sokoban_is_level_cleared(void);

#endif /* _SOKOBAN_H */