#include "gpio_irq.h"
#include "gpio_base.h"

#include <stdio.h>
#include <stdlib.h>

int register_gpio_irq(gpio_irq_t *gpio_irq, int ngpio, gpio_irq_func func, void *irq_param)
{
	gpio_irq->ngpio = ngpio;
	gpio_irq->irq_param = irq_param;
	gpio_irq->func = func;
	
	return 0;
}

int gpio_irq_create(gpio_irq_t *gpio_irq, gpio_edge_e edge)
{
	int fd = 0;
	char str[64] = {0};
	
	if (!gpio_base_is_exist(gpio_irq->ngpio)) {
		gpio_base_export(gpio_irq->ngpio);
	}
	
	gpio_base_set_direction(gpio_irq->ngpio,GPIO_DIR_INPUT);
	gpio_base_set_edge(gpio_irq->ngpio,edge);
	
	snprintf(str,sizeof(str),"%s/gpio%d/%s",GPIO_BASE_DIR,ngpio,"value");
	fd = open(str,O_WRONLY);
    if(fd < 0) {
		printf("path %s  open gpio %d failed\n", str,ngpio);
        goto err_out;
    }
	
	//first clear it
	read(fd, str, sizeof(str));
	lseek(fd, 0, SEEK_SET);
	
	//open gpio
	gpio_irq->fd = fd;
	
	return 0;
err_out:
	return -1;
}

int gpio_irq_destroy(gpio_irq_t *gpio_irq)
{
	//to do
	close(gpio_irq->fd);
	
	return 0;
}

int gpio_irq_monitor(void *arg)
{
	int ret = 0;
	fd_set rfds;
	struct timeval tv;
	char buf[4] = {0};
	gpio_irq_t *gpio_irq = (gpio_irq_t *)arg;
	
	FD_ZERO(&rfds);
	FD_SET(gpio_irq->fd,&rfds);
	
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	ret = select(gpio_irq->fd+1,&rfds,NULL,NULL,&timeout);
	if (ret < 0) {
		perror("select");
	} else if (0 == ret) {
		printf("select timeout!\n");
	} else {
		if (FD_ISSET(gpio_irq->fd, &rfds)) {
			memset(buf, 0, sizeof(buf));
			read(gpio_irq->fd, buf, sizeof(buf));
			lseek(gpio_irq->fd, 0, SEEK_SET);
			
			(gpio_irq->func)(gpio_irq->irq_param,atoi(buf));
		}
	}
	
	return ret;
}
