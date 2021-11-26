#ifndef _MMAP_FILE_H_
#define _MMAP_FILE_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


void *mmapfile(char *fn, unsigned long size);
void *mmapfile_r(char *fn, unsigned long size);
int unmapfile(void *addr, int size);


#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* __cplusplus */

#endif

