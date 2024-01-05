#include "../../../include/tables/table_row_utils.h"

/*
 *  Returns
 *  allocated sector
 */
size_t add_row_to_file(FILE *file, struct TableScheme *scheme, union TableCellWithData *row) {
    size_t row_size = sizeof(union TableCellWithData) * scheme->columns_count;
    size_t sectors_size = calculate_sectors_needed(row_size);
    uint32_t allocated_sector = allocate_sector(file, sectors_size);

    // Write row
    write_data_to_sector(file, row, row_size, allocated_sector);

    // Change sector headers links
    if (scheme->data_first_sector == TABLE_INDEX_HASH_EMPTY) {
        // Nothing to change in headers, no links
        scheme->data_first_sector = allocated_sector;
    } else {
        // Make links for two sectors
        struct SectorHeader last_row_header;
        struct SectorHeader new_last_row_header;
        // Read last and new last
        read_sector_header_by_index(file, scheme->data_last_sector, &last_row_header);
        read_sector_header_by_index(file, allocated_sector, &new_last_row_header);

        // New links
        last_row_header.next_logical_data_sector = allocated_sector;
        new_last_row_header.prev_logical_data_sector = scheme->data_last_sector;

        // Save in same place
        write_sector_header_by_index(file, scheme->data_last_sector, &last_row_header);
        write_sector_header_by_index(file, allocated_sector, &new_last_row_header);
    }

    // Change scheme signature
    scheme->data_last_sector = allocated_sector;
    write_data_to_sector(file, scheme, sizeof(struct TableScheme), scheme->own_sector);

    return allocated_sector;
}

/*
 *  Returns
 *  -1 - cannot delete row
 *  0 - ok
 */
int delete_row_from_file(FILE* file, uint32_t row_number, struct TableScheme* scheme) {
    uint32_t sector_ptr = scheme->data_first_sector;
    uint32_t row_count = 1;

    struct SectorHeader header;
    read_sector_header_by_index(file, sector_ptr, &header);
    while (row_count != row_number) {
        read_sector_header_by_index(file, sector_ptr, &header);
        sector_ptr = header.next_logical_data_sector;
        if (sector_ptr == TABLE_INDEX_HASH_EMPTY) return -1;
        row_count++;
    }

    // If deleting first or last, change signature
    if (sector_ptr == scheme->data_first_sector) {
        scheme->data_first_sector = header.next_logical_data_sector;
        write_data_to_sector(file, scheme, sizeof(struct TableScheme), scheme->own_sector);
    }
    if (sector_ptr == scheme->data_last_sector) {
        scheme->data_last_sector = header.prev_logical_data_sector;
        write_data_to_sector(file, scheme, sizeof(struct TableScheme), scheme->own_sector);
    }

    // Change list links
    struct SectorHeader prev_logic_header;
    struct SectorHeader next_logic_header;
    read_sector_header_by_index(file, header.prev_logical_data_sector, &prev_logic_header);
    read_sector_header_by_index(file, header.next_logical_data_sector, &next_logic_header);
    prev_logic_header.next_logical_data_sector = header.next_logical_data_sector;
    next_logic_header.prev_sector_number = header.prev_sector_number;
    write_sector_header_by_index(file, header.prev_logical_data_sector, &prev_logic_header);
    write_sector_header_by_index(file, header.next_logical_data_sector, &next_logic_header);

    deallocate_sector(file, sector_ptr);

    return 0;
}

/*
 *  Returns
 *  -1 - cannot find row
 *  0 - ok
 */
int get_row_by_number(FILE* file, uint32_t row_number, struct TableScheme* scheme, union TableCellWithData* row) {
    uint32_t sector_ptr = scheme->data_first_sector;
    uint32_t row_count = 1;

    struct SectorHeader header;
    while (row_count != row_number) {
        read_sector_header_by_index(file, sector_ptr, &header);
        sector_ptr = header.next_sector_number;
        if (sector_ptr == TABLE_INDEX_HASH_EMPTY) return -1;
        row_count++;
    }

    size_t row_size = sizeof(union TableCellWithData) * scheme->columns_count;
    read_data_from_sector(file, row, row_size, sector_ptr);

    return 0;
}


