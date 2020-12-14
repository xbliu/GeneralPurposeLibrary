#include <stdio.h>
#include <stdlib.h>

#include "gpio_ctrl.h"

int main(int argc, char *argv[])
{
	int ret = 0;
	int ngpio = 0;
	
	if (argc < 2) {
		fprintf(stderr,"Usage: gpio_base test ngpio !\n");
		return 0;
	}
	
	ngpio = atoi(argv[1]);
	
	fprintf(stderr,"##########gpio base test normal##########\n");
	fprintf(stderr,"##########gpio base test exist##########\n");
	ret = gpio_base_is_exist(ngpio);
	if (1 == ret) {
		fprintf(stderr,"gpio %d exist!\n",ngpio);
	} else {
		fprintf(stderr,"gpio %d not exist!\n",ngpio);
	}
	
	fprintf(stderr,"##########gpio base test export##########\n");
	ret = gpio_base_export(ngpio);
	if (GPIO_BASE_RET_OK == ret) {
		//system("ls /sys/class/gpio/*");
	}
	
	
	
	fprintf(stderr,"##########gpio base test direction##########\n");
	gpio_base_set_direction(ngpio, GPIO_DIR_INPUT);
	fprintf(stderr,"dirction :%d \n",gpio_base_get_direction(ngpio));
	gpio_base_set_direction(ngpio, GPIO_DIR_OUTPUT);
	fprintf(stderr,"dirction :%d \n",gpio_base_get_direction(ngpio));
	
	fprintf(stderr,"##########gpio base test edge##########\n");
	gpio_ctrl_set_edge(ngpio,GPIO_EDGE_NONE);
	fprintf(stderr,"edge :%d \n",gpio_base_get_edge(ngpio));
	gpio_ctrl_set_edge(ngpio,GPIO_EDGE_RISING);
	fprintf(stderr,"edge :%d \n",gpio_base_get_edge(ngpio));
	gpio_ctrl_set_edge(ngpio,GPIO_EDGE_FALLING);
	fprintf(stderr,"edge :%d \n",gpio_base_get_edge(ngpio));
	gpio_ctrl_set_edge(ngpio,GPIO_EDGE_BOTH);
	fprintf(stderr,"edge :%d \n",gpio_base_get_edge(ngpio));
	
	fprintf(stderr,"##########gpio base test value##########\n");
	gpio_ctrl_set_value(ngpio,1);
	fprintf(stderr,"val :%d \n",gpio_base_get_value(ngpio));
	gpio_ctrl_set_value(ngpio,0);
	fprintf(stderr,"val :%d \n",gpio_base_get_value(ngpio));
	
	fprintf(stderr,"##########gpio base test unexport##########\n");
	ret = gpio_base_unexport(ngpio);
	if (0 == ret) {
		system("ls /sys/class/gpio/*");
	}
	
	return 0;
}