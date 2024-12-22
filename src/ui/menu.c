
#include <stdio.h>
#include "ui/input.h"
#include "util/assert.h"
#include "ui/menu.h"

void menuInit(menu *instance, const char *title) {
    assert(instance);

    seqSetup(&instance->choices, sizeof(const char *), 8);
    seqSetup(&instance->choiceHandles, sizeof(choiceHandle), 8);
    instance->title = title;
}

void menuFree(menu *instance) {
    assert(instance);

    seqFree(&instance->choices);
    seqFree(&instance->choiceHandles);
}

void menuChoice(menu *instance, const char *desc, choiceHandle handle) {
    assert(instance);

    seqPush(&instance->choices, &desc);
    seqPush(&instance->choiceHandles, &handle);
}

void menuShow(menu *instance) {
    assert(instance);

    while (1) {
        printf("\n%s\n\n===\n\n", instance->title);

        int index = 0;
        for (char **ptr=(char **) seqFirst(&instance->choices), **end=(char **) seqLast(&instance->choices); ptr<=end; ptr++, index++)
            printf(" [%.2d] %s\n", index, *ptr);

        printf("\n===\n\n");

        int res = inputInteger("你的选择", 0, index - 1);
        putchar('\n');

        choiceHandle handle = *(choiceHandle *) seqAt(&instance->choiceHandles, res);
        if (!handle())
            break;

        waitForEnter();
        printf("\n\n");
    }
}