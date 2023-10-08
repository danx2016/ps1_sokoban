#include <stdint.h>
#include <malloc.h>

#include "mem.h"

static uint8_t mem_heap[MEM_HEAP_SIZE];

void mem_init(void)
{
    InitHeap3((uint32_t*) mem_heap, MEM_HEAP_SIZE);    
}

void *mem_alloc(size_t size)
{
    return malloc3(size);
}

void mem_free(void *ptr)
{
    free3(ptr);
}
