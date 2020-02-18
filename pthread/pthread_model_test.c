#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "pthread_model.h"

static void *model_static_test(void *arg)
{
	printf("model static test!\n");
	return NULL;
}

static void *model_dynamic_test(void *arg)
{
	printf("model dynamic test!\n");
	return NULL;
}

int main(int argc, char *argv[])
{
	/*static init test*/
	pthread_model_t model;
	pthread_model_init(&model,model_static_test,NULL);
	pthread_model_start(&model);
	usleep(2000000);
	pthread_model_destory(&model);
	
	usleep(2000000);
	
	/*dynamic init test*/
	pthread_model_t *model_dynamic = pthread_model_create(model_dynamic_test,NULL);
	pthread_model_start(model_dynamic);
	usleep(2000000);
	pthread_model_destory(model_dynamic);
	
	usleep(2000000);
	
	return 0;
}

