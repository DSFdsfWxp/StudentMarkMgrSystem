
#ifndef _memory
#define _memory

#include <stdlib.h>

void orderedMemcpyL(void *dest, const void *src, size_t size);
void orderedMemcpyR(void *dest, const void *src, size_t size);

#endif /* _memory */