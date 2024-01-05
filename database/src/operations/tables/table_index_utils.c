#include "../../../include/tables/table_index_utils.h"

struct TableIndexArray *allocate_empty_table_index() {
    struct TableIndexArray *index_array = malloc(sizeof(struct TableIndexArray));
    index_array->next_table_sector = TABLE_INDEX_HASH_EMPTY;
    for (int i = 0; i < MAX_TABLES_COUNT; i++) {
        index_array->table_map[i].table_name_hash = TABLE_INDEX_HASH_EMPTY;
    }
    return index_array;
}


/*
 * Returns
 * -1 - no more free indices
 * 0 - ok
 */
int add_table_index(
        uint32_t indices_sector, FILE *file,
        const char *table_name, uint32_t data_sector
) {
    struct TableIndexArray* index_table = allocate_empty_table_index();
    read_table_index_from_sector(file, indices_sector, index_table);
    for (int i = 0; i < MAX_TABLES_COUNT; i++) {
        if (index_table->table_map[i].table_name_hash == TABLE_INDEX_HASH_EMPTY) {
            uint32_t new_hash = hash_string_default(table_name);
            // Update data
            index_table->table_map[i].table_name_hash = new_hash;
            index_table->table_map[i].schema_sector = data_sector;
            // Save instantly
            // TODO(Create flushes for table indices update)
            return write_table_index_to_sector(file, indices_sector, index_table);
            // return 0;
        }
    }
    return -1;
}

/*
 * Returns
 * -1 - no such table
 * 0 - ok
 */
int update_table_sector_link(
        uint32_t indices_sector, FILE *file,
        const char *table_name, uint32_t new_data_sector
) {
    struct TableIndexArray* index_table = allocate_empty_table_index();
    read_table_index_from_sector(file, indices_sector, index_table);
    uint32_t table_hash = hash_string_default(table_name);
    for (int i = 0; i < MAX_TABLES_COUNT; i++) {
        if (index_table->table_map[i].table_name_hash == table_hash) {
            // Update data
            index_table->table_map[i].schema_sector = new_data_sector;
            // Save instantly
            // TODO(Create flushes for table indices update)
            return write_table_index_to_sector(file, indices_sector, index_table);
            // return 0;
        }
    }
    return -1;
}

/*
 * Returns
 * -1 - nothing to delete
 * 0 - ok
 */
int remove_table_index(
        uint32_t indices_sector, FILE *file,
        const char *table_name
) {
    struct TableIndexArray* index_table = allocate_empty_table_index();
    read_table_index_from_sector(file, indices_sector, index_table);
    uint32_t table_hash = hash_string_default(table_name);
    for (int i = 0; i < MAX_TABLES_COUNT; i++) {
        if (index_table->table_map[i].table_name_hash == table_hash) {
            index_table->table_map[i].table_name_hash = TABLE_INDEX_HASH_EMPTY;
            index_table->table_map[i].schema_sector = TABLE_INDEX_HASH_EMPTY;
            return write_table_index_to_sector(file, indices_sector, index_table);
        }
    }
    return -1;
}

/*
 * Returns
 * Table data sector index
 * 0 - Not found
 */
uint32_t find_table_sector(
        uint32_t indices_sector, FILE *file,
        const char *table_name
) {
    struct TableIndexArray* index_table = allocate_empty_table_index();
    read_table_index_from_sector(file, indices_sector, index_table);
    uint32_t table_hash = hash_string_default(table_name);
    for (int i = 0; i < MAX_TABLES_COUNT; i++) {
        if (index_table->table_map[i].table_name_hash == table_hash) {
            return index_table->table_map[i].schema_sector;
        }
    }
    return 0;
}
