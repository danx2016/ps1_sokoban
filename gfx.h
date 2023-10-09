#ifndef _GFX_H
#define _GFX_H

#include <stdint.h>
#include <libgpu.h>

#define GFX_MAX_NUM_TILESETS 4

typedef struct
{
	uint32_t mode;	// pixel mode
	RECT     crect;	// CLUT rectangle on frame buffer
	uint32_t caddr;	// CLUT address on main memory
	RECT     prect;	// texture image rectangle on frame buffer
	uint32_t paddr;	// texture image address on main memory

} GFX_TIM_Info;

extern void gfx_init(void);
extern void gfx_swap_buffers(void);

extern void gfx_set_offsets(uint32_t offset_x, uint32_t offset_y);

extern void gfx_add_tileset(size_t tileset_id, GFX_TIM_Info *tileset, uint32_t cols, uint32_t rows);
extern void gfx_use_tileset(size_t tileset_id);
extern void gfx_draw_tile(size_t tile_id, uint32_t x, uint32_t y);

extern void gfx_set_font(GFX_TIM_Info *font, uint8_t cols, uint8_t rows, uint8_t width, uint8_t height, uint8_t* charset);
extern void gfx_use_font(void);
extern void gfx_draw_text(uint8_t* text, int16_t x, int16_t y);
extern void gfx_draw_text_rgb(uint8_t* text, int16_t x, int16_t y, int8_t r, int8_t g, int8_t b);

// ab -> 0=none, 1=sub (only sub alpha blending mode is supported for now)
extern void gfx_fill_rect(int16_t x, int16_t y, int16_t w,  int16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t ab);

#define GFX_BOX_FONT_W 8
#define GFX_BOX_FONT_H 8
#define GFX_BOX_FONT_HALF_W 4
#define GFX_BOX_FONT_HALF_H 4

extern void gfx_set_font_box_border_chars(uint8_t *border_chars);

// only 8x8 fonts are supported in the current implementation
extern void gfx_draw_font_box(int32_t x, int32_t y, uint32_t cols, uint32_t rows, uint8_t transp);

#endif /* _GFX_H */