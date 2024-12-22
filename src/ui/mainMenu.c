
#include <stdint.h>
#include <stdbool.h>
#include <float.h>
#include <string.h>
#include "io/tablePrinter.h"
#include "util/data.h"
#include "util/assert.h"
#include "util/seq.h"
#include "util/stringUtil.h"
#include "struct/subTable.h"
#include "struct/item.h"
#include "struct/table.h"
#include "ui/input.h"
#include "ui/findMenu.h"
#include "ui/mainMenu.h"

menu mainMenu;

static bool listMark() {
    char txt[8];
    
    while (1) {
        inputString("输入学期 (范围: 1-8, * 代表全部)", txt, 8);

        if (!strcmp(txt, "*")) {
            putchar('\n');
            tablePrintAll(&tableStruct);
        } else if (!canParseUnsignedInteger(txt)) {
            printf("无效输入.\n");
            continue;
        } else {
            int term = atoi(txt);
            if (term > 8 || term == 0) {
                printf("无效输入.\n");
                continue;
            }

            subTable *target = tableGetSubTable(&tableStruct, term);
            if (!target) {
                printf("该学期无数据.\n");
                break;
            }

            putchar('\n');
            tablePrinter printer;
            tablePrinterBegin(&printer);
            tablePrint(&tableStruct, &printer);
            subTablePrint(target, &printer);
            tablePrinterDone(&printer);
        }

        break;
    }

    return true;
}

static bool addMark() {
    printf("提示: 可以连续输入, 输入学期为 0 可停止输入.\n");

    while (1) {
        char id[16], name[30];
        int term = inputInteger("输入学期 (范围: 1-8)", 0, 8);
        if (term == 0)
            break;
        inputString("输入学号", id, 16);
        inputString("输入姓名", name, 30);

        subTable *sTable = tableGetSubTable(&tableStruct, term);
        if (!sTable)
            sTable = tableAddSubTable(&tableStruct, term);

        item *newItem = subTableAddItem(sTable, id, name);
        uint64_t index = 0;
        for (char **ptr= (char **) seqFirst(&tableStruct.subjects), **end=(char **) seqLast(&tableStruct.subjects); ptr<=end; ptr++, index++)
            itemAddMark(newItem, inputFloatWithFormat("输入 %s 成绩", 0, FLT_MAX, *(char **) seqAt(&tableStruct.subjects, index)));

        printf("操作完成.\n");
    }

    return true;
}

static bool modifyMark() {
    char txt[1024];
    int term = inputInteger("输入学期 (范围: 1-8)", 1, 8);
    subTable *sTable = tableGetSubTable(&tableStruct, term);
    item *target = NULL;

    if (sTable)
        printf("提示: 可连续修改某人成绩, 输入 -1 作为学号或姓名可停止.\n");
    while (1) {
        if (sTable) {
            inputString("输入学号或姓名", txt, 1024);
            if (!strcmp(txt, "-1"))
                break;

            target = subTableFindItemById(sTable, txt);
            if (!target)
                target = subTableFindItemByName(sTable, txt);

            if (target) {
                putchar('\n');
                tablePrinter printer;
                tablePrinterBegin(&printer);
                tablePrint(&tableStruct, &printer);
                itemPrint(target, &printer);
                tablePrinterDone(&printer);

                putchar('\n');
                printf("提示: 可连续修改该人不同科成绩, 输入 -1 作为科目可停止.\n");
                while (1) {
                    inputString("输入科目", txt, 1024);
                    if (!strcmp(txt, "-1"))
                        break;
                    uint64_t subjectIndex = tableFindSubject(&tableStruct, txt);

                    if (subjectIndex != (uint64_t) -1) {
                        itemModifyMark(target, subjectIndex, inputFloatWithFormat("输入成绩", 0, FLT_MAX));
                        printf("操作完成.\n");
                        continue;
                    }

                    printf("未找到该科目.\n");
                }

                continue;
            }
        }

        printf("未找到记录.\n");
        if (!sTable)
            break;
    }

    return true;
}

static bool removeMark() {
    char txt[1024];

    int term = inputInteger("输入学期 (范围: 1-8)", 1, 8);
    subTable *sTable = tableGetSubTable(&tableStruct, term);
    assertWithLogReturn(sTable, "该学期没有数据.", true);

    printf("提示: 可连续删除成绩, 输入 -1 作为学号或姓名可停止.\n");
    while (1) {
        inputString("输入学号或姓名", txt, 1024);
        if (!strcmp(txt, "-1"))
            break;

        item *target = subTableFindItemById(sTable, txt);
        if (!target)
            target = subTableFindItemByName(sTable, txt);

        if (!target) {
            printf("未找到记录.\n");
            continue;
        }

        subTableRemoveItem(sTable, target);
        tableTryShrink(&tableStruct);
        printf("操作完成.\n");
    }

    return true;
}

