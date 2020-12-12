#ifndef GPIO_IRQ_H_
#define GPIO_IRQ_H_

typedef int (*gpio_irq_func)(void *arg,int val);

typedef struct {
	int fd;
	int ngpio;
	void *irq_param;
	gpio_irq_func func;
} gpio_irq_t;


#endif