#ifndef _I2C_OPS_H_
#define _I2C_OPS_H_


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


typedef struct {
	unsigned int i2c_num;
	unsigned int dev_addr;
	unsigned int reg_width;
	unsigned int data_width;
	void *priv;
} i2c_ctrl_t;


int i2c_init_ctrl(i2c_ctrl_t *ctrl, 	unsigned int i2c_num, unsigned int dev_addr, unsigned int reg_width, unsigned int data_width);
int i2c_change_dev_addr(i2c_ctrl_t *ctrl, unsigned int dev_addr);
int i2c_open(i2c_ctrl_t *ctrl);
int i2c_close(i2c_ctrl_t *ctrl);
int i2c_read(i2c_ctrl_t *ctrl, unsigned int reg_addr, unsigned int *data);
int i2c_write(i2c_ctrl_t *ctrl, unsigned int reg_addr, unsigned int data);


#ifdef __cplusplus
#if __cplusplus
	}
#endif
#endif /* __cplusplus */


#endif

