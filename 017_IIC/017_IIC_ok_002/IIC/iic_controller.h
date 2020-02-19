#ifndef _IIC_CONTROLLER_H
#define _IIC_CONTROLLER_H


/*iic控制器的信息*/
typedef struct i2c_msg
{
	unsigned int addr;
	int flags;	/*0-write , 1-read*/
	int err;
	int len;
	int cnt_transferred; /*已经传输的字节数*/
	unsigned char *buf;
}i2c_msg,*p_i2c_msg;


/*此结构体包含IIC控制器的全部操作,里面全部是函数指针*/
typedef struct i2c_controller
{
	int (*init)void; /*函数指针,不需要任何参数*/
	int (*master_xfer)(p_i2c_msg msgs,int num);/*这里传输的最好是指针*/
	char *name; 
}i2c_controller,*p_i2c_controller;







#endif
