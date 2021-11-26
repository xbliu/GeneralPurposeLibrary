#include "i2c_ctrl.h"
#include "log_adapter.h"

#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>

#include <stdio.h>
#include <string.h>


int i2c_init_ctrl(i2c_ctrl_t *ctrl, unsigned int i2c_num, unsigned int dev_addr, unsigned int reg_width, unsigned int data_width)
{
	if (!ctrl) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal param!\n");
		return -1;
	}

    ctrl->i2c_num = i2c_num;
    ctrl->dev_addr = dev_addr >> 1;
    ctrl->reg_width = reg_width;
    ctrl->data_width = data_width;

    return 0;
}

int i2c_change_dev_addr(i2c_ctrl_t *ctrl, unsigned int dev_addr)
{
    int ret = 0;
    long fd = -1;

	if (!ctrl) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal param!\n");
		return -1;
	}

	fd = (long)ctrl->priv;
    ret = ioctl(fd, I2C_SLAVE_FORCE, dev_addr >> 1);
    if(ret < 0) {
        LOG_ERROR(LOG_MOD_UTILS, "set i2c device address error:%s!\n", strerror(errno));
        return -1;
    }
    ctrl->dev_addr = dev_addr >> 1;

    return ret;
}

int i2c_open(i2c_ctrl_t *ctrl)
{
    int ret = 0;
    long fd = 0;
    char file_name[16];

	if (!ctrl) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal param!\n");
		return -1;
	}
	
    sprintf(file_name, "/dev/i2c-%u", ctrl->i2c_num);
    fd = open(file_name, O_RDWR);
    if (fd < 0) {
        LOG_ERROR(LOG_MOD_UTILS, "open %s error!\n", file_name);
        ret = -1;
        goto end0;
    }

    ret = ioctl(fd, I2C_SLAVE_FORCE, ctrl->dev_addr);
    if(ret < 0) {
        LOG_ERROR(LOG_MOD_UTILS, "set i2c device address error:%s!\n", strerror(errno));
        ret = -1;
        goto end1;
    }

    ctrl->priv = (void *)fd;

    return ret;
end1:
    close(fd);
end0:
    return ret;
}

int i2c_close(i2c_ctrl_t *ctrl)
{
	long fd = -1;

	if (!ctrl) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal param!\n");
		return -1;
	}

    fd = (long)ctrl->priv;
    return close(fd);
}

int i2c_read(i2c_ctrl_t *ctrl, unsigned int reg_addr, unsigned int *data)
{
    int ret = 0;
    unsigned char buf[4] = {0};
    struct i2c_rdwr_ioctl_data rdwr;
    struct i2c_msg msg[2];
	long fd = -1;

	if (!ctrl) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal param!\n");
		return -1;
	}

    fd = (long)ctrl->priv;

    msg[0].addr = ctrl->dev_addr;
    msg[0].flags = 0;
    msg[0].len = ctrl->reg_width;
    msg[0].buf = buf;

    msg[1].addr = ctrl->dev_addr;
    msg[1].flags = 0;
    msg[1].flags |= I2C_M_RD;
    msg[1].len = ctrl->data_width;
    msg[1].buf = buf;

    rdwr.msgs = &msg[0];
    rdwr.nmsgs = (__u32)2;

    if (2 == ctrl->reg_width) {
        buf[0] = (reg_addr >> 8) & 0xff;
        buf[1] = reg_addr & 0xff;
    } else {
        buf[0] = reg_addr & 0xff;
    }

    ret = ioctl(fd, I2C_RDWR, &rdwr);
    if (ret != 2) {
        LOG_ERROR(LOG_MOD_UTILS, "cmd_i2c_read error %s!\n", strerror(errno));
        ret = -1;
        goto end1;
    }

    if (ctrl->data_width == 2) {
        *data = buf[1] | (buf[0] << 8);
    } else {
        *data = buf[0];
    }
    
    ret = 0;
end1:    
    return ret;
}

int i2c_write(i2c_ctrl_t *ctrl, unsigned int reg_addr, unsigned int data)
{
    int index = 0;
    int ret = 0;
    unsigned char buf[4] = {0};
	long fd = -1;

	if (!ctrl) {
		LOG_ERROR(LOG_MOD_UTILS, "illegal param!\n");
		return -1;
	}

    fd = (long)ctrl->priv;

    if (2 == ctrl->reg_width) {
        buf[index] = (reg_addr >> 8) & 0xff;
        index++;
        buf[index] = reg_addr & 0xff;
        index++;
    } else {
        buf[index] = reg_addr & 0xff;
        index++;
    }

    if (2 == ctrl->data_width) {
        buf[index] = (data >> 8) & 0xff;
        index++;
        buf[index] = data & 0xff;
        index++;
    } else {
        buf[index] = data & 0xff;
        index++;
    }

    ret = write(fd, buf, index);
    if(ret < 0) {
        LOG_ERROR(LOG_MOD_UTILS, "i2c write error:%s!\n", strerror(errno));
        ret = -1;
    } else {
        ret = 0;
    }

    return ret;
}

