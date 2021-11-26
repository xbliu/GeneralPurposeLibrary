#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "log_adapter.h"


#define READ_BUF_MAX_LEN (1024)
#ifndef MIN
#define MIN(a, b) (((a) > (b)) ? (b) : (a))
#endif


int file_get_size(FILE *fp)
{
	int size = 0;

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	return size;
}

int file_get_size_by_path(char *file_path)
{
	int size = 0;
	FILE *fp = NULL;

	fp = fopen(file_path, "rb");
	if (!fp) {
		LOG_ERROR(LOG_MOD_UTILS, "can't open %s file\n", file_path);
		return -1;
	}
	
	size = file_get_size(fp);
	fclose(fp);

	return size;
}

int file_read_to_buffer(char *file_name, char *buffer, int len)
{
	int size = 0;
	FILE *fp = NULL;

	fp = fopen(file_name, "rb");
	if (!fp) {
		LOG_ERROR(LOG_MOD_UTILS, "%s file open failed!\n", file_name);
		return -1;
	}

	fseek(fp,0,SEEK_END);
	size = ftell(fp);
	fseek(fp,0,SEEK_SET);

	size = size<len ? : len;
	fread(buffer,size,1,fp);

	fclose(fp);
	return 0;
}

int file_write_from_buffer(char *file_name, char *buffer, int len)
{
	int fd = -1;
	FILE *fp = NULL;

	fp = fopen(file_name, "wb");
	if (!fp) {
		LOG_ERROR(LOG_MOD_UTILS, "%s file open failed!\n", file_name);
		return -1;
	}

	fwrite(buffer,len,1,fp);
	fflush(fp);

	/*add file sync*/
	fd = fileno(fp);
	if (-1 != fd) {
		fsync(fd);
	}

	fclose(fp);
	return 0;
}

int file_copy_content(char *src_file, char *dst_file)
{
	int read_len = 0;
	FILE *src = NULL;
	FILE *dst = NULL;
	char buf[READ_BUF_MAX_LEN] = {0};

	if(access(src_file, F_OK) != 0){
		LOG_ERROR(LOG_MOD_UTILS, "no file %s!\n", src_file);
		return -1;
	}

	src = fopen(src_file, "rb");
	if (!src) {
		LOG_ERROR(LOG_MOD_UTILS, "open file %s failed!\n", src_file);
		return -1;
	}
	
	dst = fopen(dst_file, "wb");
	if (!dst) {
		LOG_ERROR(LOG_MOD_UTILS, "open file %s failed!\n", dst_file);
		fclose(src);
		return -1;
	}

	while (!feof(src)) {
		read_len = fread(buf,1,READ_BUF_MAX_LEN,src);
		fwrite(buf, 1, read_len, dst);
	}

	fflush(dst);
	fclose(dst);
	fclose(src);

	return 0;
}


char* file_system_cmd(const char* cmd, char*buf, int size)
{
    char tmp[256] = {0};
    FILE* fp = NULL;
    int offset = 0;
    int len = 0;

    fp = popen(cmd, "r");
    if (!fp) {
		LOG_ERROR(LOG_MOD_UTILS, "open cmd %s failed!\n", cmd);
        return NULL;
    }

    while (fgets(tmp, sizeof(tmp), fp) != NULL) {
        len = strlen(tmp);
		if (offset + len >= size) {
			buf[offset] = 0;
            break;
		}

		strcpy(buf + offset, tmp);
        offset += len;
    }

	pclose(fp);

    return buf;
}

int file_system(const char *command)
{
	char buf[1024] = {0};
	file_system_cmd(command, buf, sizeof(buf));

	return 0;
}

int file_is_process_running(char* name)
{
	int count = 0;
	char cmd[128]={0};
	char buf[1024]={0};

	sprintf(cmd,"ps | grep %s | grep -v grep |wc -l",name);
	file_system_cmd(cmd, buf, sizeof(buf));

	count = atoi(buf);
	LOG_INFO(LOG_MOD_UTILS, "count[%d] \n", count);

	return count;
}

char *file_get_process_name(char *name, int len)
{
	int ret = 0;
	int copy_len = 0;
	char *ptr = NULL;
	char buf[128] = {0};

	ret = readlink("/proc/self/exe", buf, sizeof(buf));
	if (ret < 0) {
		LOG_ERROR(LOG_MOD_UTILS, "error:%s\n", strerror(errno));
		return "";
	}

	ptr = strrchr(buf,'/');
	if (!ptr) {
		LOG_ERROR(LOG_MOD_UTILS, "error:%s\n", strerror(errno));
		return "";
	}

	++ptr;
	copy_len = MIN(len, strlen(ptr));
	strncpy(name, ptr, copy_len);
	return name;
}

