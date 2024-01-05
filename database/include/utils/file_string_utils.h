#ifndef LAB1LLP_FILE_STRING_UTILS_H
#define LAB1LLP_FILE_STRING_UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../consts/sector_consts.h"
#include "../../include/sectors/sector_utils.h"

uint32_t write_string_to_sector(FILE* file, const char* string);
void print_string_from_sector(FILE* file, uint32_t sector_n, size_t str_size);

#endif //LAB1LLP_FILE_STRING_UTILS_H
