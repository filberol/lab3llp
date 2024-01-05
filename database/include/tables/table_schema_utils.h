#ifndef LAB1LLP_TABLE_SCHEMA_UTILS_H
#define LAB1LLP_TABLE_SCHEMA_UTILS_H

#include <string.h>
#include <stdlib.h>
#include "../types/schema/table_schema.h"
#include "../sectors/sector_utils.h"
#include "../consts/hash_consts.h"
#include "../../include/tables/table_index_utils.h"

int init_table_scheme(FILE* file, const char *table_name, int column_count, struct TableScheme* empty_scheme);
int schema_set_column(FILE* file, struct TableScheme* scheme, int col_num, enum CellType col_type, const char* column_name);

#endif //LAB1LLP_TABLE_SCHEMA_UTILS_H
