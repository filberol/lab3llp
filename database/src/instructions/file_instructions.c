#include "../../include/instructions/file_instructions.h"

int create_file_and_init_empty_structure(FILE* file) {

    struct StaticFileHeader def_header = create_empty_file_header();
    write_static_header(file, &def_header);

    struct TableIndexArray* global_index_array = allocate_empty_table_index();
    size_t sectors_needed = calculate_sectors_needed(sizeof(struct TableIndexArray));
    allocate_sector(file, sectors_needed);
    write_table_index_to_sector(file, def_header.table_indices_sector, global_index_array);

    def_header.first_at_end_free += sectors_needed;
    write_static_header(file, &def_header);
    free(global_index_array);

    return 0;
}
