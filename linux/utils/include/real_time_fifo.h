#ifndef _REAL_TIME_H_
#define _REAL_TIME_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#include "list.h"
#include <pthread.h>


typedef int (*release_func)(struct list_head *list);

typedef struct {
	int in;
	int out;
	int size;
	struct list_head head;
	pthread_mutex_t mutex;
	release_func release;
	int is_initial;
} real_time_fifo_t;


int real_time_fifo_init(real_time_fifo_t *rfifo, int size, release_func func);
int real_time_fifo_put(real_time_fifo_t *rfifo, struct list_head *list);
int real_time_fifo_get(real_time_fifo_t *rfifo, struct list_head **list);
int real_time_fifo_destroy(real_time_fifo_t *rfifo);



#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* __cplusplus */

#endif






