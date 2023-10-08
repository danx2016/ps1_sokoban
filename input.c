#include <libapi.h>
#include <stdio.h>

#include "input.h"

static Pad_Info pad_info[2];

static uint16_t pad_btn_a[2];
static uint16_t pad_btn_b[2]; // keep track just_pressed states

void input_init(void)
{
    size_t size = sizeof(Pad_Info);
    InitPAD((int8_t*) &pad_info[0], size, (int8_t*) &pad_info[1], size);
    StartPAD();
}

void input_fixed_update(void)
{
    for (int c = 0; c < 2; c++)
    {
        uint16_t pad = ~pad_info[c].btn;
        pad_btn_b[c] = pad & (pad ^ pad_btn_a[c]);
        pad_btn_a[c] = pad;
    }
}

bool input_is_action_pressed(Action action)
{
    if(pad_info[0].stat == 0 && pad_info[0].type == 4)
    {
        return (pad_btn_a[0] & action);
    }
    else 
    {
        return false;
    }
}

bool input_is_action_just_pressed(Action action)
{
    if(pad_info[0].stat == 0 && pad_info[0].type == 4)
    {
        return (pad_btn_b[0] & action);
    }
    else 
    {
        return false;
    }
}
