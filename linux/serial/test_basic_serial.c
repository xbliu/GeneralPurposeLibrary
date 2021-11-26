#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include "basic_serial.h"


typedef struct {
	uint8_t sfd;      // 帧起始符，0x55；
	uint8_t len;      // 数据段长度，0x04
	uint8_t cmd;      // 命令字，0x01
	uint8_t dat[4];   // 数据段，此处为待设定的时间，单位：ms，小端
	uint8_t sum;      // 8bit累加和校验码，从len开始，sum前结束
	uint8_t eof;      // 帧结束符，0xAA；
} msg_set_tim_t;


static uint64_t get_sys_time_ns()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC,&ts);
    
    return (uint64_t)(((uint64_t)(ts.tv_sec))*1000000000 +(uint64_t)(ts.tv_nsec));
}

static uint64_t get_sys_time_ms()
{
	return (get_sys_time_ns() / 1000000);
}


int main(int argc, char *argv[])
{
	int fd = 0;
	int ret = 0;
	uint32_t cur_time = 0;

	if (argc < 2) {
		printf("please enter uart name!\n");
		return 0;
	}
	
	fd = serial_open(argv[1]);
	if (fd < 0) {
		printf("<%s:%d> open uart failed!\n", __FUNCTION__, __LINE__);
		return 0;
	}

	serial_set_conf(fd, 115200, 8, 1, 'n');
	printf("<%s:%d> uart name[%s] \n", __FUNCTION__, __LINE__, argv[1]);

	msg_set_tim_t set_tim;
	set_tim.sfd = 0x55;
	set_tim.len = 0x4;
	set_tim.cmd = 0x1;
	set_tim.eof = 0xAA;
	do {

		cur_time = get_sys_time_ms() & 0xFFFFFFFF;
		set_tim.dat[0] = (cur_time & 0xFF);
		set_tim.dat[1] = ((cur_time >> 8) & 0xFF);
		set_tim.dat[2] = ((cur_time >> 16) & 0xFF);
		set_tim.dat[3] = ((cur_time >> 24) & 0xFF);
		set_tim.sum = set_tim.len + set_tim.cmd + set_tim.dat[0] + set_tim.dat[1] + set_tim.dat[2] + set_tim.dat[3];
		ret = write(fd, &set_tim, sizeof(msg_set_tim_t));

		printf("<%s:%d> %d [%d %d %d %d] num[%d]\n", __FUNCTION__, __LINE__, \
			   cur_time, set_tim.dat[0], set_tim.dat[1], set_tim.dat[2], set_tim.dat[3], ret);

		usleep(10000000);
	} while (1);

	serial_close(fd);

	return 0;
}