static bool listSubject() {
    tablePrintSubjects(&tableStruct);
    return true;
}

static bool addSubject() {
    char buff[1024];
    printf("提示: 可连续输入, 输入科目名称 -1 可停止输入.\n");

    while (1) {
        inputString("输入科目名称", buff, 1024);

        if (!strcmp(buff, "-1"))
            break;

        tableAddSubject(&tableStruct, buff);

        printf("操作完成.\n");
    }

    return true;
}

static bool removeSubject() {
    char buff[1024];
    printf("提示: 可连续删除, 输入科目名称 -1 可停止删除.\n");

    while (1) {
        inputString("输入科目名称", buff, 1024);

        if (!strcmp(buff, "-1"))
            break;

        if (tableRemoveSubject(&tableStruct, buff))
            printf("操作完成.\n");
        else
            printf("错误: 科目不存在.\n");
    }

    return true;
}

static bool modifyStudentInfo() {
    char buff[1024];
    char id[16];
    char name[30];
    printf("提示: 可连续修改, 输入 -1 作为学号或姓名可停止修改.\n");

    while (1) {
        inputString("输入学号或姓名", buff, 1024);
        if (!strcmp(buff, "-1"))
            break;
        inputString("输入新学号 (留空表示不修改)", id, 16);
        inputString("输入新姓名 (留空表示不修改)", name, 30);

        unsigned int cnt = 0;
        for (uint8_t i=1; i<=8; i++) {
            subTable *sTable = tableGetSubTable(&tableStruct, i);
            if (!sTable)
                continue;

            item *target = subTableFindItemById(sTable, buff);
            if (!target)
                target = subTableFindItemByName(sTable, buff);

            if (!target)
                continue;

            cnt ++;
            if (strlen(id) > 0)
                strcpy(target->id, id);
            if (strlen(name) > 0)
                strcpy(target->name, name);
        }
        printf("修改了 %u 条记录.\n", cnt);
    }

    return true;
}

static bool sortData() {
    char txt[1024];
    int term = inputInteger("输入学期 (范围: 1-8)", 1, 8);
    subTable *sTable = tableGetSubTable(&tableStruct, term);
    assertWithLogReturn(sTable, "该学期没有数据.", true);

    inputString("输入排序依据 (学号/总分/科目名称)", txt, 1024);
    enum seqSortMode op = (enum seqSortMode) inputInteger("输入排序方法 (0 小到大, 1 大到小)", 0, 1);

    if (!strcmp(txt, "学号")) {
        assertWithLogReturn(subTableSortItemById(sTable, op), "学号非纯数字, 无法排序.", true);
    } else if (!strcmp(txt, "总分")) {
        subTableSortItemByTotalMark(sTable, op);
    } else {
        uint64_t subjectIndex = tableFindSubject(&tableStruct, txt);
        if (subjectIndex == (uint64_t) -1) {
            printf("未找到该科目.\n");
            return true;
        }

        subTableSortItemByMark(sTable, subjectIndex, op);
    }

    printf("操作完成.\n");
    return true;
}

static bool findData() {
    menuShow(&findMenu);
    return true;
}

static bool saveToFile() {
    assertWithLogReturn(dataSave("data.csv"), "数据保存失败.", true);
    printf("操作完成.\n");
    return true;
}

static bool exitSystem() {
    return false;
}

void mainMenuInit() {
    menuInit(&mainMenu, "学生成绩管理系统");
    menuChoice(&mainMenu, "列出成绩", listMark);
    menuChoice(&mainMenu, "添加成绩", addMark);
    menuChoice(&mainMenu, "修改成绩", modifyMark);
    menuChoice(&mainMenu, "删除成绩", removeMark);
    menuChoice(&mainMenu, "列出科目", listSubject);
    menuChoice(&mainMenu, "添加科目", addSubject);
    menuChoice(&mainMenu, "删除科目", removeSubject);
    menuChoice(&mainMenu, "变更学生信息", modifyStudentInfo);
    menuChoice(&mainMenu, "排序数据", sortData);
    menuChoice(&mainMenu, "查找数据", findData);
    menuChoice(&mainMenu, "保存", saveToFile);
    menuChoice(&mainMenu, "退出", exitSystem);
}

void mainMenuFree() {
    menuFree(&mainMenu);
}