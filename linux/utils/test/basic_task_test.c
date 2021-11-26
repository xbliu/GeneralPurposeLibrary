#include <stdio.h>
#include <stdlib.h>

#include "basic_task.h"


#if 0
typedef struct {
	uint8_t running;
	pthread_t thd_id;
} task_context_t;
#endif

static void *basic_task_test(void *arg)
{
	task_context_t *context = (task_context_t *)arg;
	printf("<%s:%d> ===========\n",__FUNCTION__,__LINE__);
	while (context->running) {
		usleep(1000000);
		printf("<%s:%d> ===========\n",__FUNCTION__,__LINE__);
	}
}

int main(int argc, char *argv[])
{
	task_context_t context;
	task_t task;

	context.running = 1;
	context.tid = 0;

	task_init(&task);
	task.name = "basic_task_test";
	task.arg = &context;
	task.entry = basic_task_test;
	task_create(&task, &context.tid);
	
	do {
		usleep(3000000);
		//printf("<%s:%d> ===========\n",__FUNCTION__,__LINE__);
	} while (context.running);
	
	return 0;
}


