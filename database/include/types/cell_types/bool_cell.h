#ifndef LAB1LLP_BOOL_CELL_H
#define LAB1LLP_BOOL_CELL_H

#include <stdbool.h>
#include "cell.h"

struct BoolTableCell {
    struct TableCell meta;
    bool value;
};

#endif //LAB1LLP_BOOL_CELL_H
