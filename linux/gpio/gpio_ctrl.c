#include "gpio_ctrl.h"

int gpio_ctrl_set_value(int ngpio, int value)
{
	if (!gpio_base_is_exist(ngpio)) {
		gpio_base_export(ngpio);
	}
	
	if (GPIO_DIR_INPUT == gpio_base_get_direction(ngpio)) {
		if (GPIO_EDGE_NONE != gpio_base_get_edge(ngpio)) {
			gpio_base_set_edge(ngpio,GPIO_EDGE_NONE);
		}
		
		gpio_base_set_direction(ngpio,GPIO_DIR_OUTPUT);
	}
	
	gpio_base_set_value(ngpio,value);
	
	return 0;
}

int gpio_ctrl_set_edge(int ngpio, gpio_edge_e edge)
{
	if (!gpio_base_is_exist(ngpio)) {
		gpio_base_export(ngpio);
	}
	
	if (GPIO_DIR_OUTPUT == gpio_base_get_direction(ngpio)) {
		gpio_base_set_direction(ngpio,GPIO_DIR_INPUT);
	}
	
	gpio_base_set_edge(ngpio,edge);
	
	return 0;
}

