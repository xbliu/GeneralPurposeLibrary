#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "basic_task.h"


static void *basic_gnpl_task_test(void *arg)
{
	gnpl_task_context_t *context = (gnpl_task_context_t *)arg;
	printf("<%s:%d> ===========\n",__FUNCTION__,__LINE__);
	while (context->running) {
		printf("<%s:%d> tid[%lu]===========\n", __FUNCTION__, __LINE__, context->tid);
		usleep(1000000);
	}
}

static void real_time_gnpl_task_test(gnpl_task_context_t *context)
{
	int ret = 0;
	gnpl_task_t task;

	context->running = 1;
	context->tid = 0;

	gnpl_task_init(&task);
	task.name = "real_time_gnpl_task_test";
	task.arg = context;
	task.entry = basic_gnpl_task_test;
	task.piority = 1;
	task.cpuid = 1;
	ret = gnpl_task_create_real_time(&task, &context->tid);
	if (0 == ret) {
		printf("<%s:%d> real time tid[%lu]===========\n", __FUNCTION__, __LINE__, context->tid);
	}
}

static void normal_gnpl_task_test(gnpl_task_context_t *context)
{
	int ret = 0;
	gnpl_task_t task;

	context->running = 1;
	context->tid = 0;

	gnpl_task_init(&task);
	task.name = "normal_gnpl_task_test";
	task.arg = context;
	task.entry = basic_gnpl_task_test;
	task.piority = 0;
	task.cpuid = 1;
	ret = gnpl_task_create(&task, &context->tid);
	if (0 == ret) {
		printf("<%s:%d> normal tid[%lu]===========\n", __FUNCTION__, __LINE__, context->tid);
	}
}

int main(int argc, char *argv[])
{
	gnpl_task_context_t real_time_context;
	gnpl_task_context_t normal_context;

	real_time_gnpl_task_test(&real_time_context);
	normal_gnpl_task_test(&normal_context);
	do {
		printf("<%s:%d> ===========\n",__FUNCTION__,__LINE__);
		usleep(3000000);
	} while (1);

	return 0;
}

