//
// Created by moyak on 24.09.2023.
//

#ifndef LLP_TABLE_STUCTURES_PUBLIC_H
#define LLP_TABLE_STUCTURES_PUBLIC_H

#include <stdint.h>
#include <stdbool.h>
#include "../types/schema/table_schema.h"

struct __attribute__((__packed__)) BlockCoordinate {
    uint64_t offset;
    uint64_t size;
};

struct __attribute__((__packed__)) BlockHeader {
    uint64_t offset;
    uint64_t size;
    uint64_t previousBlockOffset;
    bool isFree;
};

struct __attribute__((__packed__)) RowPage {
    struct BlockCoordinate nextRowPage;
    struct BlockCoordinate prevRowPage;
    int32_t rowCount;
    uint64_t freeSpaceOffsetStart;
    uint64_t freeSpaceOffsetEnd;

};

struct __attribute__((__packed__)) RowPointer {
    bool isToDelete;
    uint64_t offset; // ������������ RowPage
};

struct __attribute__((__packed__)) TableHeader {
    uint64_t offsetFromBlockBegin;
    struct BlockCoordinate firstRowPage;
    int32_t columnCount;
    int32_t rowCount;

};
struct __attribute__((__packed__)) FileNode {
    struct BlockCoordinate nextTableHeader;
    struct BlockCoordinate prevTableHeader;
    char name[];
};

struct __attribute__((__packed__)) FreeListNode {
    struct BlockCoordinate nextTableHeader;
    struct BlockCoordinate prevTableHeader;
};

struct RecordData *prepareRecordDataStructure(struct TableScheme *tableHeader);

void freeRecordData(struct RecordData *recordData);

void clearRecordDataToReadFromBegin(struct RecordData *recordData);


#endif //LLP_TABLE_STUCTURES_PUBLIC_H
