#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "skt_base.h"


socket_base_t *udp_client_init(int type, int port, char *ip_addr)
{
	int sockfd = 0;
	struct sockaddr_in sin;
	socket_base_t *skt_base = NULL;
	
	skt_base = (socket_base_t *)calloc(sizeof(socket_base_t),1);
	if (!skt_base) {
		fprintf(stderr,"can't alloc memory!\n");
		return NULL;
	}
	
	sockfd = socket(AF_INET,type,0);
	if (sockfd < 0) {
		fprintf(stderr,"can't open socket:[%s,%d]!\n",ip_addr,port);
		
		free(skt_base);
		return NULL;
	}
	
	memset(&sin,0,sizeof(struct sockaddr_in));
	sin.sin_family = AF_INET;
   	sin.sin_addr.s_addr = inet_addr(ip_addr);
	sin.sin_port = htons(port);
	
	skt_base->fd = sockfd;
	skt_base->sin = sin;
	
	return skt_base;

}

int  udp_client_send(socket_base_t *skt_base, char *buf, int size)
{
	int ret = 0;
	int try_times = 3;
	
	if (!skt_base) {
		fprintf(stderr,"null parameter!\n");
		return -1;
	}
	
	do {
		ret = sendto(skt_base->fd,buf,size,0,(struct sockaddr*)&skt_base->sin,sizeof(struct sockaddr));
		if (EAGAIN != ret) {
			break;
		}
	} while (try_times--);
	
	if (ret < 0) {
		fprintf(stderr,"send failed:%s !\n",strerror(errno));
	}
	
	return ret;
}

int  udp_client_recv(socket_base_t *skt_base, char *buf, int size)
{
	int ret = 0;
	int try_times = 3;
	int addr_len = sizeof(struct sockaddr);
	
	if (!skt_base) {
		fprintf(stderr,"null parameter!\n");
		return -1;
	}
	
	do {
		ret = recvfrom(skt_base->fd,buf,size,0,(struct sockaddr*)&skt_base->sin,&addr_len);
		if (EAGAIN != ret) {
			break;
		}
	} while (try_times--);
	
	if (ret < 0) {
		fprintf(stderr,"send failed:%s !\n",strerror(errno));
	}
	
	return ret;
}

int  udp_client_destroy (socket_base_t *skt_base)
{
	int ret = 0;
	
	if (!skt_base) {
		fprintf(stderr,"null parameter!\n");
		return -1;
	}
	
	close(skt_base->fd);
	free(skt_base);
	
	return 0;
}

