
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include "util/stringUtil.h"
#include "ui/input.h"

int inputInteger(const char *prompt, int min, int max) {
    fseek(stdin, 0, SEEK_END);
    while (1) {
        printf("%s: ", prompt);
        fflush(stdout);
        int res;
        if (scanf("%d", &res) == 1 && res >= min && res <= max)
            return res;
        printf("无效输入. \n");
        fseek(stdin, 0, SEEK_END);
    }
}

float inputFloatWithFormat(const char *prompt, float min, float max, ...) {
    fseek(stdin, 0, SEEK_END);
    while (1) {
        va_list list;
        va_start(list, max);
        vprintf(prompt, list);
        va_end(list);
        printf(": ");
        fflush(stdout);
        float res;
        if (scanf("%f", &res) == 1 && res >= min && res <= max)
            return res;
        printf("无效输入. \n");
        fseek(stdin, 0, SEEK_END);
    }
}

void inputString(const char *prompt, char *buff, uint64_t buffSize) {
    fseek(stdin, 0, SEEK_END);
    char tmpBuff[buffSize];

    while (1) {
        printf("%s: ", prompt);
        fflush(stdout);

        fgets(tmpBuff, buffSize, stdin);
        uint64_t len = strlen(tmpBuff);

        bool pass = false;
        for (uint64_t i=len-1; i<len; i--) {
            if (tmpBuff[i] == '\n' || tmpBuff[i] == '\r') {
                pass = true;
                tmpBuff[i] = 0;
            }
        }

        #if defined(_WIN32) || defined(_WIN64)

        if (pass) {
            char *utf8Str = ansiToUtf8(tmpBuff);

            if (strlen(utf8Str) < buffSize) {
                strcpy(buff, utf8Str);
                free(utf8Str);
            } else {
                pass = false;
            }
        }

        #else /* _WIN32 || _WIN64 */

        if (pass)
            strcpy(buff, tmpBuff);

        #endif /* UNIX */

        if (pass)
            return;

        printf("输入过长.\n");
        fseek(stdin, 0, SEEK_END);
    }
}

void waitForEnter() {
    printf("\n按下 ENTER 继续.");
    fflush(stdout);

    fseek(stdin, 0, SEEK_END);
    fgetc(stdin);
    putchar('\n');
}