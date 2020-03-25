#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>

#include "sniffer.h"


#define MAX_ETH_FRAME (1536)


static void *sniffer_model(void *arg);


int sniffer_init(sinffer_t *snf, int debug)
{
	assert(snf);
	
	snf->buf_size = MAX_ETH_FRAME;
	snf->buffer = (unsigned char *) malloc(snf->buf_size); //max frame :1518
	if (!snf->buffer) {
		fprintf(stderr,"no mem alloc!\n");
		goto err_alloc;
	}
	
	snf->sockfd = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ;
    if(snf->sockfd < 0) {
        //Print the error with proper message
        fprintf(stderr,"raw_sock Error");
        goto err_skt;
    }
	
	INIT_LIST_HEAD(&snf->filter_head);
	snf->model_dynamic = NULL;
	snf->is_debug = debug;
	
	return 0;
err_skt:
	free(snf->buffer);
err_alloc:
	return -1;	
}

int register_sniffer(sinffer_t *snf, sniffer_filter_t *filter)
{
	assert(snf);
	
	list_add(&filter->entry,&snf->filter_head);
	return 0;
}

int sniffer_start(sinffer_t *snf)
{
	assert(snf);
	
	snf->model_dynamic = pthread_model_create(sniffer_model,snf);
	pthread_model_start(snf->model_dynamic);
	
	return 0;
}

int sniffer_destroy(sinffer_t *snf)
{
	assert(snf);
	
	pthread_model_destory(snf->model_dynamic);
	if (snf->buffer) {
		free(snf->buffer);
	}
	close(snf->sockfd);
	
	return 0;
}

/******************************************************************************
*******************************************************************************/
static void show_eth_data(unsigned char *buffer, int size)
{
	int i = 0;
	fprintf(stderr,"\n eth data: ");
	for (i=0; i<size; i++) {
		fprintf(stderr," %x ",buffer[i]);
	}
	fprintf(stderr,"\n");
}

static void *sniffer_model(void *arg)
{
	int saddr_size , data_size;
    struct sockaddr saddr;
	sniffer_filter_t *filter = NULL;
	
	sinffer_t *snf = (sinffer_t *)arg;
	
	saddr_size = sizeof(saddr);
	data_size = recvfrom(snf->sockfd , snf->buffer , snf->buf_size , 0 , &saddr , (socklen_t*)&saddr_size);
	if(data_size < 0 ) {
		printf("Recvfrom error , failed to get packets\n");
		goto err_out;
	}
	
	if (snf->is_debug) {
		show_eth_data(snf->buffer,data_size);
	}
	
	//Now process the packet
	list_for_each_entry(filter, &snf->filter_head, entry) {
		if (!filter->match(snf->buffer,data_size,filter->data)) {
			continue;
		}
		
		filter->action(snf->buffer,data_size,filter->data);
	}

err_out:
	return snf;
}
