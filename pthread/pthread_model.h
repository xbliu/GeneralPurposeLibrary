#ifndef PTHREAD_MODEL_H_
#define PTHREAD_MODEL_H_

#include <pthread.h>

typedef void *(*pthread_func) (void *);

typedef struct {
	pthread_func start_routine;
	void *arg;
	pthread_t thread_id;
	void *priv;
} pthread_model_t;

pthread_model_t *pthread_model_create(pthread_func func, void *arg);
int pthread_model_init(pthread_model_t *model,pthread_func func, void *arg);
int pthread_model_start(pthread_model_t *model);
int pthread_model_destory(pthread_model_t *model);

#endif

