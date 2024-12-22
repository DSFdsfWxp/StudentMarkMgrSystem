
#ifndef _item
#define _item

#include <stdint.h>
#include "io/csvWriter.h"
#include "io/tablePrinter.h"
#include "util/seq.h"

typedef struct {
    char id[16];
    char name[30];
    uint8_t term;
    seq marks;
} item;

void itemInit(item *instance, const char *id, const char *name, uint8_t term);
void itemFree(item *instance);

void itemAddMark(item *instance, float mark);
void itemRemoveMark(item *instance, uint64_t index);
void itemModifyMark(item *instance, uint64_t index, float mark);
float itemGetMark(item *instance, uint64_t index);
float itemTotalMark(item *instance);

void itemWrite(item *instance, csvWriter *writer);
void itemPrint(item *instance, tablePrinter *printer);

#endif /* _item */