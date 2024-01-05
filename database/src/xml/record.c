//
// Created by moyak on 14.09.2023.
//
#include <stdlib.h>
#include "../../include/aggregator/public/database.h"
#include "../../include/aggregator/public/table_structures.h"

struct RecordData *prepareRecordDataStructure(struct TableScheme *tableHeader) {
    struct RecordData *recordData = malloc(sizeof(struct RecordData));
    void **data = calloc(tableHeader->columns_count, sizeof(void *));
    recordData->columnNum = tableHeader->columns_count;
    recordData->data = data;
    recordData->unreadData = NULL;
    recordData->rowPageBlock = NULL;
    recordData->columns = NULL;
    return recordData;
}

void freeRecordData(struct RecordData *recordData) {
    free(recordData->data);
    free(recordData->columns);
//    unmapPage(recordData->rowPageBlock);
    free(recordData);
}

void clearRecordDataToReadFromBegin(struct RecordData *recordData) {
    recordData->unreadData = NULL;
    recordData->rowPageBlock = NULL;
}


