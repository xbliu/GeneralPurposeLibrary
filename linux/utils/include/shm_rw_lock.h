#ifndef _SHM_RW_LOCK_H_
#define _SHM_RW_LOCK_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#include <pthread.h>


typedef struct {
	int magic;
	char name[64];
	int ref_count;
	int lock_count;
	pthread_rwlock_t rw_lock;
	pthread_rwlockattr_t attr;
} shm_rwlock_t;


shm_rwlock_t *shm_rwlock_alloc(char *key);
int shm_rwlock_destroy(shm_rwlock_t *shm_rwlock);
int shm_rwlock_wrlock(shm_rwlock_t *shm_rwlock);
int shm_rwlock_rdlock(shm_rwlock_t *shm_rwlock);
int shm_rwlock_unlock(shm_rwlock_t *shm_rwlock);


#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* __cplusplus */

#endif


