
#ifndef _fullWidthChar
#define _fullWidthChar

#include <stdbool.h>
#include <stdint.h>

uint64_t fullWidthCharCount(const char *txt);
uint64_t utf8Strlen(const char *txt);

const char *ftos(float v);
const char *itos(int v);

bool canParseUnsignedInteger(const char *txt);


#if defined(_WIN32) || defined(_WIN64)

char *ansiToUtf8(const char *ansiStr);
char *utf8ToAnsi(const char *utf8Str);

#endif /* _WIN32 || _WIN64 */

#endif /* _fullWidthChar */