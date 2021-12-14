#ifndef _BASIC_THREAD_H_
#define _BASIC_THREAD_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdint.h>
#include <pthread.h>


typedef void *(*task_func)(void *);


typedef struct {
	char *name;	/*thread name*/
	int  cpuid; /*bind which cpu*/
	int piority; /*thread piority*/

	void *arg;
	task_func entry;

	uint8_t is_detach;
	uint8_t debug_on; //dump stack
} gnpl_task_t;

typedef struct {
	int running;
	pthread_t tid;
} gnpl_task_context_t;


void gnpl_task_dump_stack();
int gnpl_task_register_signal(void);

/* 
* task parameter setting interface
*/
int gnpl_task_init(gnpl_task_t *task);
gnpl_task_t *gnpl_task_alloc_init();
int gnpl_task_free(gnpl_task_t *task);
int gnpl_task_set_param(gnpl_task_t *task, char *name, task_func entry, void *arg);

/* 
* task creation interface 
*/ 
int gnpl_task_create(gnpl_task_t *task, pthread_t *tid);
int gnpl_task_create_real_time(gnpl_task_t *task, pthread_t *tid);


#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* __cplusplus */

#endif

