#ifndef LAB1LLP_TABLE_INDEX_H
#define LAB1LLP_TABLE_INDEX_H

#include <stdint.h>
#include "../../consts/table_consts.h"

struct TableIndex {
    uint32_t table_name_hash;
    uint32_t schema_sector;
};

struct TableIndexArray {
    uint32_t next_table_sector;
    struct TableIndex table_map[MAX_TABLES_COUNT];
};

#endif //LAB1LLP_TABLE_INDEX_H
