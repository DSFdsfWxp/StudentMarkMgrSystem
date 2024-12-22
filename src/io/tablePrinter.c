
#include <stdio.h>
#include <string.h>
#include "util/assert.h"
#include "util/stringUtil.h"
#include "io/tablePrinter.h"

void tablePrinterBegin(tablePrinter *printer) {
    assert(printer);

    seqSetup(&printer->values, sizeof(char *), 8);
    printer->writtenHeader = false;
    printer->width = 0;
}

void tablePrinterDone(tablePrinter *printer) {
    assert(printer);
    assert(seqSize(&printer->values)%printer->width == 0);

    uint64_t height = seqSize(&printer->values) / printer->width;
    uint64_t *itemWidths = (uint64_t *) malloc(printer->width * sizeof(uint64_t));
    for (uint64_t i=0; i<printer->width; i++) {
        itemWidths[i] = 0;
        for (uint64_t j=0; j<height; j++) {
            char *item = *(char **) seqAt(&printer->values, j*printer->width + i);
            uint64_t itemWidth = utf8Strlen(item) + fullWidthCharCount(item);

            if (itemWidths[i] < itemWidth)
                itemWidths[i] = itemWidth;
        }
    }

    uint64_t headerWidth = 0;
    for (uint64_t i=0; i<printer->width; i++)
        headerWidth += itemWidths[i] + 1;

    for (uint64_t i=0; i<height; i++) {
        for (uint64_t j=0; j<printer->width; j++) {
            char *item = *(char **) seqAt(&printer->values, i*printer->width + j);
            uint64_t itemWidth = utf8Strlen(item) + fullWidthCharCount(item);

            printf("%s", item);
            if (j+1 < printer->width) {
                for (uint64_t k=0; k<=itemWidths[j]-itemWidth; k++)
                    putchar(' ');
            } else {
                putchar('\n');
            }
        }

        if (i == 0 || i+1 == height) {
            for (uint64_t k=0; k<headerWidth; k++)
                putchar('=');
            putchar('\n');
        }
    }

    free(itemWidths);

    for (char **ptr=seqFirst(&printer->values), **end=seqLast(&printer->values); ptr<=end; ptr++)
        free(*ptr);
    seqFree(&printer->values);
}

void tablePrintHeaderItem(tablePrinter *printer, const char *item) {
    assert(printer);
    assert(!printer->writtenHeader);

    char *itemCopy = strdup(item);
    seqPush(&printer->values, &itemCopy);
    printer->width ++;
}

void tablePrintHeaderDone(tablePrinter *printer) {
    assert(printer);
    assert(!printer->writtenHeader);

    printer->writtenHeader = true;
}

void tablePrintContentItem(tablePrinter *printer, const char *item) {
    assert(printer);
    assert(printer->writtenHeader);

    char *itemCopy = strdup(item);
    seqPush(&printer->values, &itemCopy);
}