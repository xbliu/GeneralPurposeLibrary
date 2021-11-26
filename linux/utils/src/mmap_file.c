#include "mmap_file.h"
#include "log_adapter.h"

#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <linux/limits.h>
#include <linux/stat.h>


void *mmapfile(char *fn, unsigned long size)
{
	int mem_fd = -1;
	void *p_shm = NULL;
	int len = 0;
	char path[PATH_MAX + 1] = {0x00};

	if (fn == NULL) {
		return NULL;
	}

	if (strlen(fn) > PATH_MAX) {
		LOG_ERROR(LOG_MOD_UTILS, "error: mmapfile input path error.\n");
		return NULL;
	}

	if (NULL == realpath(fn, path)) {
		strcpy(path,fn);
	}
	
	if (size == 0) {
		LOG_ERROR(LOG_MOD_UTILS, "error:size is 0, error.\n");
		return NULL;
	}

	/* Open the file.  */
	mem_fd = open(path, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG  | O_SYNC);
	if (mem_fd == -1) {
		LOG_ERROR(LOG_MOD_UTILS, "Open file %s error: %i.\n", fn, errno);
		return NULL;
	}

	len = lseek(mem_fd, 0, SEEK_END);
	if (len < size) {
		ftruncate(mem_fd, size);
	}
	lseek(mem_fd, 0, SEEK_SET);

	/* Create the memory-mapping.  */
	p_shm = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);
	//close(mem_fd);
	if (p_shm != NULL) {
		LOG_INFO(LOG_MOD_UTILS, "Memory Map OK. Map File: %s, length:%#lX, addr:0x%08lX \n",
				fn, size, (uintptr_t)p_shm);
	} else {
		LOG_ERROR(LOG_MOD_UTILS, "Memory Map Failure. Map File %s, length:0x%08lX",
				fn, size);
	}

	return p_shm;
}

int unmapfile(void *addr, int size)
{
	munmap(addr, size);
}

void *mmapfile_r(char *fn, unsigned long size)
{
	int mem_fd = -1;
	void *p_shm = NULL;
	char path[PATH_MAX + 1] = {0x00};

	if (fn == NULL || size == 0)
		return NULL;

	if ((strlen(fn) > PATH_MAX) || (NULL == realpath(fn, path))) {
		LOG_ERROR(LOG_MOD_UTILS, "error: mmapfile_r input path error.\n");
		return NULL;
	}

	/* Open the file.  */
	mem_fd = open(path, O_RDONLY | O_SYNC);
	if (mem_fd == -1) {
		LOG_ERROR(LOG_MOD_UTILS, "Open file %s error: %d.\n", fn, errno);
		return NULL;
	}

	/* Create the memory-mapping.  */
	p_shm = mmap(0, size, PROT_READ, MAP_SHARED, mem_fd, 0);
	close(mem_fd);
	if (p_shm != NULL) {
		LOG_INFO(LOG_MOD_UTILS, "Memory Map OK. Map File: %s, length:%#lX, addr:0x%08lX\n",
				fn, size, (uintptr_t)p_shm);
	} else {
		LOG_ERROR(LOG_MOD_UTILS, "Memory Map Failure. Map File %s, length:0x%08lX\n",
				fn, size);
	}
	return p_shm;
}

