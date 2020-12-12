#ifndef GPIO_BASE_H_
#define GPIO_BASE_H_

#define GPIO_BASE_RET_OK (1)	
#define GPIO_BASE_RET_FAIL (-1)

typedef enum {
	GPIO_DIR_INPUT,
	GPIO_DIR_OUTPUT,
	GPIO_DIR_BUTT,
} gpio_direction_e;

typedef enum {
	GPIO_EDGE_NONE,
	GPIO_EDGE_RISING,
	GPIO_EDGE_FALLING,
	GPIO_EDGE_BOTH,
	GPIO_EDGE_BUTT,
} gpio_edge_e;

/*
*** return:
*** 0: return -1:failed
*/
int gpio_base_export(int ngpio);
int gpio_base_unexport(int ngpio);
/*
*** return:
*** 0: not exist 1:exist
*/
int gpio_base_is_exist(int ngpio);

int gpio_base_set_direction(int ngpio, gpio_direction_e dir);
gpio_direction_e gpio_base_get_direction(int ngpio);

int gpio_base_set_edge(int ngpio, gpio_edge_e edge);
gpio_edge_e gpio_base_get_edge(int ngpio);

int gpio_base_set_value(int ngpio, int value);
int gpio_base_get_value(int ngpio);

int gpio_base_open(int ngpio);
int gpio_base_close(int fd);

#endif