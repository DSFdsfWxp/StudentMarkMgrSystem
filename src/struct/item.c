
#include <string.h>
#include "util/assert.h"
#include "util/stringUtil.h"
#include "struct/item.h"

void itemInit(item *instance, const char *id, const char *name, uint8_t term) {
    assert(instance);
    assert(strlen(id)<16 && strlen(name)<30);

    strcpy(instance->id, id);
    strcpy(instance->name, name);
    instance->term = term;
    seqSetup(&instance->marks, sizeof(float), 8);
}

void itemFree(item *instance) {
    assert(instance);

    seqFree(&instance->marks);
}

void itemAddMark(item *instance, float mark) {
    assert(instance);

    seqPush(&instance->marks, &mark);
}

void itemRemoveMark(item *instance, uint64_t index) {
    assert(instance);

    seqRemoveByIndex(&instance->marks, index);
}

void itemModifyMark(item *instance, uint64_t index, float mark) {
    assert(instance);

    *(float *) seqAt(&instance->marks, index) = mark;
}

float itemGetMark(item *instance, uint64_t index) {
    assert(instance);

    return *(float *) seqAt(&instance->marks, index);
}

float itemTotalMark(item *instance) {
    assert(instance);

    float sum = 0;
    for (float *ptr = (float *) seqFirst(&instance->marks), *end = (float *) seqLast(&instance->marks); ptr<=end; ptr ++)
        sum += *ptr;

    return sum;
}

void itemWrite(item *instance, csvWriter *writer) {
    assert(instance);

    csvWriteContentItem(writer, instance->id);
    csvWriteContentItem(writer, instance->name);
    csvWriteContentItem(writer, itos(instance->term));

    for (float *ptr = (float *) seqFirst(&instance->marks), *end = (float *) seqLast(&instance->marks); ptr<=end; ptr ++)
        csvWriteContentItem(writer, ftos(*ptr));
}

void itemPrint(item *instance, tablePrinter *printer) {
    assert(instance);

    tablePrintContentItem(printer, instance->id);
    tablePrintContentItem(printer, instance->name);
    tablePrintContentItem(printer, itos(instance->term));

    for (float *ptr = (float *) seqFirst(&instance->marks), *end = (float *) seqLast(&instance->marks); ptr<=end; ptr ++)
        tablePrintContentItem(printer, ftos(*ptr));

    tablePrintContentItem(printer, ftos(itemTotalMark(instance)));
}