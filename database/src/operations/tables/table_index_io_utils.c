#include "../../../include/tables/table_index_utils.h"

/*
 * Returns
 * <0 - error
 * 0 - ok
 */
int read_table_index_from_sector(FILE *file, uint32_t sector_number, struct TableIndexArray *table_index) {
//    Replaced logic, method wrapper
    return read_data_from_sector(file, table_index, sizeof(struct TableIndexArray), sector_number);
//    fseek(file, sector_number * SECTOR_SIZE + sizeof(struct SectorHeader), SEEK_SET);
//    return fread(table_index, sizeof(struct TableIndexArray), 1, file) - 1;
}

/*
 * Returns
 * <0 - error
 * 0 - ok
 */
int write_table_index_to_sector(FILE *file, uint32_t sector_number, struct TableIndexArray *table_index) {
//    Replaced logic, method wrapper
    return write_data_to_sector(file, table_index, sizeof(struct TableIndexArray), sector_number);
//    fseek(file, sector_number * SECTOR_SIZE + sizeof(struct SectorHeader), SEEK_SET);
//    return fwrite(table_index, sizeof(struct TableIndexArray), 1, file) - 1;
}
