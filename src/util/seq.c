
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "util/assert.h"
#include "util/memory.h"
#include "util/seq.h"

void seqSetup(seq *instance, uint64_t elemSize, uint64_t increasedElemsPerTime) {
    assert(instance);

    instance->elemCnt = 0;
    instance->realElemCnt = increasedElemsPerTime;
    instance->elemSize = elemSize;
    instance->increasedElemsPerTime = increasedElemsPerTime;
    instance->pool = malloc(elemSize * increasedElemsPerTime);

    assert(instance->pool);
}

void seqFree(seq *instance) {
    assert(instance);

    if (instance->pool)
        free(instance->pool);
    memset(instance, 0, sizeof(seq));
}

static void seqCheck(seq *instance, bool checkPool) {
    assert(instance);
    if (checkPool)
        assert(instance->pool);
}

void *seqFirst(seq *instance) {
    seqCheck(instance, true);

    return instance->pool;
}

void *seqLast(seq *instance) {
    seqCheck(instance, true);

    if (instance->elemCnt == 0)
        return NULL;
    return (char *) instance->pool + (instance->elemCnt - 1)*instance->elemSize;
}

uint64_t seqSize(seq *instance) {
    seqCheck(instance, false);

    return instance->pool ? instance->elemCnt : 0;
}

void *seqAt(seq *instance, uint64_t index) {
    seqCheck(instance, true);
    assert(index < instance->elemCnt);

    return (char *) instance->pool + index*instance->elemSize;
}

static void seqCheckExpand(seq *instance) {
    if (instance->elemCnt < instance->realElemCnt)
        return;

    instance->realElemCnt += instance->increasedElemsPerTime;
    instance->pool = realloc(instance->pool, instance->realElemCnt * instance->elemSize);
    assert(instance->pool);
}

static void seqCheckShrink(seq *instance) {
    if (instance->realElemCnt == instance->increasedElemsPerTime ||
        instance->elemCnt > instance->realElemCnt-instance->increasedElemsPerTime)
            return;

    instance->realElemCnt -= instance->increasedElemsPerTime;
    instance->pool = realloc(instance->pool, instance->realElemCnt * instance->elemSize);
    assert(instance->pool);
}

void seqPush(seq *instance, void *elem) {
    seqCheck(instance, true);

    memcpy((char *) instance->pool + instance->elemCnt*instance->elemSize, elem, instance->elemSize);
    instance->elemCnt ++;
    seqCheckExpand(instance);
}

void seqUnshift(seq *instance, void *elem) {
    seqCheck(instance, true);

    orderedMemcpyR(seqAt(instance, 1), instance->pool, instance->elemCnt * instance->elemSize);
    memcpy(instance->pool, elem, instance->elemSize);
    instance->elemCnt ++;
    seqCheckExpand(instance);
}

void seqPop(seq *instance, void *dest) {
    seqCheck(instance, true);

    if (dest)
        memcpy(dest, seqAt(instance, instance->elemCnt - 1), instance->elemSize);
    instance->elemCnt --;
    seqCheckShrink(instance);
}

void seqShift(seq *instance, void *dest) {
    seqCheck(instance, true);

    if (dest)
        memcpy(dest, instance->pool, instance->elemSize);
    orderedMemcpyL(instance->pool, seqAt(instance, 1), (instance->elemCnt - 1) * instance->elemSize);
    instance->elemCnt --;
    seqCheckShrink(instance);
}

void seqRemoveByIndex(seq *instance, uint64_t index) {
    seqCheck(instance, true);
    assert(index < instance->elemCnt);

    orderedMemcpyL((char *) instance->pool + index*instance->elemSize, (char *) instance->pool + (index + 1)*instance->elemSize, (instance->elemCnt - index - 1) * instance->elemSize);
    instance->elemCnt --;
    seqCheckShrink(instance);
}

void seqRemoveByPtr(seq *instance, void *ptr) {
    seqCheck(instance, true);
    assert(ptr >= instance->pool && ptr <= seqLast(instance));

    uint64_t delta = (uint64_t) ptr - (uint64_t) instance->pool;
    assert(delta % instance->elemSize == 0);

    seqRemoveByIndex(instance, delta / instance->elemSize);
}

void seqClear(seq *instance) {
    seqCheck(instance, true);

    instance->elemCnt = 0;
    instance->realElemCnt = instance->increasedElemsPerTime;
    
    instance->pool = realloc(instance->pool, instance->realElemCnt * instance->elemSize);
    assert(instance->pool);
}

void seqSort(seq *instance, floatProv elemToFloat, enum seqSortMode mode) {
    seqCheck(instance, true);

    for (uint64_t i=0; i<instance->elemCnt; i++) {
        float a = elemToFloat(seqAt(instance, i));
        for (uint64_t j=i+1; j<instance->elemCnt; j++) {
            float b = elemToFloat(seqAt(instance, j));

            if (mode == seqSort_SmallToBig && b < a ||
                mode == seqSort_BigToSmall && b > a) {
                char buff[instance->elemSize];
                memcpy(buff, seqAt(instance, i), instance->elemSize);
                memcpy(seqAt(instance, i), seqAt(instance, j), instance->elemSize);
                memcpy(seqAt(instance, j), buff, instance->elemSize);
            }
        }
    }
}