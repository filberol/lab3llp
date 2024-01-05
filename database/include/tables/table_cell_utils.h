#ifndef LAB1LLP_TABLE_CELL_UTILS_H
#define LAB1LLP_TABLE_CELL_UTILS_H

#include <string.h>
#include "../../include/types/schema/table_schema.h"
#include "../../include/utils/file_string_utils.h"

int row_set_cell_value(FILE* file, const struct TableScheme* scheme, union TableCellWithData* rows, int col_num, enum CellType cell_type, void* value);
void print_cell_value(FILE* file, enum CellType cell_type, void *value);

#endif //LAB1LLP_TABLE_CELL_UTILS_H
