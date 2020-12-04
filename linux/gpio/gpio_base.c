#include "gpio_base.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define GPIO_BASE_DIR "/sys/class/gpio"

int gpio_base_export(int ngpio)
{
	int fd = 0;
	int len = 0;
	char str[64] = {0};
	
	snprintf(str,sizeof(str),"%s/%s",GPIO_BASE_DIR,"export");
	fd = open(str,O_WRONLY);
    if(fd < 0) {
		printf("%s gpio %d failed\n",str,ngpio);
        goto err_out;
    }
	
	memset(str,0,sizeof(str));
    len = snprintf(str,sizeof(str),"%d",ngpio);  
    write(fd,str,len);
	
	close(fd);
	
	return GPIO_BASE_RET_OK;
err_out:	
	return GPIO_BASE_RET_FAIL;
}

int gpio_base_unexport(int ngpio)
{
	int fd = 0;
	int len = 0;
	char str[64] = {0};
	
	snprintf(str,sizeof(str),"%s/%s",GPIO_BASE_DIR,"unexport");
	fd = open(str,O_WRONLY);
    if(fd < 0) {
		printf("path %s gpio %d failed\n",str,ngpio);
        goto err_out;
    }
	
	memset(str,0,sizeof(str));
    len = snprintf(str, sizeof(str),"%d",ngpio);  
    write(fd,str,len);
	
	close(fd);
	
	return GPIO_BASE_RET_OK;
err_out:	
	return GPIO_BASE_RET_FAIL;
}

int gpio_base_set_direction(int ngpio, gpio_direction_e dir)
{
	int fd = 0;
	int len = 0;
	char str[64] = {0};
	
	if (dir >= GPIO_DIR_BUTT) {
		printf("illegal parameter!\n");
		goto err_out;
	}
	
	snprintf(str,sizeof(str),"%s/gpio%d/%s",GPIO_BASE_DIR,ngpio,"direction");
	fd = open(str,O_WRONLY);
    if(fd < 0) {
		printf("path %s  set gpio %d failed\n", str,ngpio);
        goto err_out;
    }
	
	memset(str,0,sizeof(str));
	switch(dir) {
		case GPIO_DIR_INPUT:
			len = snprintf(str,sizeof(str),"%s","in");
			break;
		case GPIO_DIR_OUTPUT:
			len = snprintf(str,sizeof(str),"%s","out");
			break;
		default:
			break;
	}
      
    write(fd,str,len);
	close(fd);
	
	return GPIO_BASE_RET_OK;
err_out:	
	return GPIO_BASE_RET_FAIL;
}

gpio_direction_e gpio_base_get_direction(int ngpio)
{
	int fd = 0;
	int len = 0;
	char str[64] = {0};
	gpio_direction_e dir = GPIO_DIR_BUTT;
	
	snprintf(str,sizeof(str),"%s/gpio%d/%s",GPIO_BASE_DIR,ngpio,"direction");
	fd = open(str,O_WRONLY);
    if(fd < 0) {
		printf("path %s  set gpio %d failed\n", str,ngpio);
        goto err_out;
    }
	
	memset(str,0,sizeof(str));
	read(fd,str,sizeof(str));
	if (0 == strcmp(str,"in")) {
		dir = GPIO_DIR_INPUT;
	} else if (0 == strcmp(str,"out")) {
		dir = GPIO_DIR_OUTPUT;
	}
	
	close(fd);
	
err_out:	
	return dir;
}

int gpio_base_set_edge(int ngpio, gpio_edge_e edge)
{
	int fd = 0;
	int len = 0;
	char str[64] = {0};
	
	if (edge >= GPIO_EDGE_BUTT) {
		printf("illegal parameter!\n");
		goto err_out;
	}
	
	snprintf(str,sizeof(str),"%s/gpio%d/%s",GPIO_BASE_DIR,ngpio,"edge");
	fd = open(str,O_WRONLY);
    if(fd < 0) {
		printf("path %s  set gpio %d failed\n", str,ngpio);
        goto err_out;
    }
	
	memset(str,0,sizeof(str));
	switch(edge) {
		case GPIO_EDGE_NONE:
			len = snprintf(str,sizeof(str),"%s","none");
			break;
		case GPIO_EDGE_RISING:
			len = snprintf(str,sizeof(str),"%s","rising");
			break;
		case GPIO_EDGE_FALLING:
			len = snprintf(str,sizeof(str),"%s","falling");
			break;
		case GPIO_EDGE_BOTH:
			len = snprintf(str,sizeof(str),"%s","both");
			break;
		default:
			break;
	}
    
    write(fd,str,len);
	close(fd);
	
	return GPIO_BASE_RET_OK;
err_out:	
	return GPIO_BASE_RET_FAIL;
}

gpio_edge_e gpio_base_get_edge(int ngpio)
{
	int fd = 0;
	gpio_edge_e edge = GPIO_EDGE_BUTT;
	char str[64] = {0};
	
	snprintf(str,sizeof(str),"%s/gpio%d/%s",GPIO_BASE_DIR,ngpio,"edge");
	fd = open(str, O_WRONLY);
    if(fd < 0) {
		printf("path %s  set gpio %d failed\n", str,ngpio);
        goto err_out;
    }
	
	memset(str,0,sizeof(str));
	read(fd,str,sizeof(str));
	
	if (0 == strcmp(str,"none")) {
		edge = GPIO_EDGE_NONE;
	} else if (0 == strcmp(str,"rising")) {
		edge = GPIO_EDGE_RISING;
	} else if (0 == strcmp(str,"falling")) {
		edge = GPIO_EDGE_FALLING;
	} else if (0 == strcmp(str,"both")) {
		edge = GPIO_EDGE_BOTH;
	}

	close(fd);
	
	return edge;
err_out:	
	return GPIO_BASE_RET_FAIL;
}

int gpio_base_set_value(int ngpio, int value)
{
	int fd = 0;
	int len = 0;
	char str[64] = {0};
	
	snprintf(str,sizeof(str),"%s/gpio%d/%s",GPIO_BASE_DIR,ngpio,"value");
	fd = open(str,O_WRONLY);
    if(fd < 0) {
		printf("path %s  set gpio %d failed\n", str,ngpio);
        goto err_out;
    }
	
	memset(str,0,sizeof(str));
	len = snprintf(str,sizeof(str),"%d",value);
	write(fd,str,len);
	
	close(fd);
	
	return GPIO_BASE_RET_OK;
err_out:	
	return GPIO_BASE_RET_FAIL;
}

int gpio_base_get_value(int ngpio)
{
	int fd = 0;
	int value = 0;
	char str[64] = {0};
	
	snprintf(str,sizeof(str),"%s/gpio%d/%s",GPIO_BASE_DIR,ngpio,"value");
	fd = open(str,O_WRONLY);
    if(fd < 0) {
		printf("path %s  set gpio %d failed\n", str,ngpio);
        goto err_out;
    }
	
	memset(str,0,sizeof(str));
	read(fd, str,sizeof(str));
	sscanf(str,"%d",&value);
	
	close(fd);
	
	return value;
err_out:	
	return GPIO_BASE_RET_FAIL;
}

