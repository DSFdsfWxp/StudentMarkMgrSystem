
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "util/assert.h"
#include "util/stringUtil.h"
#include "struct/subTable.h"

typedef struct {
    bool isTotalMark;
    bool isId;
    uint64_t subjectIndex;
} itemSortContext;

void subTableInit(subTable *instance, uint8_t term) {
    assert(instance);

    instance->term = term;
    seqSetup(&instance->items, sizeof(item), 4);
}

void subTableFree(subTable *instance) {
    assert(instance);

    seqFree(&instance->items);
}

item *subTableAddItem(subTable *instance, const char *id, const char *name) {
    assert(instance);

    item newItem;
    itemInit(&newItem, id, name, instance->term);
    seqPush(&instance->items, &newItem);

    return (item *) seqLast(&instance->items);
}

void subTableRemoveItem(subTable *instance, item *item) {
    assert(instance);

    seqRemoveByPtr(&instance->items, item);
}

item *subTableFindItemById(subTable *instance, const char *id) {
    assert(instance);

    for (item *ptr=(item *) seqFirst(&instance->items), *end=(item *) seqLast(&instance->items); ptr<=end; ptr++) {
        if (!strcmp(ptr->id, id)) 
            return ptr;
    }

    return NULL;
}

item *subTableFindItemByName(subTable *instance, const char *name) {
    assert(instance);

    for (item *ptr=(item *) seqFirst(&instance->items), *end=(item *) seqLast(&instance->items); ptr<=end; ptr++) {
        if (!strcmp(ptr->name, name)) 
            return ptr;
    }

    return NULL;
}

void subTableFindItemByMark(subTable *instance, uint64_t subjectIndex, seq *res, float value, enum subTableValueCompareOp op) {
    assert(instance);

    for (item *ptr=(item *) seqFirst(&instance->items), *end=(item *) seqLast(&instance->items); ptr<=end; ptr++) {
        float v = itemGetMark(ptr, subjectIndex);
        bool pass = false;

        switch (op & (~stvco_Equal)) {
            case stvco_Larger:
                if (v > value)
                    pass = true;
                break;
            case stvco_Small:
                if (v < value)
                    pass = true;
                break;
        }
        if (op&stvco_Equal && v == value)
            pass = true;
        
        if (pass)
            seqPush(res, &ptr);
    }
}

void subTableFindItemByTotalMark(subTable *instance, seq *res, float value, enum subTableValueCompareOp op) {
    assert(instance);

    for (item *ptr=(item *) seqFirst(&instance->items), *end=(item *) seqLast(&instance->items); ptr<=end; ptr++) {
        float v = itemTotalMark(ptr);
        bool pass = false;

        switch (op & (~stvco_Equal)) {
            case stvco_Larger:
                if (v > value)
                    pass = true;
                break;
            case stvco_Small:
                if (v < value)
                    pass = true;
                break;
        }
        if (op&stvco_Equal && v == value)
            pass = true;
        
        if (pass)
            seqPush(res, &ptr);
    }
}

static itemSortContext itemSortFloatProvContext;

static float itemSortFloatProv(void *ptr) {
    item *p = (item *) ptr;

    if (itemSortFloatProvContext.isId)
        return (float) atof(p->id);
    else if (itemSortFloatProvContext.isTotalMark)
        return itemTotalMark(p);
    else
        return itemGetMark(p, itemSortFloatProvContext.subjectIndex);
}

bool subTableSortItemById(subTable *instance, enum seqSortMode mode) {
    assert(instance);

    for (item *ptr=(item *) seqFirst(&instance->items), *end=(item *) seqLast(&instance->items); ptr<=end; ptr++)
        assertWithReturn(canParseUnsignedInteger(ptr->id), false);

    itemSortFloatProvContext.isTotalMark = false;
    itemSortFloatProvContext.isId = true;

    seqSort(&instance->items, itemSortFloatProv, mode);

    return true;
}

void subTableSortItemByMark(subTable *instance, uint64_t subjectIndex, enum seqSortMode mode) {
    assert(instance);

    itemSortFloatProvContext.isTotalMark = false;
    itemSortFloatProvContext.isId = false;
    itemSortFloatProvContext.subjectIndex = subjectIndex;

    seqSort(&instance->items, itemSortFloatProv, mode);
}

void subTableSortItemByTotalMark(subTable *instance, enum seqSortMode mode) {
    assert(instance);

    itemSortFloatProvContext.isTotalMark = true;
    itemSortFloatProvContext.isId = false;

    seqSort(&instance->items, itemSortFloatProv, mode);
}

void subTableWrite(subTable *instance, csvWriter *writer) {
    assert(instance);

    for (item *ptr=(item *) seqFirst(&instance->items), *end=(item *) seqLast(&instance->items); ptr<=end; ptr++)
        itemWrite(ptr, writer);
}

void subTablePrint(subTable *instance, tablePrinter *printer) {
        assert(instance);

    for (item *ptr=(item *) seqFirst(&instance->items), *end=(item *) seqLast(&instance->items); ptr<=end; ptr++)
        itemPrint(ptr, printer);
}