
#include "iic_controller.h"
#include "my_printf.h"
#include "string_utils.h"
#define I2C_CONTROLLER_NUM 10   /*代表最多支持10中iic控制器*/




/*有一个iic_controller数组用来存放各种不同芯片的操作结构体*/
static p_i2c_controller p_i2c_controllers[I2C_CONTROLLER_NUM];
static p_i2c_controller p_i2c_con_selected ;

void register_i2c_controller(p_i2c_controller *p,)
{
	int i;
	for(i=0;i<I2C_CONTROLLER_NUM ; i++)
	{
		if(!p_i2c_controllers[i] )
		{
			p_i2c_controllers[i] = p;
			return ;
		}
	}

}
/*根据名字来选择某款IIC控制器*/
int select_i2c_controller(char *name)
{
	int i;
	for(i=0;i<I2C_CONTROLLER_NUM ; i++)
	{
		if( p_i2c_controllers[i] && !strcmp(p_i2c_controllers[i]->name,name))
		{
			p_i2c_con_selected = p_i2c_controllers[i];
			return 0;
		}
	}
	return -1;
}



/*实现i2c_transfer 接口函数*/
int i2c_transfer(p_i2c_msg msgs,int num)
{
	return p_i2c_con_selected->master_xfer(msgs,num);
}


void i2c_controller_init()
{
	/*注册下门面的IIC 控制器*/
	s3c2440_i2c_con_add();
	
	/*选择某款IIC控制器*/
	select_i2c_controller("s3c2440");

	/*调用该控制器初始化函数*/
	p_i2c_con_selected->init();


}




