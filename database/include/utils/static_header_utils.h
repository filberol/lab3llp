#ifndef LAB1LLP_STATIC_HEADER_UTILS_H
#define LAB1LLP_STATIC_HEADER_UTILS_H

#include <stdio.h>
#include "../types/static/file_static_header.h"
#include "../consts/hash_consts.h"

struct StaticFileHeader create_file_header(int table_count, int table_indices_sector, int last_sector_taken);
struct StaticFileHeader create_empty_file_header();
int write_static_header(FILE* file, const struct StaticFileHeader* header);
int read_static_header(FILE* file, struct StaticFileHeader* header);

#endif //LAB1LLP_STATIC_HEADER_UTILS_H
