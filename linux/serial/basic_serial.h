#ifndef _BASIC_SERIAL_H_
#define _BASIC_SERIAL_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


int serial_open(char *dev_name);
int serial_set_conf(int fd, int speed, int data_bits, int stop_bits, char parity);
int serial_set_databits(int fd, int data_bits);
int serial_set_speed(int fd, int speed);
int serial_close(int fd);



#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* __cplusplus */

#endif


