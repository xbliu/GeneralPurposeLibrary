#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "udp_client.h"


typedef struct
{
    unsigned char     rsv[2];               
    unsigned short    route_port;           
    unsigned int      route_ip;             
} YB_SERVER_PING_S,*PYB_SERVER_PING_S;

/*yb msg head define  */
typedef struct  
{ 
    uint32_t  msg_type:8; 
    uint32_t  priority:4; 
    uint32_t  mask_flag:1; 
    uint32_t  ack_req:1; 
    uint32_t  ack_flag:1; 
    uint32_t  rsv1:1; 
    uint32_t  split_msg:1; 
    uint32_t  last_msg:1; 
    uint32_t  curr_slice:3; 
    uint32_t  total_slice:3; 
    uint32_t  rsv2:8; 
} MSG_ATTR_S;

typedef union 
{
    uint32_t      uint32_val;
    MSG_ATTR_S    bit_val;
} MSG_ATTR_U;

typedef struct 
{
    unsigned int   magic;
    unsigned char  msg_ver;
    unsigned char  msg_headlen;    
    unsigned short text_len; 
    unsigned int   msg_no;
    MSG_ATTR_U     msg_attr;
    unsigned int   mask_code;
    unsigned int   ack_no;    
} YB_SERVER_MSG_HEAD_S, *PYB_SERVER_MSG_HEAD_S;


#define SERVER_MSG_HEAD_LEN  sizeof(YB_SERVER_MSG_HEAD_S)
#define YB_PRIVA_CODE (0X66886688)
#define YB_MSG_VER (1)
#define YB_MSG_TYPE_PING (1)


static socket_t socket_client = {
	.socket_base_init = udp_client_init,
	.socket_base_send = udp_client_send,
	.socket_base_recv = udp_client_recv,
	.socket_base_destroy = udp_client_destroy,
};


int main(char *argc, char *argv[])
{
	int send_times = 20;
	socket_base_t *skt_base = NULL;
	
	int msg_no = 0;
	char send_buf[1024] = {0};    
    YB_SERVER_MSG_HEAD_S  ping_header  = {0};
    YB_SERVER_PING_S      ping_text = {0};
	
	
    ping_header.magic   = htonl(YB_PRIVA_CODE);
    ping_header.msg_ver = YB_MSG_VER;
    ping_header.msg_headlen = SERVER_MSG_HEAD_LEN;

    ping_header.msg_no = htonl(msg_no++);
    ping_header.msg_attr.bit_val.msg_type  = YB_MSG_TYPE_PING;
    ping_header.msg_attr.bit_val.mask_flag = 0;
    ping_header.msg_attr.bit_val.ack_req   = 1;
    ping_header.msg_attr.bit_val.ack_flag  = 0;
    ping_header.msg_attr.bit_val.split_msg = 0;

    ping_header.mask_code = 0;
    ping_header.text_len  = htons(sizeof(ping_text));
    ping_text.route_port  = htons(3355);
    ping_text.route_ip    = htonl(0x20);
	
    memcpy(send_buf, &ping_header, SERVER_MSG_HEAD_LEN);
    memcpy((send_buf + SERVER_MSG_HEAD_LEN), &ping_text, (sizeof(ping_text)));
	
	skt_base = socket_client.socket_base_init(SOCK_DGRAM,3355,"127.0.0.1");
	if (!skt_base) {
		fprintf(stderr,"open socket client error!\n");
		return -1;
	}
	
	do {
		socket_client.socket_base_send(skt_base,send_buf,sizeof(send_buf));
		usleep(2000000);
	} while (--send_times);
	
	socket_client.socket_base_destroy(skt_base);
	
	return 0;
}

