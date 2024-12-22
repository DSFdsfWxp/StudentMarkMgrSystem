
#ifndef _input
#define _input

#include <stdint.h>

int inputInteger(const char *prompt, int min, int max);
float inputFloatWithFormat(const char *prompt, float min, float max, ...);
void inputString(const char *prompt, char *buff, uint64_t buffSize);

void waitForEnter();

#endif /* _input */