#include <stdint.h>
#include <stdbool.h>
#include <libgpu.h>
#include <libetc.h>

#include "gfx.h"

static uint32_t gfx_screen_width;
static uint32_t gfx_screen_height;
static uint32_t gfx_screen_offset_y;

static DISPENV disp[2];
static DRAWENV draw[2];
static uint32_t db = 0;

#define OT_LEN 64
static uint32_t ot[2][OT_LEN];
static uint32_t ot_index;

#define PRIMITIVES_BUFFER_SIZE 32768
static uint8_t primitives_buffer[2][PRIMITIVES_BUFFER_SIZE];
static uint8_t *next_primitive;

typedef struct
{
    uint32_t cols;
    uint32_t rows;
    uint32_t tile_width;
    uint32_t tile_height;
    GFX_TIM_Info *tim_info;
} TileSet;

static TileSet tilesets[GFX_MAX_NUM_TILESETS];
static TileSet *current_tileset;

static uint32_t gfx_offset_x = 0;
static uint32_t gfx_offset_y = 0;

// bitmap font
static GFX_TIM_Info *font_img;
static uint8_t font_width;
static uint8_t font_height;
static SVECTOR font_coords[256] = { 0 };

// font box
static uint8_t gfx_border_chars[18] = { 0 };

// kindly provided by @spicyjpeg
static bool is_gpu_in_pal_mode(void) {
    return (*((uint32_t *) 0x1f801814) >> 20) & 1;
}

static void gfx_begin_draw(void)
{
    ClearOTagR(&ot[db][0], OT_LEN);
    next_primitive = &primitives_buffer[db][0];
    ot_index = OT_LEN;
}

void gfx_init(void)
{
    bool pal_mode = is_gpu_in_pal_mode();
    ResetGraph(0);
    if (pal_mode)
    {
        gfx_screen_width = 256;
        gfx_screen_height = 256;
        gfx_screen_offset_y = 0;
        SetVideoMode(MODE_PAL);
    }
    else
    {
        gfx_screen_width = 256;
        gfx_screen_height = 240;
        gfx_screen_offset_y = 8;
        SetVideoMode(MODE_NTSC);
    }
    
    SetDefDispEnv(&disp[0], 0, gfx_screen_offset_y, gfx_screen_width, gfx_screen_height);
    SetDefDispEnv(&disp[1], 0, 256 + gfx_screen_offset_y, gfx_screen_width, gfx_screen_height);
    SetDefDrawEnv(&draw[0], 0, 256, gfx_screen_width, 256);
    SetDefDrawEnv(&draw[1], 0, 0, gfx_screen_width, 256);

    // effectively change the display vertical resolution
    disp[0].screen.h = gfx_screen_height;
    disp[1].screen.h = gfx_screen_height;

    //draw[0].isbg = 1;
    //draw[1].isbg = 1;
    //setRGB0(&draw[0], 32, 32, 32);
    //setRGB0(&draw[1], 32, 32, 32);

    // clear entire vram
    RECT rect = { 0, 0, 1024, 512 };
    ClearImage(&rect, 0, 0, 0);
    DrawSync(0);

    SetDispMask(1);
    gfx_begin_draw();
}

void gfx_swap_buffers(void)
{
    DrawSync(0);
    VSync(0);
    DrawOTag2((uint32_t*) &ot[db][OT_LEN - 1]);
    db = !db;
    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);
    gfx_begin_draw();
}

void gfx_set_offsets(uint32_t offset_x, uint32_t offset_y)
{
    gfx_offset_x = offset_x;
    gfx_offset_y = offset_y;
}

void gfx_add_tileset(size_t tileset_id, GFX_TIM_Info *tileset, uint32_t cols, uint32_t rows)
{
    tilesets[tileset_id].cols = cols;
    tilesets[tileset_id].rows = rows;
    tilesets[tileset_id].tile_width = (tileset->prect.w << (tileset->mode & 3)) / cols; 
    tilesets[tileset_id].tile_height = tileset->prect.h / rows;
    tilesets[tileset_id].tim_info = tileset;
}

void gfx_use_tileset(size_t tileset_id)
{
    current_tileset = &tilesets[tileset_id];
    GFX_TIM_Info *tileset = current_tileset->tim_info; 
    DR_TPAGE* tpage = (DR_TPAGE*) next_primitive;
    uint16_t tp = getTPage(tileset->mode & 3, 0, tileset->prect.x, tileset->prect.y);
    setDrawTPage(tpage, 0, 0, tp);
    AddPrim(&ot[db][--ot_index], tpage);
    next_primitive += sizeof(DR_TPAGE);
    ot_index--;
}

