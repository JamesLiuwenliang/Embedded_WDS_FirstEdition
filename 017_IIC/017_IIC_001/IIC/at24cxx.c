
#include "iic_controller.h"
#define AT24Cxx_ADDR 0x50 /*设备地址*/
int at24cxx_write(unsigned int addr ,unsigned char *data,int len)
{
	i2c_msg msgs;
	int i;
	unsigned char data;
	int err;
	unsigned char buf[2];

	/*这里稍微回忆一下C语言知识,指向结构体的指针被使用的时候使用结构体里面的参数使用->,但是直接使用结构体的时候使用.*/
	for(i=0;i<len;i++)
	{
		buf[0]=addr++;
		buf[1]=data[i]
	
		/*构造i2c_msg*/
		msgs.addr = AT24Cxx_ADDR;
		msgs.flags = 0; /*写*/
		msgs.len =2;
		msgs.buf = buf;
		msgs.err = 0;
		msgs.cnt_transferred = -1;

		err = i2c_transfer( &msgs,1);/*这里如果放的是结构体而不是指针,就要传输整个结构体,很浪费时间*/
		if(err)
			{return err;}
	}
	

	return 0;

	/*调用i2c_transfer*/
	



}


/*此处要参考AT24cxx的芯片手册*/
int at24cxx_read(unsigned int addr ,unsigned char *data,int len)
{
	i2c_msg msgs_for_addr;
	i2c_msg msgs_for_data;
	int err;
	
	/*构造i2c_msg*/
	msgs_for_addr.addr = AT24Cxx_ADDR;
	msgs_for_addr.flags = 0; /*写*/
	msgs_for_addr.len =1;
	msgs_for_addr.buf = &addr;
	msgs_for_addr.err = 0;
	msgs_for_addr.cnt_transferred = -1;
	
	msgs_for_data.addr  = AT24Cxx_ADDR;
	msgs_for_data.flags = 1; /*写*/
	msgs_for_data.len   = len;
	msgs_for_data.buf   = data;
	msgs_for_data.err   = 0;
	msgs_for_data.cnt_transferred = -1;


	/*调用i2c_transfer*/
	err = i2c_transfer( &msgs_for_addr,1);/*这里如果放的是结构体而不是指针,就要传输整个结构体,很浪费时间*/
	if(err)
		{return err;}

    err = i2c_transfer( &msgs_for_data,1);/*这里如果放的是结构体而不是指针,就要传输整个结构体,很浪费时间*/
    if(err)
    	{return err;}

	return 0;


}




