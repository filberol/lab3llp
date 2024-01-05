#include "../../../include/sectors/sector_utils.h"

/*
 * Returns
 * -1 - cannot allocate
 * number - sector number
 */
uint32_t allocate_sector(FILE *file, size_t alloc_size) {
    // TODO(Make allocation of sectors dynamic, now writing only at the end)
    // Read static header
    struct StaticFileHeader static_header;
    int read_result = read_static_header(file, &static_header);
    if (read_result < 0) return -1;
    uint32_t header_ptr_free = static_header.first_at_end_free;
    uint32_t last_sector = static_header.last_sector;

    struct SectorHeader header;
    header.is_taken = true;
    header.sectors_taken_in_row = alloc_size;

    // First allocation
    if (last_sector == TABLE_INDEX_HASH_EMPTY) {
        // Write sector
        header.prev_sector_number = TABLE_INDEX_HASH_EMPTY;
        header.next_sector_number = TABLE_INDEX_HASH_EMPTY;
        write_sector_header_by_index(file, header_ptr_free, &header);
    } else {
        // Change last sector header
        struct SectorHeader last_header;
        read_sector_header_by_index(file, last_sector, &last_header);
        last_header.next_sector_number = header_ptr_free;
        write_sector_header_by_index(file, last_sector, &last_header);

        // Write new sector
        header.prev_sector_number = last_sector;
        header.next_sector_number = TABLE_INDEX_HASH_EMPTY;
        write_sector_header_by_index(file, header_ptr_free, &header);
    }

    // Write header
    static_header.last_sector = header_ptr_free;
    static_header.first_at_end_free = header_ptr_free + alloc_size;
    write_static_header(file, &static_header);

    return header_ptr_free;
}

void deallocate_sector(FILE *file, uint32_t sector_number) {
    struct SectorHeader header_to_free;
    read_sector_header_by_index(file, sector_number, &header_to_free);
    if (!header_to_free.is_taken) return;

    header_to_free.is_taken = false;
    struct SectorHeader linked_sector;
    // Append at right
    if (header_to_free.next_sector_number != TABLE_INDEX_HASH_EMPTY) {
        read_sector_header_by_index(file, header_to_free.next_sector_number, &linked_sector);
        if (!linked_sector.is_taken) {
            header_to_free.next_sector_number = linked_sector.next_sector_number;
            header_to_free.sectors_taken_in_row += linked_sector.sectors_taken_in_row;
            // Change current freed
            write_sector_header_by_index(file, sector_number, &header_to_free);
        }
    }
    // Append at left
    if (header_to_free.prev_sector_number != TABLE_INDEX_HASH_EMPTY) {
        read_sector_header_by_index(file, header_to_free.prev_sector_number, &linked_sector);
        if (!linked_sector.is_taken) {
            linked_sector.next_sector_number = header_to_free.next_sector_number;
            linked_sector.sectors_taken_in_row += header_to_free.sectors_taken_in_row;
            // Change prev, concatenate
            write_sector_header_by_index(file, header_to_free.prev_sector_number, &linked_sector);
        }
    }
}
