#include "../../../include/utils/static_header_utils.h"

struct StaticFileHeader create_file_header(
        int table_count,
        int table_indices_sector,
        int first_data_sector
) {
    struct StaticFileHeader header;
    header.table_count = table_count;
    header.table_indices_sector = table_indices_sector;
    header.first_at_end_free = first_data_sector;
    header.last_sector = TABLE_INDEX_HASH_EMPTY;
    return header;
}

struct StaticFileHeader create_empty_file_header() {
    // No tables, indices from first
    // Static header strictly less than sector size
    return create_file_header(0, 1, 1);
}

/*
 * Returns
 * -1 - wrong arguments
 * -2 - cannot write header
 * 0 - ok
 */
int write_static_header(FILE* file, const struct StaticFileHeader* header) {
    if (file == NULL || header == NULL) { return -1; }
    if (fseek(file, 0, SEEK_SET) != 0) { return -2; }
    size_t header_written = fwrite(header, sizeof(struct StaticFileHeader), 1, file);
    fflush(file);
    if (header_written != 1) { return -2; }
    return 0;
}

/*
 * Returns
 * -1 - wrong arguments
 * -2 - cannot read header
 * 0 - ok
 */
int read_static_header(FILE* file, struct StaticFileHeader* header) {
    if (file == NULL || header == NULL) { return -1; }
    if (fseek(file, 0, SEEK_SET) != 0) { return -2; }
    size_t read = fread(header, sizeof(struct StaticFileHeader), 1, file);
    if (read != 1) { return -2; }
    return 0;
}
