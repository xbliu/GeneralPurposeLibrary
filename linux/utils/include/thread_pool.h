#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#include "list.h"
#include "basic_task.h"

#include <stdint.h>


typedef enum {
	WORK_STATUS_CLEAR = 0,
	WORK_STATUS_PENDING,
	WORK_STATUS_RUNNING,
	WORK_STATUS_BUTT,
} work_status_e;


typedef struct {
	struct list_head entry;
	int status;
	void *(*func)(void *);
	void *arg;
} work_t;

typedef struct {
	pthread_mutex_t worker_mtx;
	struct list_head idle_head;
	struct list_head busy_head;

	int min_worker_num;
	int max_worker_num;
	int cur_worker_num; /*the number of workers currently created*/

	/*reclaim policy*/
} thread_pool_t;

typedef struct {
	struct list_head entry;

	int work_num;
	pthread_mutex_t work_mtx;
	struct list_head work_head;

	pthread_mutex_t wait_mtx;
	pthread_cond_t wait_cond;
	task_context_t task_ctx;

	thread_pool_t *thread_pool;
	char name[32];
	int cpu_mask;
	int is_bind_cpu;
} worker_t;


int thread_pool_init(thread_pool_t *thread_pool, int min_worker_num, int max_worker_num);
int thread_pool_destroy(thread_pool_t *thread_pool);
int thread_pool_add_work(thread_pool_t *thread_pool, work_t *work);
int thread_pool_add_cpu_work(thread_pool_t *thread_pool, int cpu_mask, work_t *work);


#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* __cplusplus */

#endif


