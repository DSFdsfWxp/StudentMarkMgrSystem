
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "util/assert.h"
#include "util/data.h"
#include "util/seq.h"
#include "struct/item.h"
#include "struct/subTable.h"
#include "struct/table.h"
#include "io/tablePrinter.h"
#include "ui/input.h"
#include "ui/findMenu.h"

menu findMenu;
static seq resultItems;

static bool parseCompareCondition(const char *txt, enum subTableValueCompareOp *op, float *value) {
    int numOp = 0;
    
    for (uint64_t i=0; txt[i]; i++) {
        switch (txt[i]) {
            case '>':
                assertWithReturn(!(numOp & (~stvco_Equal)), false);
                numOp |= stvco_Larger;
                break;
            case '<':
                assertWithReturn(!(numOp & (~stvco_Equal)), false);
                numOp |= stvco_Small;
                break;
            case '=':
                assertWithReturn(!(numOp & stvco_Equal), false);
                numOp |= stvco_Equal;
                break;
            default:
                assertWithReturn(numOp > 0, false);
                for (uint64_t j=i; txt[j]; j++)
                    assertWithReturn((txt[j] >= '0' && txt[j] <= '9') || txt[j] == '.', false);

                assertWithReturn(sscanf(txt + i, "%f", value) == 1, false);
                *op = (enum subTableValueCompareOp) numOp;
                return true;
        }
    }

    return false;
}

static bool findByMark() {
    int term = inputInteger("输入学期 (范围: 1-8)", 1, 8);
    subTable *sTable = tableGetSubTable(&tableStruct, term);
    assertWithLogReturn(sTable, "此学期无记录.\n", true);

    char txt[1024];
    inputString("输入科目", txt, 1024);
    uint64_t subjectIndex = tableFindSubject(&tableStruct, txt);
    assertWithLogReturn(subjectIndex != (uint64_t) -1, "科目未找到.", true);

    inputString("输入条件 (如 >=60, =100, <60)", txt, 1024);
    enum subTableValueCompareOp op;
    float value;
    assertWithLogReturn(parseCompareCondition(txt, &op, &value), "不正确的条件.", true);

    subTableFindItemByMark(sTable, subjectIndex, &resultItems, value, op);
    printf("操作完成.\n");
    return true;
}

static bool findByTotalMark() {
    int term = inputInteger("输入学期 (范围: 1-8)", 1, 8);
    subTable *sTable = tableGetSubTable(&tableStruct, term);
    assertWithLogReturn(sTable, "此学期无记录.\n", true);

    char txt[1024];
    inputString("输入条件 (如 >=60, =100, <60)", txt, 1024);
    enum subTableValueCompareOp op;
    float value;
    assertWithLogReturn(parseCompareCondition(txt, &op, &value), "不正确的条件.", true);

    subTableFindItemByTotalMark(sTable, &resultItems, value, op);
    printf("操作完成.\n");
    return true;
}

static bool findByInfo() {
    char txt[1024];
    inputString("输入学号或姓名", txt, 1024);

    for (uint8_t i=1; i<=8; i++) {
        subTable *sTable = tableGetSubTable(&tableStruct, i);
        if (!sTable)
            continue;
        
        item *target = subTableFindItemById(sTable, txt);
        if (!target)
            target = subTableFindItemByName(sTable, txt);

        if (target)
            seqPush(&resultItems, &target);
    }

    printf("操作完成.\n");
    return true;
}

static bool clearResult() {
    seqClear(&resultItems);
    printf("操作完成.\n");
    return true;
}

static bool printResult() {
    tablePrinter printer;
    tablePrinterBegin(&printer);
    tablePrint(&tableStruct, &printer);
    for (item **ptr=(item **) seqFirst(&resultItems), **end=(item **) seqLast(&resultItems); ptr<=end; ptr++)
        itemPrint(*ptr, &printer);
    tablePrinterDone(&printer);
    return true;
}

static bool back() {
    seqClear(&resultItems);
    return false;
}

void findMenuInit() {
    seqSetup(&resultItems, sizeof(item *), 8);
    menuInit(&findMenu, "查找数据");
    menuChoice(&findMenu, "通过单科成绩查找", findByMark);
    menuChoice(&findMenu, "通过总分查找", findByTotalMark);
    menuChoice(&findMenu, "通过学生信息查找", findByInfo);
    menuChoice(&findMenu, "列出结果", printResult);
    menuChoice(&findMenu, "清除结果", clearResult);
    menuChoice(&findMenu, "返回", back);
}

void findMenuFree() {
    seqFree(&resultItems);
    menuFree(&findMenu);
}