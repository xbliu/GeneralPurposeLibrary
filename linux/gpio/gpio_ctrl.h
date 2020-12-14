#ifndef _GPIO_CTRL_H_
#define _GPIO_CTRL_H_

#include "gpio_base.h"

int gpio_ctrl_set_value(int ngpio, int value);
int gpio_ctrl_set_edge(int ngpio, gpio_edge_e edge);

#endif