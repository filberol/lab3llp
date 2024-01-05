#ifndef LAB1LLP_FILE_STATIC_HEADER_H
#define LAB1LLP_FILE_STATIC_HEADER_H

#include <stdint.h>

// File header strictly less than SECTOR_SIZE
struct StaticFileHeader {
    uint32_t table_count;
    uint32_t table_indices_sector;
    uint32_t last_sector;
    uint32_t first_at_end_free;
};

#endif //LAB1LLP_FILE_STATIC_HEADER_H
