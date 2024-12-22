
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "util/assert.h"
#include "util/stringUtil.h"
#include "io/csvWriter.h"

void csvCheck(csvWriter *writer, bool checkFile) {
    assert(writer);
    if (checkFile)
        assert(writer->file);
}

bool csvWriteBegin(csvWriter *writer, const char *file) {
    csvCheck(writer, false);

    writer->filePath = file;
    writer->file = fopen(file, "w");
    assertWithReturn(writer->file, false);
    writer->writtenHeader = false;
    writer->width = 0;
    writer->contentItemCnt = 0;

    return true;
}

void csvWriteDone(csvWriter *writer) {
    csvCheck(writer, true);

    fclose(writer->file);
    writer->file = NULL;

    #if defined(_WIN32) || defined(_WIN64)

    FILE *f = fopen(writer->filePath, "rb");
    fseek(f, 0, SEEK_END);
    uint64_t poolLen = ftell(f) + 1;

    char *pool = (char *) malloc(poolLen);
    fseek(f, 0, SEEK_SET);
    fread(pool, poolLen - 1, 1, f);
    pool[poolLen - 1] = 0;
    fclose(f);

    char *convertedPool = utf8ToAnsi(pool);
    f = fopen(writer->filePath, "wb");
    fwrite(convertedPool, strlen(convertedPool), 1, f);
    fclose(f);

    free(pool);
    free(convertedPool);

    #endif /* _WIN32 || _WIN64 */
}

void csvWriteHeaderItem(csvWriter *writer, const char *item) {
    csvCheck(writer, true);
    assert(!writer->writtenHeader);

    fprintf(writer->file, writer->width > 0 ? ",%s" : "%s", item);
    writer->width ++;
}

void csvWriteHeaderDone(csvWriter *writer) {
    csvCheck(writer, true);
    assert(!writer->writtenHeader);

    fprintf(writer->file, "\n");
    writer->writtenHeader = true;
}

void csvWriteContentItem(csvWriter *writer, const char *item) {
    csvCheck(writer, true);

    fprintf(writer->file, writer->contentItemCnt%writer->width > 0 ? ",%s" : "%s", item);
    if (++ writer->contentItemCnt % writer->width == 0)
        fprintf(writer->file, "\n");
}