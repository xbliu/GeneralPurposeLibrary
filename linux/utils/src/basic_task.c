#include "basic_task.h"
#include "log_adapter.h"

#include <errno.h>
#include <signal.h>
#include <string.h>

#ifndef _ANDROID_
#include <execinfo.h>
#endif


#define MAX_BACKTRACE_NUM (32)
#define MAX_THREAD_NAME_LEN (32)


static int set_pthread_attr(pthread_attr_t *attr, int piority, int  cpuid)
{
	struct sched_param param;
	cpu_set_t cpu_mask;
	
	if (pthread_attr_init(attr)) {
		LOG_ERROR(LOG_MOD_UTILS, "pthread attr init err %d: %s \n", errno, strerror(errno));
		return -1;
	}

#if 0
	if (pthread_attr_setinheritsched(attr, PTHREAD_EXPLICIT_SCHED)) {
		printf("pthread attr setinheritsched err %d: %s \n", errno, strerror(errno));
		goto err_destroy;
	}
#endif

	if (pthread_attr_setschedpolicy(attr, SCHED_RR)) {
		goto err_destroy;
	}

	if (0 != piority) {
		param.sched_priority = piority;
		if (pthread_attr_setschedparam(attr, &param)) {
			LOG_ERROR(LOG_MOD_UTILS, "pthread attr setschedparam err %d: %s \n", errno, strerror(errno));
			goto err_destroy;
		}
	}

#ifndef _ANDROID_
	if (-1 != cpuid) {
		CPU_ZERO(&cpu_mask);
		CPU_SET(cpuid, &cpu_mask);
		if (pthread_attr_setaffinity_np(attr, sizeof(cpu_set_t), &cpu_mask)) {
			LOG_ERROR(LOG_MOD_UTILS, "pthread attr setaffinity np err %d: %s \n", errno, strerror(errno));
			goto err_destroy;
		}
	}
#endif

	return 0;

err_destroy:
	pthread_attr_destroy(attr);
	return -1;
}

static void signal_handler(int signum)
{
#ifndef _ANDROID_
	char thread_name[MAX_THREAD_NAME_LEN] = {0};
	pthread_getname_np(pthread_self(), thread_name, MAX_THREAD_NAME_LEN);

	LOG_INFO(LOG_MOD_UTILS, "thread %s catch a signal %d and may be crashed!\n", thread_name, signum);

	if ((SIGSEGV == signum) || (SIGABRT == signum)) {
		dump_thread_stack();
	}
#endif
}

static void *signal_task(void *arg)
{
	task_t *task = (task_t *)arg;

	signal(SIGSEGV, signal_handler);
	signal(SIGILL, signal_handler);
	signal(SIGABRT, signal_handler);
	signal(SIGBUS, signal_handler);
	signal(SIGFPE, signal_handler);
	signal(SIGSYS, signal_handler);

	return task->entry(task->arg);
}


void dump_thread_stack()
{
#ifndef _ANDROID_
	int i = 0;
	int size = 0;
    void *buffer[MAX_BACKTRACE_NUM] = {0};
	char **symbol = NULL;
	char thread_name[MAX_THREAD_NAME_LEN] = {0};

	pthread_getname_np(pthread_self(), thread_name, MAX_THREAD_NAME_LEN);
    size = backtrace(buffer, MAX_BACKTRACE_NUM);
    symbol = backtrace_symbols(buffer, size);
	
    LOG_INFO(LOG_MOD_UTILS, "[%15s]Seg-fault backtrack:\n",thread_name);
    for (i = 0; i < size; ++i) {
      LOG_INFO(LOG_MOD_UTILS, "[%15s][%3d/%3d]: %s\n", thread_name, i, size, symbol[i]);
    }
    free(symbol);
#else
	//to do android
#endif
}

int task_init(task_t *task)
{
	memset(task,0,sizeof(task_t));
	task->cpuid = -1;

	return 0;
}

task_t *task_alloc_init()
{
	task_t *task = (task_t *)malloc(sizeof(task_t));
	if (!task) {
		LOG_ERROR(LOG_MOD_UTILS, "no mem for task!\n");
		return NULL;
	}

	task_init(task);
	return task;
}

int task_free(task_t *task)
{
	if (task) {
		free(task);
	}

	return 0;
}

int task_set_param(task_t *task, char *name, task_func entry, void *arg)
{
	task->name = name;
	task->arg = arg;
	task->entry = entry;
	return 0;
}

int task_create(task_t *task, pthread_t *tid)
{
	int ret = 0;
	pthread_attr_t attr;

	ret = set_pthread_attr(&attr,task->piority,task->cpuid);
	if (ret < 0) {
		LOG_ERROR(LOG_MOD_UTILS, "task set piority cpuid failed!\n");
		goto err_set;
	}

	if (task->debug_on) {
		ret = pthread_create(tid,&attr,signal_task,task);
	} else {
		ret = pthread_create(tid,&attr,task->entry,task->arg);
	}

	if (ret < 0) {
		ret = -1;
		LOG_ERROR(LOG_MOD_UTILS, "failed to create thread:%s!\n", strerror(errno));
		goto err_create;
	}

#ifndef _ANDROID_
	if(task->name) {
		pthread_setname_np(*tid, task->name);
	}
#endif

	if (task->is_detach) {
		pthread_detach(*tid);
	}

	return 0;

err_create:
	pthread_attr_destroy(&attr);
err_set:
	return ret;
}

int task_register_signal(void)
{
	signal(SIGSEGV, signal_handler);
	signal(SIGILL, signal_handler);
	signal(SIGABRT, signal_handler);
	signal(SIGBUS, signal_handler);
	signal(SIGFPE, signal_handler);
	signal(SIGSYS, signal_handler);

	return 0;
}

