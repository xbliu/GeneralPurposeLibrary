#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>

#include "ipc_mutex.h"


void *ipc_mutex_create(char *name)
{
	int ret = 0;
	int fd = 0;
	int priv_size = strlen(name) + 1;
	int size = sizeof(ipc_mutex_t) + priv_size;
	ipc_mutex_t *ipc_mutex = NULL;
	char shm_name[MAX_SHM_NAME_LEN] = {0};
	int is_create = 0;
	
	if (!name) {
		fprintf(stderr,"null paramter name!\n");
		return NULL;
	}
	
	sprintf(shm_name,"/dev/shm/%s",name);
	is_create = (access(shm_name,F_OK) != 0 ? 0 : 1); //1 exist : 0 not exist
	
	//fprintf(stderr,"%s file exist: %d\n",shm_name,is_create);
	
	fd = shm_open(name, O_CREAT | O_RDWR,0666);
	if (fd < 0) {
		fprintf(stderr,"open %s file failed : %s!\n",name,strerror(errno));
		return NULL;
	}
	
	if (!is_create) {
		ret = ftruncate(fd,size);
		if (0 != ret) {
			close(fd);
			shm_unlink(name);
			fprintf(stderr,"ftruncate %s file failed!\n",name);
			return NULL;
		}
	}
	
	//fprintf(stderr,"size:%d\n",size);
	
	ipc_mutex = mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (!ipc_mutex) {
		close(fd);
		shm_unlink(name);
		fprintf(stderr,"mmap %s file failed!\n",name);
		return NULL;
	}
	
	if (!is_create) {
		memset(ipc_mutex,0,size);
		pthread_mutexattr_init(&ipc_mutex->mutexattr);
	}
	
	pthread_mutexattr_setpshared(&ipc_mutex->mutexattr, PTHREAD_PROCESS_SHARED);
	pthread_mutexattr_setrobust(&ipc_mutex->mutexattr, PTHREAD_MUTEX_ROBUST);
	
	if (!is_create) {
		pthread_mutex_init(&ipc_mutex->mutex, &ipc_mutex->mutexattr);
		ipc_mutex->priv_size = priv_size;
		memcpy(&ipc_mutex->priv,name,priv_size);
	}
	
	ipc_mutex->refcnt++;
	
	//fprintf(stderr,"ipc mutex priv_size:%d\n",ipc_mutex->priv_size);
	//fprintf(stderr,"ipc mutex name:%s\n",(char *)(&ipc_mutex->priv));
	
	close(fd);
	return ipc_mutex;
}

int ipc_mutex_lock(void *handle)
{
	int ret = 0;
	ipc_mutex_t *ipc_mutex = handle;
	
	if (!ipc_mutex) {
		fprintf(stderr,"null paramter handle!\n");
		return -1;
	}
		
	do {
		ret = pthread_mutex_lock(&ipc_mutex->mutex);
		if (EOWNERDEAD == ret) {
			//fprintf(stderr,"refcnt:%d  a process abnormal exit!\n",handle->refcnt);
			pthread_mutex_consistent(&ipc_mutex->mutex);
			pthread_mutex_unlock(&ipc_mutex->mutex);
			ipc_mutex->refcnt--;
		}
	} while (EOWNERDEAD == ret);
	
	if (0 != ret) {
		fprintf(stderr,"lock failed: %d\n",ret);
		return -1;
	}
	
	return 0;
}

int ipc_mutex_unlock(void *handle)
{
	int ret = 0;
	ipc_mutex_t *ipc_mutex = handle;
	
	if (!ipc_mutex) {
		fprintf(stderr,"null paramter handle!\n");
		return -1;
	}
	
	ret = pthread_mutex_unlock(&ipc_mutex->mutex);
	if (0 != ret) {
		fprintf(stderr,"unlock failed: %d\n",ret);
		return -1;
	}
	
	return 0;
}

int ipc_mutex_destroy(void *handle)
{
	int ret = 0;
	int is_create = 0;
	char shm_name[MAX_SHM_NAME_LEN] = {0};
	ipc_mutex_t *ipc_mutex = handle;
	
	if (!ipc_mutex) {
		fprintf(stderr,"null paramter handle!\n");
		return -1;
	}
	
	//sprintf(shm_name,"/dev/shm/%s",(char *)(&ipc_mutex->priv));
	//is_create = (access(shm_name,F_OK) != 0 ? 0 : 1); //1 exist : 0 not exist
	
	if (--ipc_mutex->refcnt) {
		munmap(ipc_mutex, sizeof(ipc_mutex_t)+ipc_mutex->priv_size);
		return 0;
	}
	
	pthread_mutexattr_destroy(&ipc_mutex->mutexattr);
    pthread_mutex_destroy(&ipc_mutex->mutex);
	memcpy(shm_name,(char *)(&ipc_mutex->priv),sizeof(shm_name));
	munmap(ipc_mutex, sizeof(ipc_mutex_t)+ipc_mutex->priv_size);
	
	ret = shm_unlink(shm_name);
	if (0 != ret) {
		fprintf(stderr,"shm unlink %s failed!\n",shm_name);
		return -1;
	}
	
	//fprintf(stderr,"shm unlink success!\n");
	
	return 0;
}

