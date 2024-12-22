
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "util/assert.h"
#include "util/stringUtil.h"
#include "util/memory.h"
#include "io/csvReader.h"

static void csvCheck(csvReader *reader, bool checkPool) {
    assert(reader);
    if (checkPool)
        assert(reader->pool);
}

bool csvRead(csvReader *reader, const char *file) {
    csvCheck(reader, false);

    FILE *f = fopen(file, "rb");
    assertWithLogReturn(f, "文件不存在.", false);

    fseek(f, 0, SEEK_END);
    uint64_t tmpPoolSize = ftell(f) + 1;
    
    fseek(f, 0, SEEK_SET);
    char *tmpPool = (char *) malloc(tmpPoolSize);
    assert(tmpPool);
    fread(tmpPool, tmpPoolSize - 1, 1, f);
    tmpPool[tmpPoolSize - 1] = 0;

    #if defined(_WIN32) || defined(_WIN64)

    reader->pool = ansiToUtf8(tmpPool);
    assert(reader->pool);
    reader->poolSize = strlen(reader->pool) + 1;
    free(tmpPool);

    for (uint64_t i=0; i<reader->poolSize; i++) {
        if (reader->pool[i] == '\r')
            orderedMemcpyL(reader->pool + i, reader->pool + i + 1, reader->poolSize - i - 1);
    }
    reader->poolSize = strlen(reader->pool) + 1;

    #else /* _WIN32 || _WIN64 */

    reader->pool = tmpPool;
    reader->poolSize = tmpPoolSize;

    #endif /* UNIX */

    fclose(f);

    seqSetup(&reader->values, sizeof(char *), 8);
    seqPush(&reader->values, &reader->pool);
    reader->width = 0;

    bool readingHeader = true;
    for (uint64_t i=0; i<reader->poolSize; i++) {
        switch (reader->pool[i]) {
            case '\n':
                if (readingHeader) {
                    readingHeader = false;
                    reader->width ++;
                }
                if (reader->pool[i+1]) {
                    char *ptr = reader->pool + i + 1;
                    seqPush(&reader->values, &ptr);
                }
                reader->pool[i] = 0;
                break;
            case ',':
                if (reader->pool[i+1]) {
                    char *ptr = reader->pool + i + 1;
                    seqPush(&reader->values, &ptr);
                }
                reader->pool[i] = 0;

                if (readingHeader)
                    reader->width ++;
                break;
        }
    }

    return true;
}

void csvReaderFree(csvReader *reader) {
    csvCheck(reader, true);

    free(reader->pool);
    reader->pool = NULL;
    seqFree(&reader->values);
    reader->poolSize = 0;
    reader->width = 0;
}

uint64_t csvGetRowsCount(csvReader *reader) {
    csvCheck(reader, true);
    assertWithReturn(seqSize(&reader->values)%reader->width == 0, -1);
    return seqSize(&reader->values)/reader->width - 1;
}

char *csvGetHeader(csvReader *reader, uint64_t index) {
    csvCheck(reader, true);
    assert(index < reader->width);

    return *(char **) seqAt(&reader->values, index);
}

char *csvGetValue(csvReader *reader, uint64_t rowIndex, uint64_t index) {
    csvCheck(reader, true);
    assert(index < reader->width);
    assert(rowIndex < seqSize(&reader->values)/reader->width - 1);

    return *(char **) seqAt(&reader->values, (rowIndex + 1)*reader->width + index);
}