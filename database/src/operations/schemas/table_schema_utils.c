#include "../../../include/tables/table_schema_utils.h"

/*
 * Returns
 * -1 - wrong args
 * number - sector for scheme
 */
int init_table_scheme(
        FILE* file, const char *table_name, int column_count, struct TableScheme* empty_scheme
) {
    if (column_count > TABLE_MAX_COLUMNS) return -1;
    if (strlen(table_name) > TABLE_MAX_NAME_LENGTH) return -1;

    size_t column_headers_buff_size = sizeof(struct HeaderCell) * column_count;
    size_t sectors_for_table_schema = calculate_sectors_needed(sizeof(struct TableScheme));
    size_t sectors_for_column_headers = calculate_sectors_needed(column_headers_buff_size);
    uint32_t table_scheme_sector = allocate_sector(file, sectors_for_table_schema);
    uint32_t table_headers_sector = allocate_sector(file, sectors_for_column_headers);

    strcpy(empty_scheme->name, table_name);
    empty_scheme->columns_count = column_count;
    empty_scheme->columns_info_sector = table_headers_sector;
    empty_scheme->own_sector = table_scheme_sector;
    empty_scheme->data_first_sector = TABLE_INDEX_HASH_EMPTY; // Empty rows
    empty_scheme->data_last_sector = TABLE_INDEX_HASH_EMPTY;

    struct HeaderCell* empty_columns = malloc(column_headers_buff_size);
    struct HeaderCell empty_cell = {
            .column_name = "NaN", .meta = {TABLE_TYPE_EMPTY}
    };
    for (int i = 0; i < column_count; i++) {
        empty_columns[i] = empty_cell;
    }

    //Write scheme and column headers
    write_data_to_sector(file, empty_scheme, sizeof(struct TableScheme), table_scheme_sector);
    write_data_to_sector(file, empty_columns, column_headers_buff_size, table_headers_sector);
    free(empty_columns);

    struct StaticFileHeader header;
    read_static_header(file, &header);
    add_table_index(header.table_indices_sector, file, table_name, table_scheme_sector);

    return 0;
}

/*
 * Returns
 * -1 - wrong args
 * 0 - ok
 */
int schema_set_column(
        FILE* file, struct TableScheme* scheme, int col_num, enum CellType col_type, const char* column_name
) {
    if (strlen(column_name) > COLUMN_MAX_NAME_LENGTH) return -1;
    if (col_num > scheme->columns_count) return -1;

    struct HeaderCell new_cell;
    struct HeaderCell columns[scheme->columns_count];
    size_t col_buff_size = sizeof(struct HeaderCell) * scheme->columns_count;
    read_data_from_sector(file, &columns, col_buff_size, scheme->columns_info_sector);

    strcpy(new_cell.column_name, column_name);
    new_cell.meta.cell_type = col_type;
    columns[col_num - 1] = new_cell;

    write_data_to_sector(file, &columns, col_buff_size, scheme->columns_info_sector);
    return 0;
}
