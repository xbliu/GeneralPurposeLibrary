#ifndef FILE_MKDIR_H_
#define FILE_MKDIR_H_

#include <sys/types.h>

int fs_mkdirs(const char* path, mode_t mode);
int fs_create_file(char *name, mode_t mode);

#endif