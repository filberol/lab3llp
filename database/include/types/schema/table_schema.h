#ifndef LAB1LLP_TABLE_SCHEMA_H
#define LAB1LLP_TABLE_SCHEMA_H

#include <stdint.h>
#include "../../consts/table_consts.h"

#include "../cell_types/bigint_cell.h"
#include "../cell_types/bool_cell.h"
#include "../cell_types/float_cell.h"
#include "../cell_types/int_cell.h"
#include "../cell_types/string_cell.h"

union TableCellWithData {
    struct BigIntTableCell bigint_c;
    struct BoolTableCell bool_c;
    struct FloatTableCell float_c;
    struct IntTableCell int_c;
    struct StringTableCell string_c;
};

struct TableScheme {
    char name[TABLE_MAX_NAME_LENGTH];
    int columns_count;
    uint32_t columns_info_sector;
    uint32_t data_first_sector;
    uint32_t data_last_sector;
    uint32_t own_sector;
};

#endif //LAB1LLP_TABLE_SCHEMA_H
