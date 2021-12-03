#include "thread_pool.h"

#include<unistd.h>

		
#ifndef MAX
#define MAX(a, b) (((a) < (b)) ? (b) : (a))
#endif

//may move to common macro
#ifndef offsetof
#define offsetof(type, member) ((unsigned int) &((type *)0)->member)
#endif

#ifndef container_of
#define container_of(ptr, type, member)  ((type *)( (char *)(ptr) - offsetof(type,member) ))
#endif

#define DEBUG_PRINT(fmt...) //printf(fmt)


static int get_cpu_num();
static int get_online_cpu_num();
static int is_online_cpu(int cpu);

static void *worker_func(void *arg);
static int destroy_worker(worker_t *worker);
static worker_t *create_worker(int index, int cpu, int piority, int is_detach);
static worker_t *thread_pool_create_worker(thread_pool_t *thread_pool, int cpu, int piority, int is_detach);
static int destroy_all_worker(struct list_head *head);
static int add_work_to_worker(worker_t *worker, work_t *work);
static worker_t *find_worker(struct list_head *worker_head, int cpu_mask, int is_fuzzy);
static int set_worker_cpu_mask(worker_t *worker, int cpu_mask);


int thread_pool_init(thread_pool_t *thread_pool, int min_worker_num, int max_worker_num)
{
	int i = 0;
	int cpu_num = 0;
	int online_cpu_num = 0;
	worker_t *worker = NULL;

	if (!thread_pool || (min_worker_num > max_worker_num)) {
		printf("<%s:%d> illegal param!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	memset(thread_pool, 0, sizeof(thread_pool_t));
	INIT_LIST_HEAD(&thread_pool->idle_head);
	INIT_LIST_HEAD(&thread_pool->busy_head);

	online_cpu_num = get_online_cpu_num();
	thread_pool->min_worker_num = MAX(min_worker_num, online_cpu_num);
	thread_pool->max_worker_num = max_worker_num;
	pthread_mutex_init(&thread_pool->worker_mtx, NULL);

	/*min worker: cpu worker*/
	cpu_num = get_cpu_num();
	for (i=0; i<cpu_num; i++) {
		if (!is_online_cpu(i)) {
			continue;
		}
		thread_pool_create_worker(thread_pool, i, 0, 1);
	}

	/*min worker: general worker*/
	for (i=thread_pool->min_worker_num-online_cpu_num; i>0; i--) {
		thread_pool_create_worker(thread_pool, -1, 0, 1);
	}

	return 0;
}

int thread_pool_destroy(thread_pool_t *thread_pool)
{
	destroy_all_worker(&thread_pool->idle_head);
	destroy_all_worker(&thread_pool->busy_head);
	pthread_mutex_destroy(&thread_pool->worker_mtx);

	return 0;
}

int thread_pool_add_work(thread_pool_t *thread_pool, work_t *work)
{
	return thread_pool_add_cpu_work(thread_pool, -1, work);
}

int thread_pool_add_cpu_work(thread_pool_t *thread_pool, int cpu_mask, work_t *work)
{
	worker_t *worker = NULL;

	worker = find_worker(&thread_pool->idle_head, cpu_mask, 1);
	if (!worker) {
		worker = thread_pool_create_worker(thread_pool, -1, 0, 1);
	}

	if (!worker) {
		printf("<%s:%d> no more worker[%d %d]!\n", __FUNCTION__, __LINE__, thread_pool->cur_worker_num, thread_pool->max_worker_num);
		//to do: add to busy worker
		return -1;
	}

	if (!worker->is_bind_cpu) {
		set_worker_cpu_mask(worker, cpu_mask);
	}

	add_work_to_worker(worker, work);
	return 0;
}


/*
************************************static function************************************
*/
static int get_online_cpu_num()
{
	 /*
    * - _SC_NPROCESSORS_CONF: the number of processors configured
	* - _SC_NPROCESSORS_ONLN: the number of processors currently online (available)  
    */
	int active_cpu_num;
	active_cpu_num = sysconf(_SC_NPROCESSORS_ONLN);

	return MAX(0, active_cpu_num);
}

static int get_cpu_num()
{
	int cpu_num = 0;
	cpu_num = sysconf(_SC_NPROCESSORS_CONF);
	return MAX(0, cpu_num);
}

static int is_online_cpu(int cpu)
{
	int ret = 0;
	char buf[4] = {0};
	char path[128] = {0};

	snprintf(path, sizeof(path), "/sys/devices/system/cpu/cpu%d/online", cpu);
	ret = file_read_to_buffer(path, buf, sizeof(buf));
	if (0 != ret) {
		return 0;
	}

	if ('1' != buf[0]) {
		return 0;
	}
	
	return 1;
}

static int set_worker_cpu_mask(worker_t *worker, int cpu_mask)
{  
	int i = 0;
	cpu_set_t mask;
	CPU_ZERO(&mask);
	int cpu_num = get_cpu_num();

	for (i=0; i<cpu_num; i++) {
		if (!is_online_cpu(i)) {
			continue;
		}
		CPU_SET(i, &mask);
	}

	if (-1 == pthread_setaffinity_np(worker->task_ctx.tid, sizeof(mask), &mask)) {
        return -1;
    }

    return 0;
}

static worker_t *find_worker(struct list_head *worker_head, int cpu_mask, int is_fuzzy)
{
	int is_find = 0;
	worker_t *worker = NULL;
	struct list_head *pos = NULL;

	/*find the right worker, otherwise the last one*/
	list_for_each(pos, worker_head) {
		worker = container_of(pos, worker_t, entry);
		if ((worker->cpu_mask & cpu_mask) || (is_fuzzy && !worker->is_bind_cpu)) {
			is_find = 1;
			break;
		}
	}

	if (!is_find) {
		printf("<%s:%d> not found worker!\n", __FUNCTION__, __LINE__);
		return NULL;
	}
	
	printf("<%s:%d> find worker [%s] \n", __FUNCTION__, __LINE__, worker->name);
	return worker;
}

static int add_work_to_worker(worker_t *worker, work_t *work)
{
	int is_find = 0;
	struct list_head *pos = NULL;
	work_t *ptr = NULL;

	/*is it already in the list*/
	list_for_each(pos, &worker->work_head) {
		ptr = container_of(pos, work_t, entry);
		if (ptr == work) {
			is_find = 1;
			break;
		}
	}

	if (!is_find) {
		pthread_mutex_lock(&worker->work_mtx);
		if (WORK_STATUS_PENDING !=  work->status) {
			list_add_tail(&work->entry, &worker->work_head);
			work->status = WORK_STATUS_PENDING;
			worker->work_num++;
		}
		pthread_mutex_unlock(&worker->work_mtx);
	}

	pthread_cond_signal(&worker->wait_cond);

	return 0;
}

static int init_worker(worker_t *worker)
{
	if (!worker) {
		printf("<%s:%d> illegal param!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	memset(worker, 0, sizeof(worker_t));

	INIT_LIST_HEAD(&worker->work_head);
	INIT_LIST_HEAD(&worker->entry);
	pthread_mutex_init(&worker->work_mtx, NULL);
	pthread_mutex_init(&worker->wait_mtx, NULL);
	pthread_cond_init(&worker->wait_cond, NULL);

	return 0;
}

static worker_t *create_worker(int index, int cpu, int piority, int is_detach)
{
	int ret = 0;
	task_t task;
	worker_t *worker = NULL;

	worker = (worker_t *)malloc(sizeof(worker_t));
	if (!worker) {
		printf("<%s:%d> no mem for worker!\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	init_worker(worker);

	task_init(&task);
	task.is_detach = is_detach;
	task.cpuid = cpu;
	task.piority = piority;
	snprintf(worker->name, sizeof(worker->name), "worker_%d", index);
	task_set_param(&task, worker->name, worker_func, worker);
	worker->task_ctx.running = 1;
	ret = task_create(&task, &worker->task_ctx.tid);
	if (0 != ret) {
		printf("<%s:%d> create worker failed!\n", __FUNCTION__, __LINE__);
		free(worker);
		return NULL;
	}

	return worker;
}

static int destroy_worker(worker_t *worker)
{
	worker->task_ctx.running = 0;
	pthread_cond_signal(&worker->wait_cond);
	return 0;
}

static int add_worker(thread_pool_t *thread_pool, worker_t *worker)
{
	if (!thread_pool || !worker) {
		printf("<%s:%d> illegal param!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	list_add_tail(&worker->entry, &thread_pool->idle_head);
	worker->thread_pool = thread_pool;
	thread_pool->cur_worker_num++;

	return 0;
}

static void *worker_func(void *arg)
{
	worker_t *worker = NULL;
	work_t *work = NULL;
	struct list_head *ptr = NULL;

	worker = (worker_t *)arg;
	while (worker->task_ctx.running) {
		/*wait for the work to come*/
		pthread_mutex_lock(&worker->wait_mtx);
		while (list_empty(&worker->work_head)) {
			DEBUG_PRINT("<%s:%d> name[%s] start to wait work!\n", __FUNCTION__, __LINE__, worker->name);
			pthread_cond_wait(&worker->wait_cond, &worker->wait_mtx);
		}
		pthread_mutex_unlock(&worker->wait_mtx);

		if (worker->thread_pool) {
			list_move(&worker->entry, &worker->thread_pool->busy_head);
		}

		/*handle work*/
		DEBUG_PRINT("<%s:%d> name[%s] start to handle work!\n", __FUNCTION__, __LINE__, worker->name);
		while (!list_empty(&worker->work_head)) {
			ptr = worker->work_head.next;
			list_del(ptr);
			worker->work_num--;
			work = container_of(ptr, work_t, entry);
			work->status = WORK_STATUS_RUNNING;
			DEBUG_PRINT("<%s:%d> name[%s] call func!\n", __FUNCTION__, __LINE__, worker->name);
			work->func(work->arg);
			work->status = WORK_STATUS_CLEAR;
		}

		if (worker->thread_pool) {
			list_move(&worker->entry, &worker->thread_pool->idle_head);
		}
	}

	pthread_mutex_destroy(&worker->work_mtx);
	pthread_mutex_destroy(&worker->wait_mtx);
	pthread_cond_destroy(&worker->wait_cond);
	if (worker->thread_pool) {
		list_del(&worker->entry);
	}
	free(worker);

	return NULL;
}

static worker_t *thread_pool_create_worker(thread_pool_t *thread_pool, int cpu, int piority, int is_detach)
{
	worker_t *worker = NULL;

	if (!thread_pool) {
		printf("<%s:%d> illegal param!\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	worker = create_worker(thread_pool->cur_worker_num+1, cpu, piority, is_detach);
	if (!worker) {
		printf("<%s:%d> create worker failed!\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	pthread_mutex_lock(&thread_pool->worker_mtx);
	if (thread_pool->cur_worker_num >= thread_pool->max_worker_num) {
		pthread_mutex_unlock(&thread_pool->worker_mtx);
		/*cancel worker*/
		destroy_worker(worker);
		printf("<%s:%d> can not create worker!\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	if (-1 != cpu) {
		worker->is_bind_cpu = 1;
		worker->cpu_mask = (1 << cpu);
	} else {
		worker->is_bind_cpu = 0;
		worker->cpu_mask = 0xFFFFFFFF;
	}
	add_worker(thread_pool, worker);
	pthread_mutex_unlock(&thread_pool->worker_mtx);

	return worker;
}

static int destroy_all_worker(struct list_head *head)
{
	worker_t *worker = NULL;
	struct list_head *ptr = NULL;

	list_for_each(ptr, head) {
		worker = container_of(ptr, worker_t, entry);
		destroy_worker(worker);
	}

	return 0;
}

