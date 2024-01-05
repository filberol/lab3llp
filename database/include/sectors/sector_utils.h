#ifndef LAB1LLP_SECTOR_UTILS_H
#define LAB1LLP_SECTOR_UTILS_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "../../include/sectors/sectors.h"
#include "../types/static/file_static_header.h"
#include "../../include/utils/static_header_utils.h"

uint32_t allocate_sector(FILE* file, size_t alloc_size);
void deallocate_sector(FILE* file, uint32_t sector_number);
size_t calculate_sectors_needed(size_t struct_size);

int read_sector_header_by_index(FILE *file, uint32_t sector_number, struct SectorHeader *header);
int write_sector_header_by_index(FILE *file, uint32_t sector_number, struct SectorHeader *header);

int write_data_to_sector(FILE* file, void* data_ptr, size_t buffer_size, uint32_t sector_n);
int read_data_from_sector(FILE* file, void* data_ptr, size_t buffer_size, uint32_t sector_n);

#endif //LAB1LLP_SECTOR_UTILS_H
