
#ifndef _table
#define _table

#include <stdint.h>
#include "io/csvReader.h"
#include "io/csvWriter.h"
#include "io/tablePrinter.h"
#include "struct/subTable.h"
#include "util/seq.h"

typedef struct {
    seq subjects;
    seq subTables;
} table;

void tableInit(table *instance);
void tableFree(table *instance);

bool tableRead(table *instance, csvReader *reader);
void tableWrite(table *instance, csvWriter *writer);
void tablePrint(table *instance, tablePrinter *printer);
void tablePrintAll(table *instance);

subTable *tableGetSubTable(table *instance, uint8_t term);
subTable *tableAddSubTable(table *instance, uint8_t term);

void tableTryShrink(table *instance);

void tablePrintSubjects(table *instance);
void tableAddSubject(table *instance, const char *subject);
bool tableRemoveSubject(table *instance, const char *subject);
uint64_t tableFindSubject(table *instance, const char *subject);

#endif /* _table */