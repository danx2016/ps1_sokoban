#include <sys/file.h>
#include <kernel.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <libapi.h>

#include "mem_card.h"

static const uint8_t* port_to_device[] = { "bu00:", "bu10:" };

static const int32_t descriptors[] = { SwCARD, HwCARD };
static const int32_t classes[] = { EvSpIOE, EvSpERROR, EvSpTIMOUT, EvSpNEW };

static int32_t events[2][4];

void mem_card_init()
{
    // open events related to memory card
    EnterCriticalSection();
    for (size_t d = 0; d < 2; d++)
    {
        for (size_t c = 0; c < 4; c++)
        { 
            events[d][c] = OpenEvent(descriptors[d], classes[c], EvMdNOINTR, NULL);
        }
    }
    ExitCriticalSection();

    // enable events
    for (size_t d = 0; d < 2; d++)
    {
        for (size_t c = 0; c < 4; c++)
        { 
            EnableEvent(events[d][c]);
        }
    }
 
    // init memory card
    InitCARD(1);
    StartCARD();

    // initialize the memory card file system
    _bu_init();    
}

static int32_t clear_result(int32_t descriptor)
{
    size_t descriptor_index = descriptor == SwCARD ? 0 : 1;
    for (uint32_t c = 0; c < 4; c++)
    { 
        TestEvent(events[descriptor_index][c]);
    }
}

static int32_t wait_result(int32_t descriptor)
{
    size_t descriptor_index = descriptor == SwCARD ? 0 : 1;
    while (true)
    {
        for (uint32_t c = 0; c < 4; c++)
        { 
            if (TestEvent(events[descriptor_index][c]))
            {
                return classes[c];
            }
        }
    }
}

bool mem_card_check(uint32_t port)
{
    int32_t result_class;
    int32_t chan = port << 4;
    
    clear_result(SwCARD);

    if (!_card_info(chan))
    {
        return false;
    }

    result_class = wait_result(SwCARD);

    // card not connected
    if (result_class == EvSpERROR || result_class == EvSpTIMOUT)
    {
        return false;
    }
    else if (result_class != EvSpIOE)
    {
        clear_result(HwCARD);

        if (!_card_clear(chan))
        {
            return false;
        }

        result_class = wait_result(HwCARD);

        // card not connected
        if (result_class == EvSpERROR || result_class == EvSpTIMOUT)
        {
            return false;
        }
    }

    clear_result(SwCARD);

    // test logical format
    if (!_card_load(chan))
    {
        return false;
    }

    result_class = wait_result(SwCARD);

    // card not connected
    if (result_class == EvSpERROR || result_class == EvSpTIMOUT)
    {
        return false;
    }
    else if (result_class == EvSpNEW) {

        // found not formatted memory card, so format (?)
        if (!_card_format(chan))
        {
            return false;
        }
    }

    return true;
}

void mem_card_get_files_info(uint32_t port, Memory_Card_Files_Info *files_info)
{
    uint8_t name[8];
    strcpy(name, port_to_device[port]);
    strcat(name, "*");
    struct DIRENTRY file_entry;
    size_t file_index = 0;
    uint32_t total_used_8k_blocks = 0;
    if (firstfile(name, &file_entry))
    {
        do {
            // printf("file found %s ..\n", file_entry.name);
            strcpy(files_info->file_names[file_index], file_entry.name);
            files_info->file_sizes[file_index] = file_entry.size;
            files_info->file_num_8k_blocks[file_index] = file_entry.size / 8192;
            total_used_8k_blocks += file_entry.size / 8192;
            file_index++;
        }
        while (nextfile(&file_entry));
    }
    files_info->port = port;
    files_info->num_entries = file_index;
    files_info->num_free_8k_blocks = 15 - total_used_8k_blocks;
}

bool mem_card_check_file_exists(uint32_t port, uint8_t *filename)
{
    uint8_t name[128];
    strcpy(name, port_to_device[port]);
    strcat(name, filename);
    struct DIRENTRY file_entry;
    return firstfile(name, &file_entry);
}

bool mem_card_create_file(uint32_t port, uint8_t *filename, uint32_t block_8k_num)
{
    uint8_t name[128];
    strcpy(name, port_to_device[port]);
    strcat(name, filename);
    int32_t fd = open(name, O_CREAT | (block_8k_num << 16));
    close(fd);
    return (fd != -1);
}

bool mem_card_delete_file(uint32_t port, uint8_t *filename)
{
    uint8_t name[128];
    strcpy(name, port_to_device[port]);
    strcat(name, filename);
    delete(name);
}

bool mem_card_write_file(uint32_t port, uint8_t *filename, uint8_t *filedata, uint32_t filesize)
{
    uint32_t sectors_num = (filesize + 127) / 128;
    uint8_t name[128];
    strcpy(name, port_to_device[port]);
    strcat(name, filename);
    int32_t fd = open(name, O_WRONLY);
    if (fd != -1)
    { 
        write(fd, filedata, sectors_num * 128);
        close(fd);
    }
    return (fd != -1);
}

bool mem_card_read_file(uint32_t port, uint8_t *filename, uint8_t *data_buffer, uint32_t data_size)
{
    uint8_t name[128];
    strcpy(name, port_to_device[port]);
    strcat(name, filename);

    struct DIRENTRY file_entry;
    if (!firstfile(name, &file_entry))
    {
        return false;
    }

    int32_t fd = open(name, O_RDONLY);
    if (fd != -1)
    {
        read(fd, data_buffer, data_size); //file_entry.size);
        close(fd);
    }
    return (fd != -1);
}

#include "game.h"

bool save_mem_card_game_entry(uint32_t port, uint8_t last_cleared_level)
{
    if (mem_card_check(port))
    {
        uint8_t file_buffer[GAME_MEM_CARD_FILE_SIZE];
        Memory_Card_Entry_Header *header = (Memory_Card_Entry_Header*) file_buffer;

        // prepare the header
        header->magic[0] = 'S'; 
        header->magic[1] = 'C';  // ID must always say SC
        header->type = 0x12;     // 2 frame icon
        header->block_entry = 1; // 1 block
        
        strcpy(header->title, "PS1 SOKOBAN DANX2016");
        
        uint8_t _clut[32];
        uint8_t _icon[128];

        // copy icon's CLUT to the header
        memcpy(header->clut, _clut, 32);

        memcpy(header->icon[0], _icon[0], 128);
        memcpy(header->icon[1], _icon[1], 128);

        // game_last_cleared_level 
        file_buffer[sizeof(Memory_Card_Entry_Header) + 0] = last_cleared_level;
        file_buffer[sizeof(Memory_Card_Entry_Header) + 1] = 0;
        file_buffer[sizeof(Memory_Card_Entry_Header) + 2] = 0;
        file_buffer[sizeof(Memory_Card_Entry_Header) + 3] = 0;

        mem_card_write_file(port, GAME_MEM_CARD_FILE_NAME, (uint8_t*) file_buffer, GAME_MEM_CARD_FILE_SIZE);

        return mem_card_check_file_exists(port, GAME_MEM_CARD_FILE_NAME);
    }
    return false;
}
