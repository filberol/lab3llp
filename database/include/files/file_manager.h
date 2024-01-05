#ifndef LAB1LLP_FILE_MANAGER_H
#define LAB1LLP_FILE_MANAGER_H

#include <stdio.h>

FILE *open_file(const char *filename, const char *mode);
void close_file(FILE *file);

#endif //LAB1LLP_FILE_MANAGER_H
