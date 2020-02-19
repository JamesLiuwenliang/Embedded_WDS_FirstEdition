
#include "iic_controller.h"
#define AT24Cxx_ADDR 0x50 /*�豸��ַ*/
int at24cxx_write(unsigned int addr ,unsigned char *data,int len)
{
	i2c_msg msgs;
	int i;
	unsigned char data;
	int err;
	unsigned char buf[2];

	/*������΢����һ��C����֪ʶ,ָ��ṹ���ָ�뱻ʹ�õ�ʱ��ʹ�ýṹ������Ĳ���ʹ��->,����ֱ��ʹ�ýṹ���ʱ��ʹ��.*/
	for(i=0;i<len;i++)
	{
		buf[0]=addr++;
		buf[1]=data[i]
	
		/*����i2c_msg*/
		msgs.addr = AT24Cxx_ADDR;
		msgs.flags = 0; /*д*/
		msgs.len =2;
		msgs.buf = buf;
		msgs.err = 0;
		msgs.cnt_transferred = -1;

		err = i2c_transfer( &msgs,1);/*��������ŵ��ǽṹ�������ָ��,��Ҫ���������ṹ��,���˷�ʱ��*/
		if(err)
			{return err;}
	}
	

	return 0;

	/*����i2c_transfer*/
	



}


/*�˴�Ҫ�ο�AT24cxx��оƬ�ֲ�*/
int at24cxx_read(unsigned int addr ,unsigned char *data,int len)
{
	i2c_msg msgs_for_addr;
	i2c_msg msgs_for_data;
	int err;
	
	/*����i2c_msg*/
	msgs_for_addr.addr = AT24Cxx_ADDR;
	msgs_for_addr.flags = 0; /*д*/
	msgs_for_addr.len =1;
	msgs_for_addr.buf = &addr;
	msgs_for_addr.err = 0;
	msgs_for_addr.cnt_transferred = -1;
	
	msgs_for_data.addr  = AT24Cxx_ADDR;
	msgs_for_data.flags = 1; /*д*/
	msgs_for_data.len   = len;
	msgs_for_data.buf   = data;
	msgs_for_data.err   = 0;
	msgs_for_data.cnt_transferred = -1;


	/*����i2c_transfer*/
	err = i2c_transfer( &msgs_for_addr,1);/*��������ŵ��ǽṹ�������ָ��,��Ҫ���������ṹ��,���˷�ʱ��*/
	if(err)
		{return err;}

    err = i2c_transfer( &msgs_for_data,1);/*��������ŵ��ǽṹ�������ָ��,��Ҫ���������ṹ��,���˷�ʱ��*/
    if(err)
    	{return err;}

	return 0;


}




