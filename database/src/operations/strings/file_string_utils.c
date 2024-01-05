#include "../../../include/utils/file_string_utils.h"

/*
 * Returns
 * -1 - wrong args
 * number - sector written
 */
uint32_t write_string_to_sector(FILE* file, const char* string) {
    size_t buff_size = strlen(string) + SECTOR_TO_DATA_MARGIN;
    char buffer[buff_size];
    strcpy(buffer, string);

    size_t sec_need = calculate_sectors_needed(buff_size);
    uint32_t free_sector = allocate_sector(file, sec_need);
    write_data_to_sector(file, buffer, buff_size, free_sector);
    return free_sector;
}

void print_string_from_sector(FILE* file, uint32_t sector_n, size_t str_size) {
    size_t buff_size = str_size + SECTOR_TO_DATA_MARGIN;
    char buffer[buff_size];

    read_data_from_sector(file, buffer, buff_size, sector_n);
    printf("%s", buffer);
}
