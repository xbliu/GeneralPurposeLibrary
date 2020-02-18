#include "pthread_model.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int running; /*0 bit:run 1 bit:dynamic init*/
	pthread_attr_t attr;
} pthread_handle_t;


static void *pthread_model_execute(void *param)
{
	pthread_model_t *model = (pthread_model_t *)param;
	pthread_handle_t *handle = (pthread_handle_t *)(&model->priv);;
	
	if (!model->start_routine) {
		fprintf(stderr,"no start_routine for pthread_model!\n");
		return NULL;
	}
	
	while (handle->running & 0x1) {
		model->start_routine(model->arg);
	}
	
	return NULL;
}

pthread_model_t *pthread_model_create(pthread_func func, void *arg)
{
	int ret = 0;
	pthread_handle_t *handle = NULL;
	
	pthread_model_t *model = (pthread_model_t *)malloc(sizeof(pthread_model_t) + sizeof(pthread_handle_t));
	if (!model) {
		fprintf(stderr,"no mem for pthread_model!\n");
		return NULL;
	}
	
	handle = (pthread_handle_t *)(&model->priv);
	ret = pthread_attr_init(&handle->attr);
	if (0 != ret) {
		fprintf(stderr,"can't get thread attr!\n");
		free(model);
		return NULL;
	}
	
	//pthread_attr_getstack(&handle->attr, &stackaddr, &stacksize);
	pthread_attr_setdetachstate(&handle->attr, PTHREAD_CREATE_DETACHED);
	
	model->start_routine = func;
	model->arg = arg;
	handle->running = 1 << 1; /*dynamic init*/
	
	return model;
}

int pthread_model_init(pthread_model_t *model, pthread_func func, void *arg)
{
	pthread_handle_t *handle = NULL;
	
	handle = (pthread_handle_t *)(&model->priv);
	model->start_routine = func;
	model->arg = arg;
	handle->running = 0;
	
	return 0;
}

int pthread_model_start(pthread_model_t *model)
{
	int ret = 0;
	pthread_handle_t *handle = NULL;
	pthread_attr_t *attr = NULL;
	
	handle = (pthread_handle_t *)(&model->priv);
	handle->running |= (1 << 0);
	if (handle->running & (1 << 1)) {
	    attr = &handle->attr;
	}
	
	ret = pthread_create(&model->thread_id, attr, pthread_model_execute, model);
	if (0 != ret) {
		fprintf(stderr,"create thread failed!\n");
		return -1;
	}
	
	return 0;
}

int pthread_model_destory(pthread_model_t *model)
{
	pthread_handle_t *handle = NULL;
	
	handle = (pthread_handle_t *)(&model->priv);
	handle->running &= ~(1<<0);
	pthread_cancel(model->thread_id);
	
	if (handle->running & (1 << 1)) {
		pthread_attr_destroy(&handle->attr);
		free(model);
	}
	
	return 0;
}
