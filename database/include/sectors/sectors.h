#ifndef LAB1LLP_SECTORS_H
#define LAB1LLP_SECTORS_H

#include <stdbool.h>
#include <stdint.h>
#include "../consts/sector_consts.h"

// Sector attributes
struct SectorHeader {
    bool is_taken;
    size_t sectors_taken_in_row;
    uint32_t prev_sector_number;
    uint32_t next_sector_number;

    uint32_t prev_logical_data_sector;
    uint32_t next_logical_data_sector;
};

// Sector itself
union SectorData {
    struct SectorHeader status;
    unsigned char data[SECTOR_SIZE];
};

#endif //LAB1LLP_SECTORS_H
