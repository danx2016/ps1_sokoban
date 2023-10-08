#ifndef _MEM_CARD_H
#define _MEM_CARD_H

#include <kernel.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint32_t port;
    uint32_t num_entries;
    uint8_t file_names[15][20];
    uint32_t file_sizes[15];
    uint32_t file_num_8k_blocks[15];
    uint32_t num_free_8k_blocks;
} Memory_Card_Files_Info;


#define MEM_CARD_PORT_1 0
#define MEM_CARD_PORT_2 1

// port -> 0=port_1, 1=port_2
extern void mem_card_init();
extern bool mem_card_check(uint32_t port);
extern void mem_card_get_files_info(uint32_t port, Memory_Card_Files_Info *files_info);
extern bool mem_card_check_file_exists(uint32_t port, uint8_t *filename);
extern bool mem_card_create_file(uint32_t port, uint8_t *filename, uint32_t block_8k_num);
extern bool mem_card_delete_file(uint32_t port, uint8_t *filename);
extern bool mem_card_write_file(uint32_t port, uint8_t *filename, uint8_t *filedata, uint32_t filesize);

// data_size must be multiple of 128
extern bool mem_card_read_file(uint32_t port, uint8_t *filename, uint8_t *data_buffer, uint32_t data_size);

typedef struct {
    uint8_t magic[2];     // always 'SC'
    uint8_t type;         // number of icon frames (0x11 - one frame, 0x12 - two frames, 0x13 - three frames)
    uint8_t block_entry;  // size of save file in blocks
    uint8_t title[64];    // title of save file (encoded in Shift-JIS format)
    uint8_t reserve[28];  // unused
    uint8_t clut[32];     // color palette of icon frames (16 RGB5X1 16-bit color entries)
    uint8_t icon[3][128];
} Memory_Card_Entry_Header;

extern bool save_mem_card_game_entry(uint32_t port, uint8_t last_cleared_level);

#endif /* _MEM_CARD_H */