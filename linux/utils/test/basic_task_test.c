#include <stdio.h>
#include <stdlib.h>

#include "basic_task.h"


static void *basic_task_test(void *arg)
{
	task_context_t *context = (task_context_t *)arg;
	printf("<%s:%d> ===========\n",__FUNCTION__,__LINE__);
	while (context->running) {
		printf("<%s:%d> tid[%lu]===========\n", __FUNCTION__, __LINE__, context->tid);
		usleep(1000000);
	}
}

static void real_time_task_test(task_context_t *context)
{
	int ret = 0;
	task_t task;

	context->running = 1;
	context->tid = 0;

	task_init(&task);
	task.name = "real_time_task_test";
	task.arg = context;
	task.entry = basic_task_test;
	task.piority = 1;
	task.cpuid = 1;
	ret = task_create_real_time(&task, &context->tid);
	if (0 == ret) {
		printf("<%s:%d> real time tid[%lu]===========\n", __FUNCTION__, __LINE__, context->tid);
	}
}

static void normal_task_test(task_context_t *context)
{
	int ret = 0;
	task_t task;

	context->running = 1;
	context->tid = 0;

	task_init(&task);
	task.name = "normal_task_test";
	task.arg = context;
	task.entry = basic_task_test;
	task.piority = 0;
	task.cpuid = 1;
	ret = task_create(&task, &context->tid);
	if (0 == ret) {
		printf("<%s:%d> normal tid[%lu]===========\n", __FUNCTION__, __LINE__, context->tid);
	}
}

int main(int argc, char *argv[])
{
	task_context_t real_time_context;
	task_context_t normal_context;

	real_time_task_test(&real_time_context);
	normal_task_test(&normal_context);
	do {
		printf("<%s:%d> ===========\n",__FUNCTION__,__LINE__);
		usleep(3000000);
	} while (1);

	return 0;
}


