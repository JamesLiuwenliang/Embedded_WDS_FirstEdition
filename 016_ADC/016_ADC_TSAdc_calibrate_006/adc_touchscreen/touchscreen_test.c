
#include "touchscreen.h"
#include "../lcd/lcd.h"
#include "../lcd/framebuff.h"

/*
 *������У׼��Ҫ��;
 * 	1. ���ڴ�����Ҫ��β���ȡƽ��ֵ
 *  2. Ҫ�����Ƿ�ֵ
 */
void touchscreen_test()
{
	unsigned int fb_base;
	int xres,yres,bpp;
	int x,y,pressure;
	

	/*���LCD����*/
	get_lcd_params(&fb_base, &xres, &yres, &bpp);

	
	touchscreen_init();

	/*����*/
	clear_screen(0);

	/*��ʾ����У׼*/
	fb_print_string(70,70,"Touch cross to calibrate",0xfffffff);
	ts_calibrate();

	/*��ʾУ׼����*/
	fb_print_string(70 ,yres-70,"draw",0xfffffff);


	while(1)
	{
		if( ts_read(&x,&y,&pressure)==0)
		{
			printf("x = %d , y = %d\r\n",x,y);
			
			if(pressure) /*���µ�״̬�Ż����*/
			{
				fb_put_pixel( x, y,0xff00);
			}

		}

	}

}






