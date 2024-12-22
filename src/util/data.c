
#include "io/csvReader.h"
#include "io/csvWriter.h"
#include "util/assert.h"
#include "util/data.h"

table tableStruct;

bool dataLoad(const char *path) {
    tableInit(&tableStruct);
    
    csvReader reader;
    assertWithReturn(csvRead(&reader, path), false);
    assertWithReturn(tableRead(&tableStruct, &reader), false);
    csvReaderFree(&reader);

    return true;
}

bool dataSave(const char *path) {
    csvWriter writer;

    assertWithReturn(csvWriteBegin(&writer, path), false);
    tableWrite(&tableStruct, &writer);
    csvWriteDone(&writer);

    return true;
}

void dataFree() {
    tableFree(&tableStruct);
}