#ifndef IPC_FLOCK_H_
#define IPC_FLOCK_H_


#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <pthread.h>


#define MAX_FILE_LOCK_NAME_LEN (128)


typedef struct
{
	int fd;
} ipc_flock_t;

void *ipc_flock_create(char *name);
int ipc_flock_lock(void *handle);
int ipc_flock_unlock(void *handle);
int ipc_flock_destroy(void *handle);

//will be implemented
int ipc_flock_trylock(void *handle);

#endif


