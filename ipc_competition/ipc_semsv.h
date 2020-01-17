#ifndef IPC_SEMSV_H_
#define IPC_SEMSV_H_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define MAX_SEM_NAME_LEN (128)

typedef struct
{
	int sem_id;
	int is_creator;
} ipc_semsv_t;

#ifndef HAVE_SEMUN_UNION

union semun 
{
    int val;  
    struct semid_ds *buf; 
    unsigned short *array;
}sem_union;

#endif


void *ipc_semsv_create(char *name);
int ipc_semsv_lock(void *handle);
int ipc_semsv_unlock(void *handle);
int ipc_semsv_destroy(void *handle);

//will be implemented
int ipc_semsv_trylock(void *handle);

#endif

