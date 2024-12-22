
#ifndef _seq
#define _seq

#include <stdint.h>
#include "util/providers.h"

typedef struct {
    uint64_t elemSize;
    uint64_t elemCnt;
    uint64_t realElemCnt;
    uint64_t increasedElemsPerTime;
    void *pool;
} seq;

enum seqSortMode {
    seqSort_SmallToBig,
    seqSort_BigToSmall
};

void seqSetup(seq *instance, uint64_t elemSize, uint64_t increasedElemsPerTime);
void seqFree(seq *instance);

void *seqFirst(seq *instance);
void *seqLast(seq *instance);
uint64_t seqSize(seq *instance);

void *seqAt(seq *instance, uint64_t index);

void seqPush(seq *instance, void *elem);
void seqUnshift(seq *instance, void *elem);

void seqPop(seq *instance, void *dest);
void seqShift(seq *instance, void *dest);
void seqRemoveByIndex(seq *instance, uint64_t index);
void seqRemoveByPtr(seq *instance, void *ptr);
void seqClear(seq *instance);

void seqSort(seq *instance, floatProv elemToFloat, enum seqSortMode mode);

#endif /* _seq */