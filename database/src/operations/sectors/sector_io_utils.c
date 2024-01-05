#include "../../../include/sectors/sector_utils.h"

const size_t sector_data_offset = sizeof(struct SectorHeader) + SECTOR_TO_DATA_MARGIN;

// Leave padding for sector header and data to avoid crossing
size_t calculate_sectors_needed(size_t struct_size) {
    // Integer division, then always cover the leftovers with one sector
    return (struct_size + sizeof(struct SectorHeader) + SECTOR_TO_DATA_MARGIN) / SECTOR_SIZE + 1;
}

/*
 * Returns
 * -4 - Struct bigger than sector capacity
 * -3 - Sector not allocated previously
 * -2 - Sector freed
 * -1 - Error writing sector
 * 0 - ok
 */
int write_data_to_sector(FILE *file, void *data_ptr, size_t buffer_size, uint32_t sector_n) {
    struct SectorHeader current_header;
    int sec_read = read_sector_header_by_index(file, sector_n, &current_header);
    // Check sector of size even exists, if returned by allocate_sector
    if (sec_read == -1) return -3;
    // Protection from overwriting
    if (calculate_sectors_needed(buffer_size) > current_header.sectors_taken_in_row) return -4;
    // Protection from random write
    if (!current_header.is_taken) return -2;

    size_t sector_ptr = sector_n * SECTOR_SIZE;
    fseek(file, sector_ptr + sector_data_offset, SEEK_SET);
    return fwrite(data_ptr, buffer_size, 1, file) - 1;
}

/*
 * Returns
 * -4 - Buffer bigger than sector capacity
 * -3 - Sector not allocated previously
 * -2 - Sector freed
 * -1 - Error read sector
 * 0 - sector number
 */
int read_data_from_sector(FILE *file, void *data_ptr, size_t buffer_size, uint32_t sector_n) {
    struct SectorHeader current_header;
    int sec_read = read_sector_header_by_index(file, sector_n, &current_header);
    // Check sector of size even exists, if returned by allocate_sector
    if (sec_read == -1) return -3;
    // Protection from overlap read
    if (calculate_sectors_needed(buffer_size) > current_header.sectors_taken_in_row) return -4;
    // Protection from random writes
    if (!current_header.is_taken) return -2;

    size_t sector_ptr = sector_n * SECTOR_SIZE;
    fseek(file, sector_ptr + sector_data_offset, SEEK_SET);
    return fread(data_ptr, buffer_size, 1, file) - 1;
}
