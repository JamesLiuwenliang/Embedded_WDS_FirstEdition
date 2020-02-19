#include "lcd_controller.h"
/*向上:接收不同LCD的参数
 *向下:使用这些参数设置对应的LCD控制器
 */
 #define lcd_controller_num 10
static p_lcd_controller p_array_lcd_controller [lcd_controller_num];
static p_lcd_controller g_p_lcd_coontroller_selected ; /*被选中的lcd_controller*/
int register_lcd_controller(p_lcd_controller plcdcon)
{
	int i;
	for(i=0;i<lcd_controller_num;i++)
	{
		if(!p_array_lcd_controller[i])
		{
			p_array_lcd_controller[i] = plcdcon;
			return i;
		}

	}
	return -1;


}
int select_lcd_controller(char *name)
{
	int i;
	for(i=0;i<lcd_controller_num;i++)
	{
		if(p_array_lcd_controller[i] && !strcmp(p_array_lcd_controller[i]->name,name))
		{
			g_p_lcd_coontroller_selected = p_array_lcd_controller[i];
		
			return i;
		}

	}
	return -1;
}

int lcd_controller_init(p_lcd_params plcdparams)
{
	/*调用所选择的的LCD控制器的初始化函数*/
	if(g_p_lcd_coontroller_selected)
	{
		g_p_lcd_coontroller_selected->init(plcdparams);
		return 0;
	}
	return -1;
}

void lcd_controller_add(void)
{
	s3c2440_lcd_controller_add();



}



