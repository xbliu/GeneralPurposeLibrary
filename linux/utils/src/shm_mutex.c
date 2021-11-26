#include "shm_mutex.h"
#include "mmap_file.h"
#include "log_adapter.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#include <fcntl.h>
#include <stdint.h>
#include <string.h>


#define USE_SHMAT (0)
#define SHM_MUTEX_MAGIC (0x5aa53cc3)


shm_mutex_t *shm_mutex_alloc(char *key)
{
	int shm_id;
	int size = 0;
	struct shmid_ds buf;
	shm_mutex_t *shm_mutex = NULL;

	if (!key) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal param!\n");
		return NULL;
	}
	
	/*mmap*/
#if USE_SHMAT
	size = sizeof(shm_mutex_t);
	shm_id = shmget(atoi(key), size, IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if (shm_id < 0) {
		LOG_ERROR(LOG_MOD_UTILS, "shmget failed: %s!\n", strerror(errno));
		return NULL;
	}

	LOG_INFO(LOG_MOD_UTILS, "shm_id:%d key:%d\n", shm_id, atoi(key));

	shm_mutex = shmat(shm_id, NULL, 0);
	if (!shm_mutex) {
		LOG_ERROR(LOG_MOD_UTILS, "shmat failed: %s!\n", strerror(errno));
		return NULL;
	}

	shmctl(shm_id,IPC_STAT,&buf);

	if (1 == buf.shm_nattch) {
		memset(shm_mutex,0,size);

		/*set name*/
		snprintf(shm_mutex->name,sizeof(shm_mutex->name),"%s",key);

		/*set process shared*/
		pthread_mutexattr_init(&shm_mutex->attr);
		pthread_mutexattr_setpshared(&shm_mutex->attr, PTHREAD_PROCESS_SHARED);
		pthread_mutex_init(&shm_mutex->mutex,&shm_mutex->attr);

		LOG_INFO(LOG_MOD_UTILS, "lock count:%d \n", shm_mutex->lock_count);
	}
 #else
	char file_path[64] = {0};
	size = sizeof(shm_mutex_t);
	snprintf(file_path,sizeof(file_path),"/tmp/%s",key);
	shm_mutex = (shm_mutex_t *)mmapfile(file_path,size);
	if (!shm_mutex) {
		LOG_ERROR(LOG_MOD_UTILS, "mmapfile failed\n");
		return NULL;
	}

	if (SHM_MUTEX_MAGIC != shm_mutex->magic) {
		memset(shm_mutex,0,size);

		/*set name*/
		shm_mutex->magic = SHM_MUTEX_MAGIC;
		snprintf(shm_mutex->name,sizeof(shm_mutex->name),"%s",key);

		/*set process shared*/
		pthread_mutexattr_init(&shm_mutex->attr);
		pthread_mutexattr_setpshared(&shm_mutex->attr, PTHREAD_PROCESS_SHARED);
#ifndef _ANDROID_
		pthread_mutexattr_setrobust(&shm_mutex->attr, PTHREAD_MUTEX_ROBUST);
#endif
		pthread_mutex_init(&shm_mutex->mutex,&shm_mutex->attr);
	}
	shm_mutex->ref_count++;
	LOG_INFO(LOG_MOD_UTILS, "addr[%p] lock count:[%d] ref_count[%d] file[%s]\n", shm_mutex, shm_mutex->lock_count, shm_mutex->ref_count, file_path);
#endif

	return shm_mutex;
}

int shm_mutex_destroy(shm_mutex_t *shm_mutex)
{
	if (!shm_mutex) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal param!\n");
		return -1;
	}

#if USE_SHMAT
	shmdt(shm_mutex);
#else
	/*
		remove the destroy, start initialization once, and restart the tmp file will be purged;
		otherwise initialization and destruction spin locks need to be added
	*/
	shm_mutex->ref_count--;
	/*
	if (0 == shm_mutex->ref_count) {
		pthread_mutex_destroy(&shm_mutex->mutex);
		shm_mutex->magic = 0x0;
	}
	*/
	unmapfile(shm_mutex,sizeof(shm_mutex_t));
#endif

	return 0;
}

int shm_mutex_lock(shm_mutex_t *shm_mutex)
{
	int ret = 0;
	ret = pthread_mutex_lock(&shm_mutex->mutex);
	if (0 == ret) {
		//LOG_INFO(LOG_MOD_UTILS, "lock count:%d\n", shm_mutex->lock_count);
		shm_mutex->lock_count++;
		return 0;
	}

#ifndef _ANDROID_
	if (EOWNERDEAD == ret) {
		ret = pthread_mutex_consistent(&shm_mutex->mutex);
		LOG_INFO(LOG_MOD_UTILS, "call pthread_mutex_consistent\n");
	}
	
	if (0 == ret) {
		shm_mutex->ref_count--;
		return 0;
	}
#endif

	LOG_ERROR(LOG_MOD_UTILS, "mutex lock error:%s\n", strerror(errno));
	return -1;
}

int shm_mutex_unlock(shm_mutex_t *shm_mutex)
{
	shm_mutex->lock_count--;
	pthread_mutex_unlock(&shm_mutex->mutex);
	return 0;
}

