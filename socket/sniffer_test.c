#include <stdio.h>
#include <signal.h>
#include <linux/ip.h>
#include <linux/if_ether.h>
#include <unistd.h>

#include "sniffer.h"


/******************************************************************************
*******************************************************************************/
static void int_handler(int s);
static int alive_match(unsigned char *eth_data, int size, void *data);
static int alive_action(unsigned char *eth_data, int size, void *data);


static sniffer_filter_t m_filter =
{
	.match = alive_match,
	.action = alive_action,
	.data = "wlan0",
};
static sinffer_t snf = {0};
static int m_running = 1;


int main(int argc, char *argv[])
{
	struct sigaction sigint_handler;
	
	sigint_handler.sa_handler = int_handler;
	sigemptyset(&sigint_handler.sa_mask);
	sigint_handler.sa_flags = 0;
	sigaction(SIGINT,&sigint_handler,NULL);
	
	sniffer_init(&snf, 0);
	register_sniffer(&snf,&m_filter);
	sniffer_start(&snf);
	
	do {
		usleep(1000000);
	} while(m_running);
	
	return 0;
}


/******************************************************************************
*******************************************************************************/
static void int_handler(int s)
{
	fprintf(stderr,"Caught signal %d\n",s);
	sniffer_destroy(&snf);
	m_running = 0;
}

/*
一帧心跳包：红色的为每一帧心跳包都一样的，可以作为抓包时的特征点。
66 88 66 88 01 18 00 08 00 00 00 a1 01 32 00 00 00 00 00 00 00 00 00 01 00 00 fd 84 71 74 9d 15
************YB_SERVER_MSG_HEAD_S**************(协议公共头)
magic 66886688   不变，所有消息一样
msg_ver 01    协议版本号，所有消息一样
msg_headlen 18    头长度，心跳包，协商消息一样，其他消息有子协议头，长度不一样。
text_len 0008   正文长度

msg_no 000000a1   消息序号，会变
msg_attr 01320000   MSG_ATTR_U   01代表发出心跳。
mask_code 00000000 mask码
ack_no 00000001  回应消息序号，会变
*/
static int alive_match(unsigned char *eth_data, int size, void *data)
{
	struct iphdr *iph = (struct iphdr*)(eth_data + sizeof(struct ethhdr));
	
	if (iph->protocol != 17) { //check UDP Protocol
		return 0;
	}
	
	if(!(eth_data[38]==0x00 && eth_data[39]==0x28)) { //check user data len
		return 0;
	}
	
	if (!((0x66 == eth_data[42]) && (0x88 == eth_data[43]) && (0x66 == eth_data[44]) && (0x88 == eth_data[45]) && \
			(0x18 == eth_data[47]) && (0x00 == eth_data[48]) && (0x08 == eth_data[49]) && (0x01 == eth_data[54]))
	   ) { //check heartbeat msg
		return 0;
	}
	
	return 1;
}

static int alive_action(unsigned char *eth_data, int size, void *data)
{
	int i = 0;
	fprintf(stderr,"\n alive eth data: \n");
	for (i=0; i<size; i++) {
		fprintf(stderr,"%02x",eth_data[i]);
	}
	fprintf(stderr,"\n");
	
	return 0;
}



