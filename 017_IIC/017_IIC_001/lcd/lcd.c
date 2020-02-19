#include "lcd.h"
#include "lcd_controller.h"

#define lcd_num 10  /*֧�ֶ��ٿ�LCD*/
static p_lcd_params p_array_lcd[lcd_num];
static p_lcd_params g_p_lcd_params; /*��ѡ�е�lcd_params*/
int register_lcd_params(p_lcd_params lcdparams)//ע�ắ��
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
int select_lcd_params(char *name)//��ʼ���õĺ���
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

void get_lcd_params(unsigned int *fb_base,int *xres ,int *yres,int *bpp )
{
	*fb_base = g_p_lcd_params->fb_base;
	*xres = g_p_lcd_params->xres;
	*yres = g_p_lcd_params->yres;
	*bpp = g_p_lcd_params->bpp;
}


void lcd_enable()
{
	lcd_controller_enable();
}

void lcd_disable()
{
	lcd_controller_disable();
}

int lcd_init(void)
{

	/*ע��LCD*/
	lcd_4_3_add();
	
	/*ע��LCD������*/
	lcd_controller_add();

	/*ѡ��ĳ��LCD*/
	select_lcd_params("lcd_4.3");

	/*ѡ��ĳ��LCD������*/
	select_lcd_controller("s3c2440");

	/*ʹ��LCD�Ĳ���,��ʼ��LCD������*/
	lcd_controller_init(g_p_lcd_params);

}



