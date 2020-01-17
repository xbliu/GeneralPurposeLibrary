#ifndef IPC_TEST_COMMON_H_

typedef struct 
{
	void *(*create)(char *name);
	int (*lock)(void *handle);
	int (*unlock)(void *handle);
	int (*destroy)(void *handle);
} ipc_lock_t;

#ifdef IPC_MUTEX

#include "ipc_mutex.h"

static ipc_lock_t ipc_lock = {
	.create = ipc_mutex_create,
	.lock = ipc_mutex_lock,
	.unlock = ipc_mutex_unlock,
	.destroy = ipc_mutex_destroy,
};

#elif IPC_SEMSV

#include "ipc_semsv.h"
static ipc_lock_t ipc_lock = {
	.create = ipc_semsv_create,
	.lock = ipc_semsv_lock,
	.unlock = ipc_semsv_unlock,
	.destroy = ipc_semsv_destroy,
};

#endif

#endif