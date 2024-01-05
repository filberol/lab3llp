#ifndef LAB1LLP_TABLE_RAW_UTILS_H
#define LAB1LLP_TABLE_RAW_UTILS_H

#include "../../include/types/schema/table_schema.h"
#include "../../include/sectors/sector_utils.h"

size_t add_row_to_file(FILE* file, struct TableScheme* scheme, union TableCellWithData *row);
int delete_row_from_file(FILE* file, uint32_t row_number, struct TableScheme* scheme);
int get_row_by_number(FILE* file, uint32_t row_number, struct TableScheme* scheme, union TableCellWithData* row);

#endif //LAB1LLP_TABLE_RAW_UTILS_H