void gfx_draw_tile(size_t tile_id, uint32_t x, uint32_t y)
{
    TileSet *tileset = current_tileset;
    SPRT *sprt = (SPRT*) next_primitive;
    setSprt(sprt);
    setXY0(sprt, x + gfx_offset_x, y + gfx_offset_y);
    setWH(sprt, tileset->tile_width, tileset->tile_height);
    uint8_t u = (tile_id % tileset->cols) * tileset->tile_width;
    uint8_t v = (tile_id / tileset->cols) * tileset->tile_height;
    setUV0(sprt, u, v);
    setClut(sprt, tileset->tim_info->crect.x, tileset->tim_info->crect.y);
    setRGB0(sprt, 128, 128, 128);
    addPrim(&ot[db][ot_index], sprt);
    next_primitive += sizeof(SPRT);
}

void gfx_set_font(GFX_TIM_Info *font, uint8_t cols, uint8_t rows, uint8_t width, uint8_t height, uint8_t* charset)
{
    font_img = font;
    font_width = width;
    font_height = height;
    // font coordinates
    uint32_t chars_count = cols * rows;
    int16_t rx = 0;
    int16_t ry = 0;
    for (uint32_t i = 0; i < chars_count; i++)
    {
        uint8_t char_index = charset[i];
        font_coords[char_index].vx = rx;
        font_coords[char_index].vy = ry;
        rx += font_width;
        if (rx > (cols - 1) * font_width)
        {
            rx = 0;
            ry += font_height;
        }
    }
}

void gfx_use_font(void)
{
    DR_TPAGE* tpage = (DR_TPAGE*) next_primitive;
    uint16_t tp = getTPage(font_img->mode & 3, 2, font_img->prect.x, font_img->prect.y);
    setDrawTPage(tpage, 0, 0, tp);
    AddPrim(&ot[db][--ot_index], tpage);
    next_primitive += sizeof(DR_TPAGE);
    ot_index--;
}

void gfx_draw_text(uint8_t* text, int16_t x, int16_t y)
{
    gfx_draw_text_rgb(text, x, y, 128, 128, 128);
}

void gfx_draw_text_rgb(uint8_t* text, int16_t x, int16_t y, int8_t r, int8_t g, int8_t b)
{
    uint32_t char_index = 0;
    uint8_t current_char;
    while (current_char = text[char_index])
    {
        SVECTOR* font_coord = &font_coords[current_char];
        SPRT *sprt = (SPRT*) next_primitive;
        setSprt(sprt);
        setXY0(sprt, x, y);
        setUV0(sprt, font_coord->vx, font_coord->vy);
        setWH(sprt, font_width, font_height);
        setRGB0(sprt, r, g, b);
        setClut(sprt, font_img->crect.x, font_img->crect.y);
        addPrim(&ot[db][ot_index], sprt);
        next_primitive += sizeof(SPRT);
        x += font_width;
        char_index++;
    } 
}

void gfx_fill_rect(int16_t x, int16_t y, int16_t w,  int16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t ab)
{
    TILE *tile = (TILE*) next_primitive;
    setTile(tile);
    setXY0(tile, x, y);
    setWH(tile, w, h);
    setRGB0(tile, r, g, b);
    setSemiTrans(tile, (ab > 0));
    addPrim(&ot[db][ot_index], tile);
    next_primitive += sizeof(TILE);
}

// border_chars = "(tl)(t)(tr)(l)(c)(r)(bl)(b)(br)"
void gfx_set_font_box_border_chars(uint8_t *border_chars)
{
    for (int i = 0; i < 9; i++)
    {
        gfx_border_chars[i * 2] = border_chars[i];
    }
}

void gfx_draw_font_box(int32_t x, int32_t y, uint32_t cols, uint32_t rows, uint8_t transp)
{
    uint32_t bx = x;
    uint32_t by = y;
    uint32_t bb = by + GFX_BOX_FONT_H * (rows - 1);
    uint32_t br = bx + GFX_BOX_FONT_W * (cols - 1);

    gfx_use_font();

    for (int c = 1; c < cols - 1; c++)
    {
        gfx_draw_text(&gfx_border_chars[2], bx + c * GFX_BOX_FONT_W, by);    
        gfx_draw_text(&gfx_border_chars[14], bx + c * GFX_BOX_FONT_W, bb);
    }
    
    for (int r = 1; r < rows - 1; r++)
    {
        gfx_draw_text(&gfx_border_chars[6], bx, by + r * GFX_BOX_FONT_H);    
        gfx_draw_text(&gfx_border_chars[10], br, by + r * GFX_BOX_FONT_H);
    }

    gfx_draw_text(&gfx_border_chars[0], bx, by);  
    gfx_draw_text(&gfx_border_chars[4], br, by);  
    gfx_draw_text(&gfx_border_chars[12], bx, bb);  
    gfx_draw_text(&gfx_border_chars[16], br, bb);  
    
    gfx_fill_rect(x + GFX_BOX_FONT_HALF_W, y + GFX_BOX_FONT_HALF_H, (cols - 1) * GFX_BOX_FONT_W, (rows - 1) * GFX_BOX_FONT_H, transp, transp, transp, 2);
}