/*
***file lock同一进程新锁会替换旧锁,所以同一进程允许递归调用
*/

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "ipc_flock.h"
#include "file_mkdir.h"


static int flock_simple(int fd, int cmd, int type, off_t offset, int whence, off_t len);


#define flock_read_simple(fd,offset,len)  flock_simple(fd,F_SETLKW,F_RDLCK,offset,SEEK_SET,len)
#define flock_write_simple(fd,offset,len) flock_simple(fd,F_SETLKW,F_WRLCK,offset,SEEK_SET,len)
#define flock_unlock_simple(fd,offset,len) flock_simple(fd,F_SETLKW,F_UNLCK,offset,SEEK_SET,len)


void *ipc_flock_create(char *name)
{
	char flock_name[MAX_FILE_LOCK_NAME_LEN] = {0};
	
	if (!name) {
		fprintf(stderr,"null parameter!\n");
		return NULL;
	}
	
	sprintf(flock_name,"/tmp/%s",name);
	if (access(flock_name,F_OK)) {
		fprintf(stderr,"create %s file!\n",flock_name);
		fs_mkdirs(flock_name,S_IRWXU | S_IRWXG);
		fs_create_file(flock_name,S_IRWXU | S_IRWXG);
	}
	
	ipc_flock_t *flock = calloc(sizeof(ipc_flock_t),1);
	if (!flock) {
		fprintf(stderr,"no free mem!\n");
		goto err_out;
	}
	
	flock->fd = open(flock_name,O_RDWR,0666);
	if (flock->fd < 0) {
		fprintf(stderr,"open %s file failed!\n",flock_name);
		free(flock);
		goto err_out;
	}
	
	return flock;
	
err_out:	
	return NULL;
}

int ipc_flock_lock(void *handle)
{
	int ret = 0;
	ipc_flock_t *flock = (ipc_flock_t *)handle;
	
	if (!flock) {
		fprintf(stderr,"null parameter!\n");
		return -1;
	}
	
	ret = flock_write_simple(flock->fd,0,0);
	if (-1 == ret) {
		fprintf(stderr,"flock failed:%s!\n",strerror(errno));
		return -1;
	}
	
	return 0;
}
int ipc_flock_unlock(void *handle)
{
	int ret = 0;
	ipc_flock_t *flock = (ipc_flock_t *)handle;
	
	if (!flock) {
		fprintf(stderr,"null parameter!\n");
		return -1;
	}
	
	ret = flock_unlock_simple(flock->fd,0,0);
	if (-1 == ret) {
		fprintf(stderr,"flock failed:%s!\n",strerror(errno));
		return -1;
	}
	
	return 0;
}
int ipc_flock_destroy(void *handle)
{
	ipc_flock_t *flock = (ipc_flock_t *)handle;
	
	if (!flock) {
		fprintf(stderr,"null parameter!\n");
		return -1;
	}
	
	close(flock->fd);
	free(flock);
	
	return 0;
}


/********************static*****************************/

static int flock_simple(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
	struct flock    lock;

	lock.l_type = type;
	lock.l_start = offset;
	lock.l_whence = whence;
	lock.l_len = len;

	return fcntl(fd, cmd, &lock);
}

		
		