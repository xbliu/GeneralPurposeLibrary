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


static int fs_mkdirs(const char* path, mode_t mode);
static int fs_create_file(char *name, mode_t mode);


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
		fs_mkdirs(sem_name,0666);
		fs_create_file(sem_name,0666);
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
	
	struct sembuf sem_buf;
	sem_buf.sem_num = 0;
	sem_buf.sem_op = -1;
	sem_buf.sem_flg = SEM_UNDO;
	ret = semop(semsv->sem_id,&sem_buf,1);
	if (-1 == ret) {
		fprintf(stderr,"sem lock failed : %s!\n",strerror(errno));
	}
	
	return 0;
}

int ipc_semsv_unlock(void *handle)
{
	int ret = 0;
	ipc_semsv_t *semsv = (ipc_semsv_t *)handle;
	
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


/**********************static******************************/

static int fs_mkdirs(const char* path, mode_t mode)
{
    int res = 0;
    int fd = 0;
    struct stat sb;
    char* buf = strdup(path);
    char* segment = buf + 1;
    char* p = segment;

    if (*buf != '/') {
        fprintf(stderr,"Relative paths are not allowed: %s", buf);
        res = -EINVAL;
        goto done;
    }

    if ((fd = open("/", 0)) == -1) {
        fprintf(stderr,"Failed to open(/): %s", strerror(errno));
        res = -errno;
        goto done;
    }
    
    while (*p != '\0') {
        if (*p == '/') {
            *p = '\0';

            if (!strcmp(segment, "..") || !strcmp(segment, ".") || !strcmp(segment, "")) {
                fprintf(stderr,"Invalid path: %s", buf);
                res = -EINVAL;
                goto done_close;
            }

            if (fstatat(fd, segment, &sb, AT_SYMLINK_NOFOLLOW) != 0) {
                if (errno == ENOENT) {
                    /* Nothing there yet; let's create it! */
                    if (mkdirat(fd, segment, mode) != 0) {
                        if (errno == EEXIST) {
                            /* We raced with someone; ignore */
                        } else {
                            fprintf(stderr,"Failed to mkdirat(%s): %s", buf, strerror(errno));
                            res = -errno;
                            goto done_close;
                        }
                    }
                } else {
                    fprintf(stderr,"Failed to fstatat(%s): %s", buf, strerror(errno));
                    res = -errno;
                    goto done_close;
                }
            } else {
                if (S_ISLNK(sb.st_mode)) {
                    fprintf(stderr,"Symbolic links are not allowed: %s", buf);
                    res = -ELOOP;
                    goto done_close;
                }
                if (!S_ISDIR(sb.st_mode)) {
                    fprintf(stderr,"Existing segment not a directory: %s", buf);
                    res = -ENOTDIR;
                    goto done_close;
                }
            }

            /* Yay, segment is ready for us to step into */
            int next_fd;
            if ((next_fd = openat(fd, segment, O_NOFOLLOW | O_CLOEXEC)) == -1) {
                fprintf(stderr,"Failed to openat(%s): %s", buf, strerror(errno));
                res = -errno;
                goto done_close;
            }

            close(fd);
            fd = next_fd;

            *p = '/';
            segment = p + 1;
        }
        p++;
    }

done_close:
    close(fd);
done:
    free(buf);
    return res;
}

static int fs_create_file(char *name, mode_t mode)
{
	int fd = creat(name,mode);
	if (fd < 0) {
		fprintf(stderr,"create %s file failed : %s!\n",name,strerror(errno));
		return -1;
	}
	
	close(fd);
}

