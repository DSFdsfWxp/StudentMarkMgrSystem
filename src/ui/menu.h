
#ifndef _menu
#define _menu

#include <stdbool.h>
#include <stdint.h>
#include "util/seq.h"

typedef struct {
    const char *title;
    seq choices;
    seq choiceHandles;
} menu;

typedef bool (*choiceHandle)();

void menuInit(menu *instance, const char *title);
void menuFree(menu *instance);

void menuChoice(menu *instance, const char *desc, choiceHandle handle);
void menuShow(menu *instance);

#endif /* _menu */