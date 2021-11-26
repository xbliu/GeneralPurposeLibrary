#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "i2c_ctrl.h"


#define XCHIP_I2C_ADDR (0x36)
#define SC132_I2C_ADDR (0x60)


static void xc9080_i2c_bypass_on_chn(i2c_ctrl_t *i2c_ctrl, char chn)
{
	i2c_write(i2c_ctrl, 0xfffd,0x80);
	i2c_write(i2c_ctrl, 0xfffe,0x50);
	i2c_write(i2c_ctrl, 0x004d,chn);
}

static void xc9080_i2c_bypass_off(i2c_ctrl_t *i2c_ctrl)
{
    i2c_write(i2c_ctrl, 0xfffd,0x80);
    i2c_write(i2c_ctrl, 0xfffe,0x50);
    i2c_write(i2c_ctrl, 0x004d,0x00);
}

void sc132_xchip_read_method1(unsigned int reg_addr)
{
	unsigned int data = 0;

	i2c_ctrl_t xchip_ctrl;
	i2c_ctrl_t scl132_ctrl;
	i2c_init_ctrl(&xchip_ctrl, 2, XCHIP_I2C_ADDR, 2, 1);
	i2c_init_ctrl(&scl132_ctrl, 2, SC132_I2C_ADDR, 2, 1);

	i2c_open(&xchip_ctrl);
	i2c_open(&scl132_ctrl);

	//by pass on
	xc9080_i2c_bypass_on_chn(&xchip_ctrl,3);
	
	i2c_read(&scl132_ctrl, reg_addr, &data);
	printf("<%s:%d> reg_addr[0x%x] data[0x%x] \n",__FUNCTION__,__LINE__,reg_addr,data);

	xc9080_i2c_bypass_off(&xchip_ctrl);
	i2c_close(&xchip_ctrl);
	i2c_close(&scl132_ctrl);
}

void sc132_xchip_read_method2(unsigned int reg_addr)
{
	int ret = 0;
	unsigned int data = 0;
	i2c_ctrl_t i2c_ctrl;

	i2c_init_ctrl(&i2c_ctrl, 2, XCHIP_I2C_ADDR, 2, 1);
	ret = i2c_open(&i2c_ctrl);
	if (0 != ret) {
		printf("<%s:%d> open xchip i2c failed!\n",__FUNCTION__,__LINE__);
		return;
	}

#if 1
	//by pass on
	xc9080_i2c_bypass_on_chn(&i2c_ctrl,3);

	i2c_change_dev_addr(&i2c_ctrl,SC132_I2C_ADDR);
	i2c_read(&i2c_ctrl, reg_addr, &data);
	printf("<%s:%d> reg_addr[0x%x] data[0x%x] \n",__FUNCTION__,__LINE__,reg_addr,data);
	
	i2c_change_dev_addr(&i2c_ctrl,XCHIP_I2C_ADDR);
	xc9080_i2c_bypass_off(&i2c_ctrl);
	usleep(1000000);
#endif

	ret = i2c_close(&i2c_ctrl);
	if (0 != ret) {
		printf("<%s:%d> close xchip i2c failed ret[%d] %s !\n",__FUNCTION__,__LINE__,ret,strerror(errno));
		return;
	}
}


int main(int argc, char *argv[])
{
	unsigned int reg_addr = 0;

	if (argc > 1) {
		reg_addr = atoi(argv[1]);
	} else {
		reg_addr = 0x3e01;
	}

	sc132_xchip_read_method1(reg_addr);
	sc132_xchip_read_method2(reg_addr);

	return 0;
}


