
#ifndef _csvReader
#define _csvReader

#include <stdbool.h>
#include <stdint.h>
#include "util/seq.h"

typedef struct {
    char *pool;
    seq values;
    uint64_t width;
    uint64_t poolSize;
} csvReader;

bool csvRead(csvReader *reader, const char *file);
void csvReaderFree(csvReader *reader);

uint64_t csvGetRowsCount(csvReader *reader);

char *csvGetHeader(csvReader *reader, uint64_t index);
char *csvGetValue(csvReader *reader, uint64_t rowIndex, uint64_t index);

#endif /* _csvReader */