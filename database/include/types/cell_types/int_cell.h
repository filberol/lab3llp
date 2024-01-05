#ifndef LAB1LLP_INT_CELL_H
#define LAB1LLP_INT_CELL_H

#include <stdint.h>
#include "./cell.h"

struct IntTableCell {
    struct TableCell meta;
    int32_t value;
};

#endif //LAB1LLP_INT_CELL_H
