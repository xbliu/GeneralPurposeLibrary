#ifndef _FILE_UTILS_H_
#define _FILE_UTILS_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include <stdio.h>

int file_get_size(FILE *fp);
int file_get_size_by_path(char *file_path);
int file_read_to_buffer(char *file_name, char *buffer, int len);
int file_write_from_buffer(char *file_name, char *buffer, int len);
int file_copy_content(char *src_file, char *dst_file);
char* file_system_cmd(const char* cmd, char*buf, int size);
int file_system(const char *command);



#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* __cplusplus */

#endif


