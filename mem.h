#ifndef _MEM_H
#define _MEM_H

#include <stddef.h>

#define MEM_HEAP_SIZE 1048576

extern void mem_init(void);
extern void *mem_alloc(size_t size);
extern void mem_free(void *ptr);

#endif /* _MEM_H */