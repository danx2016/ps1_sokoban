#ifndef _RES_H
#define _RES_H

#include <stdint.h>
#include <libgpu.h>

#include "gfx.h"
#include "sokoban.h"

extern void res_init(void);
extern void *res_load(uint8_t *filename);
extern GFX_TIM_Info *res_load_tim_image(uint8_t *filename);
extern Sokoban_Level *res_load_sokoban_level(uint8_t *filename);

#endif /* _RES_H */
