
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#if defined(_WIN32) || defined(_WIN64)
#   include <windows.h>
#endif /* _WIN32 || _WIN64 */

#include "util/stringUtil.h"

// 判断一个UTF-8编码的字符是否为全角字符或全角标点符号
static bool isFullWidthCharacter(uint32_t codepoint) {
    // 包括全角ASCII字符和一些全角符号及货币符号
    return (codepoint >= 0xFF01 && codepoint <= 0xFF60) || 
           (codepoint >= 0xFFE0 && codepoint <= 0xFFE6) ||
           (codepoint >= 0x4E00 && codepoint <= 0x9FA5);
}

// 将UTF-8编码的字符转换为Unicode码点
static uint32_t utf8ToCodePoint(const char **str) {
    uint32_t codePoint = 0;
    unsigned char byte = *((*str)++);
    
    if ((byte & 0x80) == 0) { // 1-byte sequence
        codePoint = byte;
    } else if ((byte & 0xE0) == 0xC0) { // 2-byte sequence
        codePoint = (byte & 0x1F) << 6 | ((*(*str)++) & 0x3F);
    } else if ((byte & 0xF0) == 0xE0) { // 3-byte sequence
        codePoint = (byte & 0x0F) << 12 | ((*(*str)++) & 0x3F) << 6 | ((*(*str)++) & 0x3F);
    } else if ((byte & 0xF8) == 0xF0) { // 4-byte sequence
        codePoint = (byte & 0x07) << 18 | ((*(*str)++) & 0x3F) << 12 | ((*(*str)++) & 0x3F) << 6 | ((*(*str)++) & 0x3F);
    }
    return codePoint;
}

// 统计UTF-8字符串中的全角字符（包括全角标点符号）个数
uint64_t fullWidthCharCount(const char *txt) {
    uint64_t count = 0;
    while (*txt) {
        uint32_t codePoint = utf8ToCodePoint(&txt);
        if (isFullWidthCharacter(codePoint)) {
            ++ count;
        }
    }
    return count;
}

uint64_t utf8Strlen(const char *txt) {
    uint64_t cnt = 0;
    while (*txt) {
        utf8ToCodePoint(&txt);
        ++ cnt;
    }
    return cnt;
}

const char *ftos(float v) {
    static char buff[32] = {0};
    sprintf(buff, "%g", v);
    return buff;
}

const char *itos(int v) {
    static char buff[32] = {0};
    sprintf(buff, "%d", v);
    return buff;
}

bool canParseUnsignedInteger(const char *txt) {
    for (uint64_t i=0; txt[i]; i++) {
        if (txt[i] < '0' || txt[i] > '9')
            return false;
    }
    return true;
}

#if defined(_WIN32) || defined(_WIN64)

char *ansiToUtf8(const char *ansiStr) {
    // 首先将ANSI字符串转换为宽字符（wchar_t）
    int wideCharCnt = MultiByteToWideChar(CP_ACP, 0, ansiStr, -1, NULL, 0);
    wchar_t* wideStr = (wchar_t*) malloc(wideCharCnt * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, ansiStr, -1, wideStr, wideCharCnt);

    // 然后将宽字符转换为UTF-8编码
    int utf8CharCount = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);
    char* utf8Str = (char*) malloc(utf8CharCount + 1);
    WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, utf8Str, utf8CharCount, NULL, NULL);
    utf8Str[utf8CharCount] = 0;

    free(wideStr); // 释放宽字符字符串分配的内存

    return utf8Str;
}

char *utf8ToAnsi(const char *utf8Str) {
    // 首先将UTF-8字符串转换为宽字符（wchar_t）
    int wideCharCnt = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, NULL, 0);
    wchar_t* wideStr = (wchar_t*) malloc(wideCharCnt * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, wideStr, wideCharCnt);

    // 然后将宽字符转换为ANSI编码
    int ansiCharCnt = WideCharToMultiByte(CP_ACP, 0, wideStr, -1, NULL, 0, NULL, NULL);
    char* ansiStr = (char*) malloc(ansiCharCnt + 1);
    WideCharToMultiByte(CP_ACP, 0, wideStr, -1, ansiStr, ansiCharCnt, NULL, NULL);
    ansiStr[ansiCharCnt] = 0;

    free(wideStr); // 释放宽字符字符串分配的内存

    return ansiStr;
}

#endif /* _WIN32 || _WIN64 */