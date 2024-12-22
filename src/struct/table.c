
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "util/assert.h"
#include "struct/table.h"

void tableInit(table *instance) {
    assert(instance);

    seqSetup(&instance->subjects, sizeof(char *), 8);
    seqSetup(&instance->subTables, sizeof(subTable), 2);
}

void tableFree(table *instance) {
    assert(instance);

    for (char **ptr=(char **) seqFirst(&instance->subjects), **end=(char **) seqLast(&instance->subjects); ptr<=end; ptr++)
        free(*ptr);

    seqFree(&instance->subjects);
    seqFree(&instance->subTables);
}

bool tableRead(table *instance, csvReader *reader) {
    assert(instance);
    assert(reader);
    assertWithReturn(reader->width >= 3, false);

    const char *headers[] = {"学号", "姓名", "学期"};
    for (uint64_t i=0; i<reader->width; i++) {
        if (i < 3) {
            assertWithReturn(!strcmp(headers[i], csvGetHeader(reader, i)), false);
        } else {
            tableAddSubject(instance, csvGetHeader(reader, i));
        }
    }

    uint64_t rowsCnt = csvGetRowsCount(reader);
    assertWithReturn(rowsCnt != (uint64_t) -1, false);

    for (uint64_t i=0; i<rowsCnt; i++) {
        char *id = csvGetValue(reader, i, 0);
        char *name = csvGetValue(reader, i, 1);
        uint8_t term = (uint8_t) atoi(csvGetValue(reader, i, 2));

        assertWithReturn(strlen(id) < 16, false);
        assertWithReturn(strlen(name) < 30, false);
        assertWithReturn(term <= 8, false);

        subTable *sTable = tableGetSubTable(instance, term);
        if (!sTable)
            sTable = tableAddSubTable(instance, term);

        item *newItem = subTableAddItem(sTable, id, name);
        for (uint64_t j=3; j<reader->width; j++)
            itemAddMark(newItem, (float) atof(csvGetValue(reader, i, j)));
    }

    return true;
}

void tableWrite(table *instance, csvWriter *writer) {
    assert(instance);
    assert(writer);

    csvWriteHeaderItem(writer, "学号");
    csvWriteHeaderItem(writer, "姓名");
    csvWriteHeaderItem(writer, "学期");

    for (char **ptr=(char **) seqFirst(&instance->subjects), **end=(char **) seqLast(&instance->subjects); ptr<=end; ptr++)
        csvWriteHeaderItem(writer, *ptr);

    csvWriteHeaderDone(writer);

    for (subTable *ptr=(subTable *) seqFirst(&instance->subTables), *end=(subTable *) seqLast(&instance->subTables); ptr<=end; ptr++)
        subTableWrite(ptr, writer);
}

void tablePrint(table *instance, tablePrinter *printer) {
    assert(instance);
    assert(printer);

    tablePrintHeaderItem(printer, "学号");
    tablePrintHeaderItem(printer, "姓名");
    tablePrintHeaderItem(printer, "学期");

    for (char **ptr=(char **) seqFirst(&instance->subjects), **end=(char **) seqLast(&instance->subjects); ptr<=end; ptr++)
        tablePrintHeaderItem(printer, *ptr);

    tablePrintHeaderItem(printer, "总分");
    tablePrintHeaderDone(printer);
}

void tablePrintAll(table *instance) {
    assert(instance);

    tablePrinter printer;
    tablePrinterBegin(&printer);
    tablePrint(instance, &printer);

    for (subTable *ptr=(subTable *) seqFirst(&instance->subTables), *end=(subTable *) seqLast(&instance->subTables); ptr<=end; ptr++)
        subTablePrint(ptr, &printer);
    
    tablePrinterDone(&printer);
}

subTable *tableGetSubTable(table *instance, uint8_t term) {
    assert(instance);

    for (subTable *ptr=(subTable *) seqFirst(&instance->subTables), *end=(subTable *) seqLast(&instance->subTables); ptr<=end; ptr++)
        if (ptr->term == term)
            return ptr;

    return NULL;
}

subTable *tableAddSubTable(table *instance, uint8_t term) {
    assert(instance);

    subTable newSubTable;
    subTableInit(&newSubTable, term);

    seqPush(&instance->subTables, &newSubTable);
    return (subTable *) seqLast(&instance->subTables);
}

void tableTryShrink(table *instance) {
    assert(instance);

    while (1) {
        bool removed = false;

        for (subTable *ptr=(subTable *) seqFirst(&instance->subTables), *end=(subTable *) seqLast(&instance->subTables); ptr<=end; ptr++) {
            if (seqSize(&ptr->items) == 0) {
                subTableFree(ptr);
                seqRemoveByPtr(&instance->subTables, ptr);
                removed = true;
                break;
            }
        }

        if (!removed)
            break;
    }
}

void tablePrintSubjects(table *instance) {
    assert(instance);

    tablePrinter printer;
    tablePrinterBegin(&printer);

    tablePrintHeaderItem(&printer, "科目");
    for (uint64_t i=1; i<4; i++)
        tablePrintHeaderItem(&printer, " ");
    tablePrintHeaderDone(&printer);

    for (char **ptr=(char **) seqFirst(&instance->subjects), **end=(char **) seqLast(&instance->subjects); ptr<=end; ptr++)
        tablePrintContentItem(&printer, *ptr);

    uint8_t rest = 4 - (seqSize(&instance->subjects) % 4);
    for (uint8_t i=0; i<rest; i++)
        tablePrintContentItem(&printer, " ");

    tablePrinterDone(&printer);
}

void tableAddSubject(table *instance, const char *subject) {
    assert(instance);

    char *subjectCopy = strdup(subject);
    seqPush(&instance->subjects, &subjectCopy);

    for (subTable *ptr=(subTable *) seqFirst(&instance->subTables), *end=(subTable *) seqLast(&instance->subTables); ptr<=end; ptr++)
        for (item *iptr=(item *) seqFirst(&ptr->items), *iend=(item *) seqLast(&ptr->items); iptr<=iend; iptr++)
            itemAddMark(iptr, 0);
}

bool tableRemoveSubject(table *instance, const char *subject) {
    assert(instance);

    uint64_t index = tableFindSubject(instance, subject);
    assertWithReturn(index != (uint64_t) -1, false);

    for (subTable *ptr=(subTable *) seqFirst(&instance->subTables), *end=(subTable *) seqLast(&instance->subTables); ptr<=end; ptr++)
        for (item *iptr=(item *) seqFirst(&ptr->items), *iend=(item *) seqLast(&ptr->items); iptr<=iend; iptr++)
            itemRemoveMark(iptr, index);

    seqRemoveByIndex(&instance->subjects, index);

    return true;
}

uint64_t tableFindSubject(table *instance, const char *subject) {
    assert(instance);

    uint64_t index = 0;
    for (char **ptr=(char **) seqFirst(&instance->subjects), **end=(char **) seqLast(&instance->subjects); ptr<=end; ptr++, index++)
        if (!strcmp(*ptr, subject))
            return index;

    return -1;
}