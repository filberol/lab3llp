#ifndef LAB1LLP_STRING_CELL_H
#define LAB1LLP_STRING_CELL_H

#include <stdint.h>
#include "cell.h"

struct StringTableCell {
    struct TableCell meta;
    uint32_t string_size;
    uint32_t string_value_sector;
};

#endif //LAB1LLP_STRING_CELL_H
