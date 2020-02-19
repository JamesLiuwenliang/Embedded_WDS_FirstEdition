#include "lcd_controller.h"
/*����:���ղ�ͬLCD�Ĳ���
 *����:ʹ����Щ�������ö�Ӧ��LCD������
 */
 #define lcd_controller_num 10
static p_lcd_controller p_array_lcd_controller [lcd_controller_num];
static p_lcd_controller g_p_lcd_coontroller_selected ; /*��ѡ�е�lcd_controller*/
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
	/*������ѡ��ĵ�LCD�������ĳ�ʼ������*/
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



