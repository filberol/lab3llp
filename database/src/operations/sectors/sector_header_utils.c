#include "../../../include/sectors/sector_utils.h"

/*
 * Returns
 * -1 - error
 * 0 - ok
 */
int read_sector_header_by_index(FILE *file, uint32_t sector_number, struct SectorHeader *header) {
    fseek(file, sector_number * SECTOR_SIZE, SEEK_SET);
    return fread(header, sizeof(struct SectorHeader), 1, file) - 1;
}

/*
 * Returns
 * -1 - error
 * 0 - ok
 */
int write_sector_header_by_index(FILE *file, uint32_t sector_number, struct SectorHeader *header) {
    fseek(file, sector_number * SECTOR_SIZE, SEEK_SET);
    return fwrite(header, sizeof(struct SectorHeader), 1, file) - 1;
}
