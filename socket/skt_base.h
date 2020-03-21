#ifndef SKT_BASE_H_
#define SKT_BASE_H_

typedef struct {
	int fd;
	struct sockaddr_in sin;
} socket_base_t;

typedef struct 
{
	socket_base_t *(*socket_base_init)(int type, int port, char *ip_addr);
	int (*socket_base_send)(socket_base_t *handler, char *buf, int size);
	int (*socket_base_recv)(socket_base_t *handler, char *buf, int size);
	int (*socket_base_destroy)(socket_base_t *handler);
} socket_t;

#endif
