#include "shm_rw_lock.h"
#include "mmap_file.h"
#include "log_adapter.h"

#define SHM_MUTEX_MAGIC (0x5aa53cc3)


shm_rwlock_t *shm_rwlock_alloc(char *key)
{
	int size = 0;
	shm_rwlock_t *shm_rwlock = NULL;

	if (!key) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal param!\n");
		return NULL;
	}

	size = sizeof(shm_rwlock_t);
	shm_rwlock = (shm_rwlock_t *)mmapfile(key,size);
	if (!shm_rwlock) {
		LOG_ERROR(LOG_MOD_UTILS, "mmapfile failed\n");
		return NULL;
	}

	if (SHM_MUTEX_MAGIC != shm_rwlock->magic) {
		memset(shm_rwlock,0,size);

		/*set name*/
		shm_rwlock->magic = SHM_MUTEX_MAGIC;
		snprintf(shm_rwlock->name,sizeof(shm_rwlock->name),"%s",key);
		shm_rwlock->ref_count++;

		/*set process shared*/
		pthread_rwlockattr_init(&shm_rwlock->attr);
		pthread_rwlockattr_setpshared(&shm_rwlock->attr, PTHREAD_PROCESS_SHARED);
		pthread_rwlock_init(&shm_rwlock->rw_lock,&shm_rwlock->attr);

		LOG_INFO(LOG_MOD_UTILS, "addr[%p] lock count:[%d] \n", shm_rwlock, shm_rwlock->lock_count);
	}

	return shm_rwlock;
}

int shm_rwlock_destroy(shm_rwlock_t *shm_rwlock)
{
	shm_rwlock->ref_count--;
	//LOG_INFO(LOG_MOD_UTILS, "shm_rwlock->ref_count:%d \n", shm_rwlock->ref_count);
	pthread_rwlock_destroy(&shm_rwlock->rw_lock);
	unmapfile(shm_rwlock,sizeof(shm_rwlock_t));
	return 0;
}

int shm_rwlock_wrlock(shm_rwlock_t *shm_rwlock)
{
	shm_rwlock->ref_count++;
	//LOG_INFO(LOG_MOD_UTILS, "shm_rwlock->ref_count:%d \n", shm_rwlock->ref_count);
	return pthread_rwlock_wrlock(&shm_rwlock->rw_lock);
}

int shm_rwlock_rdlock(shm_rwlock_t *shm_rwlock)
{
	shm_rwlock->ref_count++;
	//LOG_INFO(LOG_MOD_UTILS, "shm_rwlock->ref_count:%d \n", shm_rwlock->ref_count);
	return pthread_rwlock_rdlock(&shm_rwlock->rw_lock);
}

int shm_rwlock_unlock(shm_rwlock_t *shm_rwlock)
{
	shm_rwlock->ref_count--;
	//LOG_INFO(LOG_MOD_UTILS, "shm_rwlock->ref_count:%d \n", shm_rwlock->ref_count);
	return pthread_rwlock_unlock(&shm_rwlock->rw_lock);
}

