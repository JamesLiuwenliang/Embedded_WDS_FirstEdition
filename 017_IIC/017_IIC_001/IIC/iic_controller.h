#ifndef _IIC_CONTROLLER_H
#define _IIC_CONTROLLER_H


/*iic����������Ϣ*/
typedef struct i2c_msg
{
	unsigned int addr;
	int flags;	/*0-write , 1-read*/
	int err;
	int len;
	int cnt_transferred; /*�Ѿ�������ֽ���*/
	unsigned char *buf;
}i2c_msg,*p_i2c_msg;


/*�˽ṹ�����IIC��������ȫ������,����ȫ���Ǻ���ָ��*/
typedef struct i2c_controller
{
	int (*init)void; /*����ָ��,����Ҫ�κβ���*/
	int (*master_xfer)(p_i2c_msg msgs,int num);/*���ﴫ��������ָ��*/
	char *name; 
}i2c_controller,*p_i2c_controller;







#endif
