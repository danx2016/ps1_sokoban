#ifndef _INPUT_H
#define _INPUT_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint8_t	 stat;
    uint8_t	 len:4;
    uint8_t	 type:4;
    uint16_t btn;
} Pad_Info;

typedef enum
{
    ACTION_UP      =    16,
    ACTION_DOWN    =    64,
    ACTION_LEFT    =   128,
    ACTION_RIGHT   =    32,
    ACTION_CONFIRM =  8192,
    ACTION_CANCEL  = 16384,
    ACTION_SELECT  =     1,
    ACTION_START   =     8
} Action;

extern void input_init(void);
extern void input_fixed_update(void);
extern bool input_is_action_pressed(Action action);
extern bool input_is_action_just_pressed(Action action);

#endif /* _INPUT_H */