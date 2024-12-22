
#ifndef _subTable
#define _subTable

#include <stdint.h>
#include "io/csvWriter.h"
#include "io/tablePrinter.h"
#include "struct/item.h"
#include "util/seq.h"

typedef struct {
    uint8_t term;
    seq items;
} subTable;

enum subTableValueCompareOp {
    stvco_Larger = 1, // 001
    stvco_Small = 2,  // 010
    stvco_Equal = 4   // 100
};

void subTableInit(subTable *instance, uint8_t term);
void subTableFree(subTable *instance);

item *subTableAddItem(subTable *instance, const char *id, const char *name);
void subTableRemoveItem(subTable *instance, item *item);

item *subTableFindItemById(subTable *instance, const char *id);
item *subTableFindItemByName(subTable *instance, const char *name);
void subTableFindItemByMark(subTable *instance, uint64_t subjectIndex, seq *res, float value, enum subTableValueCompareOp op);
void subTableFindItemByTotalMark(subTable *instance, seq *res, float value, enum subTableValueCompareOp op);

bool subTableSortItemById(subTable *instance, enum seqSortMode mode);
void subTableSortItemByMark(subTable *instance, uint64_t subjectIndex, enum seqSortMode mode);
void subTableSortItemByTotalMark(subTable *instance, enum seqSortMode mode);

void subTableWrite(subTable *instance, csvWriter *writer);
void subTablePrint(subTable *instance, tablePrinter *printer);

#endif /* _subTable */