
#ifndef _tablePrinter
#define _tablePrinter

#include <stdint.h>
#include <stdbool.h>
#include "util/seq.h"

typedef struct {
    bool writtenHeader;
    uint64_t width;
    seq values;
} tablePrinter;

void tablePrinterBegin(tablePrinter *printer);
void tablePrinterDone(tablePrinter *printer);

void tablePrintHeaderItem(tablePrinter *printer, const char *item);
void tablePrintHeaderDone(tablePrinter *printer);

void tablePrintContentItem(tablePrinter *printer, const char *item);

#endif /* _tablePrinter */