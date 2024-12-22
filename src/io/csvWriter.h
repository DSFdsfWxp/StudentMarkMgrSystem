
#ifndef _csvWriter
#define _csvWriter

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    const char *filePath;
    FILE *file;
    bool writtenHeader;
    uint64_t width;
    uint64_t contentItemCnt;
} csvWriter;

bool csvWriteBegin(csvWriter *writer, const char *file);
void csvWriteDone(csvWriter *writer);

void csvWriteHeaderItem(csvWriter *writer, const char *item);
void csvWriteHeaderDone(csvWriter *writer);

void csvWriteContentItem(csvWriter *writer, const char *item);

#endif /* _csvWriter */