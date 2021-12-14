#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "basic_task.h"


static void *basic_gnpl_task(void *arg);
static void normal_task_usage(gnpl_task_context_t *context);
static void real_time_task_usage(gnpl_task_context_t *context);


int main(int argc, char *argv[])
{
	int try_times = 5;
	gnpl_task_context_t real_time_context;
	gnpl_task_context_t normal_context;

	real_time_task_usage(&real_time_context);
	normal_task_usage(&normal_context);
	do {
		printf("<%s:%d> =====parent=====\n", __FUNCTION__, __LINE__);
		usleep(2000000);
	} while (try_times--);

	real_time_context.running = 0;
	normal_context.running = 0;
	pthread_join(real_time_context.tid, NULL);
	pthread_join(normal_context.tid, NULL);

	return 0;
}


static void *basic_gnpl_task(void *arg)
{
	gnpl_task_context_t *context = (gnpl_task_context_t *)arg;
	if (!context) {
		printf("<%s:%d> =====illegal param: NULL context=====\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	printf("<%s:%d> =====<in[%lu]>=====\n", __FUNCTION__, __LINE__, context->tid);
	while (context->running) {
		printf("<%s:%d> =====tid[%lu]=====\n", __FUNCTION__, __LINE__, context->tid);
		usleep(1000000);
	}
	printf("<%s:%d> =====<out[%lu]>=====\n", __FUNCTION__, __LINE__, context->tid);

	return context;
}

static void real_time_task_usage(gnpl_task_context_t *context)
{
	int ret = 0;
	gnpl_task_t task;

	gnpl_task_init(&task);
	gnpl_task_set_param(&task, "real_time_task_usage", basic_gnpl_task, context);
	task.cpuid = 1;
	task.piority = 1;

	context->running = 1;
	ret = gnpl_task_create_real_time(&task, &context->tid);
	if (0 != ret) {
		printf("<%s:%d> =====failed to create real time task=====\n", __FUNCTION__, __LINE__);
		return;
	}

	printf("<%s:%d> =====real time tid[%lu]=====\n", __FUNCTION__, __LINE__, context->tid);
}

static void normal_task_usage(gnpl_task_context_t *context)
{
	int ret = 0;
	gnpl_task_t task;

	context->running = 1;

	gnpl_task_init(&task);
	gnpl_task_set_param(&task, "normal_task_usage", basic_gnpl_task, context);
	ret = gnpl_task_create(&task, &context->tid);
	if (0 != ret) {
		printf("<%s:%d> =====failed to create normal task=====\n", __FUNCTION__, __LINE__);
		return;
	}

	printf("<%s:%d> =====normal tid[%lu]=====\n", __FUNCTION__, __LINE__, context->tid);
}

