
#include "util/memory.h"

void orderedMemcpyL(void *dest, const void *src, size_t size) {
    char *d = (char *) dest;
    char *s = (char *) src;

    for (size_t i=0; i<size; i++)
        d[i] = s[i];
}

void orderedMemcpyR(void *dest, const void *src, size_t size) {
    char *d = (char *) dest;
    char *s = (char *) src;

    if (size == 0)
        return;

    for (size_t i=size-1; i<size; i--)
        d[i] = s[i];
}