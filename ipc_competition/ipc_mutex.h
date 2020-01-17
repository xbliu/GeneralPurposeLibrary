#ifndef IPC_MUTEX_H_
#define IPC_MUTEX_H_


#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <pthread.h>


#define MAX_SHM_NAME_LEN (128)


typedef struct
{
	pthread_mutex_t mutex;
	pthread_mutexattr_t mutexattr;
	short int refcnt;
	short int priv_size; //max 64k
	void *priv; //may be use flexible array member
} ipc_mutex_t;


void *ipc_mutex_create(char *name);
int ipc_mutex_lock(void *handle);
int ipc_mutex_unlock(void *handle);
int ipc_mutex_destroy(void *handle);

//will be implemented
int ipc_mutex_trylock(void *handle);

#endif


