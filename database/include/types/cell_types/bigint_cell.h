#ifndef LAB1LLP_BIGINT_CELL_H
#define LAB1LLP_BIGINT_CELL_H

#include "cell.h"

struct BigIntTableCell {
    struct TableCell meta;
    int64_t value;
};

#endif //LAB1LLP_BIGINT_CELL_H
