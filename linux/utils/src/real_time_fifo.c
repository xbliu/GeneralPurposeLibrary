#include "real_time_fifo.h"
#include "log_adapter.h"

#include <stdio.h>


int real_time_fifo_init(real_time_fifo_t *rfifo, int size, release_func func)
{
	if (!rfifo) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal arguments!\n");
		return -1;
	}

	rfifo->size = size;
	rfifo->in = rfifo->out = 0;
	rfifo->release = func;
	rfifo->is_initial = 1;
	INIT_LIST_HEAD(&rfifo->head);
	pthread_mutex_init(&rfifo->mutex,NULL);

	return 0;
}

int real_time_fifo_put(real_time_fifo_t *rfifo, struct list_head *list)
{
	struct list_head *ptr = NULL;

	if (!rfifo || !list) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal arguments!\n");
		return -1;
	}

	if (0 == rfifo->is_initial) {
		LOG_ERROR(LOG_MOD_UTILS, "not init!\n");
		return -1;
	}

	pthread_mutex_lock(&rfifo->mutex);
	/*not empty for the case: 0 == size*/
	if ((rfifo->out - rfifo->in == rfifo->size) && !list_empty(&rfifo->head)) {
			ptr = rfifo->head.next;
			list_del(ptr);
			(NULL == rfifo->release) ? : rfifo->release(ptr);
	}
	rfifo->in++;
	pthread_mutex_unlock(&rfifo->mutex);
	list_add_tail(list, &rfifo->head);
	rfifo->out++;

	return 0;
}

int real_time_fifo_get(real_time_fifo_t *rfifo, struct list_head **list)
{
	if (!rfifo || !list) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal arguments!\n");
		return -1;
	}

	if (0 == rfifo->is_initial) {
		LOG_ERROR(LOG_MOD_UTILS, "not init!\n");
		return -1;
	}

	pthread_mutex_lock(&rfifo->mutex);
	if (list_empty(&rfifo->head)) {
		pthread_mutex_unlock(&rfifo->mutex);
		return -1;
	}

	*list = rfifo->head.next;
	list_del(*list);
	rfifo->in++;
	pthread_mutex_unlock(&rfifo->mutex);

	return 0;
}

int real_time_fifo_destroy(real_time_fifo_t *rfifo)
{
	struct list_head *pos = NULL;
	struct list_head *next = NULL;

	if (!rfifo) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal arguments!\n");
		return -1;
	}

	list_for_each_safe(pos, next, &rfifo->head) {
		list_del(pos);
		(NULL == rfifo->release) ? : rfifo->release(pos);
	}

	rfifo->in = rfifo->out = 0;
	rfifo->is_initial = 0;
	pthread_mutex_destroy(&rfifo->mutex);

	return 0;
}

