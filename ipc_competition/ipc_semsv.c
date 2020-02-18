#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>

#include "ipc_semsv.h"
#include "file_mkdir.h"


void *ipc_semsv_create(char *name)
{
	char sem_name[MAX_SEM_NAME_LEN] = {0};
	int sem_id = 0;
	key_t key_id = 0;
	
	if (!name) {
		fprintf(stderr,"null parameter!\n");
		return NULL;
	}
	
	sprintf(sem_name,"/tmp/%s",name);
	if (access(sem_name,F_OK)) {
		fprintf(stderr,"create sem file!\n");
		fs_mkdirs(sem_name,S_IRWXU | S_IRWXG);
		fs_create_file(sem_name,S_IRWXU | S_IRWXG);
	}
	
	ipc_semsv_t *semsv = calloc(sizeof(ipc_semsv_t),1);
	if (!semsv) {
		fprintf(stderr,"no free mem!\n");
		goto err_alloc;
	}
	
	key_id = ftok(sem_name,0);
	if (key_id < 0) {
		fprintf(stderr,"ftok %s file failed : %s!\n",sem_name,strerror(errno));
		goto err_out;
	}
	
	sem_id = semget(key_id,1,0666 | IPC_CREAT | IPC_EXCL);
	if (sem_id < 0 && errno != EEXIST) {
		fprintf(stderr,"sem get failed!\n");
		goto err_out;
	}
	
	if (sem_id >= 0) {
		/*init it if first create*/
		union semun sem_union;
		sem_union.val = 1;
		if(-1 == semctl(sem_id,0,SETVAL,sem_union)) {
			fprintf(stderr,"semctl init failed : %s!\n",strerror(errno));
			goto err_out;
		}
		semsv->is_creator = 1;
	} else {
		/*get it if exist*/
		sem_id = semget(key_id,1,0555);
		semsv->is_creator = 0;
	}
	
	fprintf(stderr,"sem id:0x%x\n",sem_id);
	//semsv->key_id = key_id;
	semsv->sem_id = sem_id;
	
	return (void *)semsv;
	
err_out:
	free(semsv);
err_alloc:	
	return NULL;
}

int ipc_semsv_lock(void *handle)
{
	int ret = 0;
	ipc_semsv_t *semsv = (ipc_semsv_t *)handle;
	
	if (!semsv) {
		fprintf(stderr,"null parameter!\n");
		return -1;
	}
	
	struct sembuf sem_buf;
	sem_buf.sem_num = 0;
	sem_buf.sem_op = -1;
	sem_buf.sem_flg = SEM_UNDO;
	ret = semop(semsv->sem_id,&sem_buf,1);
	if (-1 == ret) {
		fprintf(stderr,"sem lock failed : %s!\n",strerror(errno));
	}
	
	return ret;
}

int ipc_semsv_unlock(void *handle)
{
	int ret = 0;
	ipc_semsv_t *semsv = (ipc_semsv_t *)handle;
	
	if (!semsv) {
		fprintf(stderr,"null parameter!\n");
		return -1;
	}
	
	struct sembuf sem_buf;
	sem_buf.sem_num = 0;
	sem_buf.sem_op = 1;
	sem_buf.sem_flg = SEM_UNDO;
	ret = semop(semsv->sem_id,&sem_buf,1);
	if (-1 == ret) {
		fprintf(stderr,"sem unlock failed!\n");
	}
	
	return ret;
}

int ipc_semsv_destroy(void *handle)
{
	int ret = 0;
	ipc_semsv_t *semsv = (ipc_semsv_t *)handle;
	union semun sem_union;
	
	if (!semsv) {
		fprintf(stderr,"null parameter!\n");
		return -1;
	}
	
	/*only creator need do this*/
	if (semsv->is_creator) {
		ret = semctl(semsv->sem_id,0,IPC_RMID,sem_union);
		if (-1 == ret) {
			fprintf(stderr,"sem delete failed!\n");
			goto err_out;
		}
	}
	
	free(semsv);
err_out:
	return ret;
}

