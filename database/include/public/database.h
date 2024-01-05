#ifndef LAB1_DATABASE_H
#define LAB1_DATABASE_H

#include <stdint.h>
#include "../util/public/types.h"
#include "../util/public/predicate.h"

struct __attribute__((__packed__)) Column {
    enum DataType dataType;
    char name[];
};

struct __attribute__((__packed__)) RecordData {
    int32_t columnNum;
    struct BlockHeader *rowPageBlock;
    char *unreadData;
    struct Column **columns;
    void **data;
};


#endif //LAB1_DATABASE_H

