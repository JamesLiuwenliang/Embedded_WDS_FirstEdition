
#include "touchscreen.h"
#include "../lcd/lcd.h"
#include "../lcd/framebuff.h"

/*
 *触摸屏校准的要点;
 * 	1. 对于触摸屏要多次测量取平均值
 *  2. 要丢弃非法值
 */
void touchscreen_test()
{
	unsigned int fb_base;
	int xres,yres,bpp;
	int x,y,pressure;
	

	/*获得LCD参数*/
	get_lcd_params(&fb_base, &xres, &yres, &bpp);

	
	touchscreen_init();

	/*清屏*/
	clear_screen(0);

	/*显示文字校准*/
	fb_print_string(70,70,"Touch cross to calibrate",0xfffffff);
	ts_calibrate();

	/*显示校准文字*/
	fb_print_string(70 ,yres-70,"draw",0xfffffff);


	while(1)
	{
		if( ts_read(&x,&y,&pressure)==0)
		{
			printf("x = %d , y = %d\r\n",x,y);
			
			if(pressure) /*按下的状态才会描点*/
			{
				fb_put_pixel( x, y,0xff00);
			}

		}

	}

}






