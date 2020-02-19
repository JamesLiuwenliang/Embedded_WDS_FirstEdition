#include "Lcd.h"

#define lcd_num 10  /*支持多少款LCD*/
static p_lcd_params p_array_lcd[lcd_num];
static p_lcd_params g_p_lcd_params; /*被选中的lcd_params*/
int register_lcd_params(p_lcd_params lcdparams)//注册函数
{
	int i;
	for(i=0;i<lcd_num;i++)
	{
		if(!p_array_lcd[i])
		{
			p_array_lcd[i] = lcdparams;
			return i;
		}

	}
	return -1;
}
int select_lcd_params(char *name)//初始化用的函数
{
	int i;
	for(i=0;i<lcd_num;i++)
	{
		if(p_array_lcd[i] && !strcmp(p_array_lcd[i]->name,name))
		{
			g_p_lcd_params = p_array_lcd[i];
		
			return i;
		}

	}
	return -1;
}

int lcd_init(void)
{

	/*注册LCD*/
	lcd_4_3_add();
	
	/*注册LCD控制器*/
	lcd_controller_add();

	/*选择某款LCD*/
	select_lcd_params("lcd_4.3");

	/*选择某款LCD控制器*/
	select_lcd_controller("s3c2440");

	/*使用LCD的参数,初始化LCD控制器*/
	lcd_controller_init(g_p_lcd_coontroller_selected);

}




