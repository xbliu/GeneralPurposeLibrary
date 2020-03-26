#ifndef UDP_CLIENT_H_
#define UDP_CLIENT_H_

#include "skt_base.h"

socket_base_t *udp_client_init(int type, int port, char *ip_addr);
int  udp_client_send(socket_base_t *skt_base, unsigned char *buf, int size);
int  udp_client_recv(socket_base_t *skt_base, unsigned char *buf, int size);
int  udp_client_destroy(socket_base_t *skt_base);

#endif

