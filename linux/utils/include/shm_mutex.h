#ifndef _SHM_MUTEX_H_
#define _SHM_MUTEX_H_

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
	pthread_mutex_t mutex;
	pthread_mutexattr_t attr;
} shm_mutex_t;


shm_mutex_t *shm_mutex_alloc(char *key);
int shm_mutex_destroy(shm_mutex_t *shm_mutex);
int shm_mutex_lock(shm_mutex_t *shm_mutex);
int shm_mutex_unlock(shm_mutex_t *shm_mutex);


#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* __cplusplus */

#endif

