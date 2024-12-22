
#ifndef _data
#define _data

#include <stdbool.h>
#include "struct/table.h"

extern table tableStruct;

bool dataLoad(const char *path);
bool dataSave(const char *path);
void dataFree();

#endif /* _data */