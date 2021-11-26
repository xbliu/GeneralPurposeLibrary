#ifndef _BASIC_THREAD_H_
#define _BASIC_THREAD_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>


typedef void *(*task_func)(void *);


typedef struct {
	char *name;	//thread name
	int  cpuid; //bind which cpu
	int piority;

	void *arg;
	task_func entry;

	uint8_t is_detach;
	uint8_t debug_on; //dump stack
} task_t;

typedef struct {
	int running;
	pthread_t tid;
} task_context_t;


void dump_thread_stack();
int task_init(task_t *task);
task_t *task_alloc_init();
int task_free(task_t *task);
int task_set_param(task_t *task, char *name, task_func entry, void *arg);
int task_create(task_t *task, pthread_t *tid);
int task_register_signal(void);


#endif

