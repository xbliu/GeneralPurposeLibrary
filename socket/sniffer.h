#ifndef SNIFFER_H_
#define SNIFFER_H_

#include "list/klist.h"
#include "pthread/pthread_model.h"

void *sniffer_thread(void *arg);

typedef int (*match_func)(unsigned char *eth_data, int size, void *data);
typedef int (*action_func)(unsigned char *eth_data, int size, void *data);

typedef struct {
	struct list_head entry;
	match_func match;
	action_func action;
	
	void *data;
} sniffer_filter_t;

typedef struct {
	int sockfd;
	unsigned char *buffer;
	int buf_size;
	
	pthread_model_t *model_dynamic;
	struct list_head filter_head;
	int is_debug;
} sinffer_t;

int sniffer_init(sinffer_t *snf, int debug);
int register_sniffer(sinffer_t *snf, sniffer_filter_t *filter);
int sniffer_start(sinffer_t *snf);
int sniffer_destroy(sinffer_t *snf);

#endif
