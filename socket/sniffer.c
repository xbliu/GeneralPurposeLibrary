#include<netinet/in.h>
#include<errno.h>
#include<netdb.h>
#include<stdio.h> 
#include<stdlib.h>   
#include<string.h>
#include <stdbool.h>
 
#include<netinet/ip_icmp.h>  
#include<netinet/udp.h>   
#include<netinet/tcp.h>   
#include<netinet/ip.h>   
#include<netinet/if_ether.h>  
#include<net/ethernet.h>  
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/ioctl.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>

#include <linux/socket.h>
#include <netpacket/packet.h>
#include <net/if.h>


//#include "sniffer.h"

#define IFNAME "wlan0"

typedef struct dhd_priv_cmd {
	char *buf;
	int used_len;
	int total_len;
} dhd_priv_cmd;

typedef struct heartbeat_packet {
	unsigned char *buf;
	int size;
	
	char dst_mac[6];
	char src_ip[4];
	char dst_ip[4];
	char src_port[2];
	char dst_prot[2];
} heartbeat_packet_t;

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
int is_heartbeat(char *eth_data, int len)
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

/*
*****6+6+2+20+[40]+8<udp>+32<data> + 4 = 118<max>
*****bcmhd 心跳包指令:
*****参考如下命令来定时发送ethernet data：
	dhd_priv keep_alive <id> <perioid in ms> <ethernet data>
	dhd_priv keep_alive 1 30000 0x0014a54b164f000f66f45b7e08004500001e000040004011c52a0a8830700a88302513c413c4000a00000a0d
	前6 byte是destination目的地址mac，后面6 byte是本机mac
*/
int build_keep_alive_command(char *eth_data, int len, char *cmd)
{
	int i= 0;
	int copy_len = 0;
	//char cmd[256] = {0};
	
	strcat(cmd, "keep_alive 0 10000 0x");
	/*it's usually 78 bytes,up to 118 bytes,buffer does not need to be greater than 256 bytes*/
	copy_len = (len < 116 ? len : 116);
	for(i=0; i<copy_len; i+=4) {
		sprintf(cmd+strlen(cmd),"%02x%02x%02x%02x",eth_data[i],eth_data[i+1],eth_data[i+2],eth_data[i+3]);
	}
	
	return 0;
}

int send_priv_cmd(char *cmd, int len)
{
	int ret = 0;
	int sock_fd = 0;
	char ifname[IFNAMSIZ+1];
	struct ifreq ifr;
	dhd_priv_cmd priv_cmd;
	
	sock_fd = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock_fd < 0)  {
        perror("create socket failed.\n");
		return -1;
	}
	
	memset(&ifr, 0, sizeof(ifr));
	memset(&priv_cmd, 0, sizeof(priv_cmd));
	
	strcpy(ifname, IFNAME);
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	
	priv_cmd.buf = cmd;
	priv_cmd.used_len = len;
	priv_cmd.total_len = len;
	ifr.ifr_data = &priv_cmd;
	
	ret = ioctl(sock_fd, SIOCDEVPRIVATE + 1, &ifr);
	if (ret < 0) {
		printf("failed to issue private commands %d\n", ret);
		perror("ioctl_sock cmd Error.");
	}
	
	close(sock_fd);
	
	return ret;
}

void handle_packet(unsigned char* buffer, int size)
{
	char cmd[256] = {0};
	
	if (!is_heartbeat(buffer,size)) {
		return;
	}
	
	build_keep_alive_command(buffer,size,cmd);
	send_priv_cmd(cmd,sizeof(cmd));
}

//void *sniffer_thread(void *arg)
void main(void *arg)
{
    int saddr_size , data_size;
    struct sockaddr saddr;
	char ifname[IFNAMSIZ+1];
	
    unsigned char *buffer = (unsigned char *) malloc(1536); //max frame :1518
	if (!buffer) {
		printf("no mem alloc!\n");
		return;
	}
    
    printf("Starting sniffer thread.\n");

	strcpy(ifname, IFNAME);
    int raw_sock = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ;
    if(raw_sock < 0) {
        //Print the error with proper message
        perror("raw_sock Error");
        return;
    }
	
    while(1) {
        saddr_size = sizeof(saddr);
        data_size = recvfrom(raw_sock , buffer , sizeof(buffer) , 0 , &saddr , (socklen_t*)&saddr_size);
        if(data_size < 0 ) {
            printf("Recvfrom error , failed to get packets\n");
            continue;
        }
		
        //Now process the packet
        handle_packet(buffer , data_size);
    }
	
    close(raw_sock);
	free(buffer);
}


 
