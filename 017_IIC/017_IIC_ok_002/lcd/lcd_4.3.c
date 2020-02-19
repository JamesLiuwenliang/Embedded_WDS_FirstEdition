/*这些所有的项目,需要根据LCD芯片手册来设置;
 *上一个设置的c文件lcd-controller.c是根据LCD控制器(S3C2440)来设置的
 */
/*需要指向一块没有使用的内存*/
#include "lcd.h"
#define LCD_FRAME_BUFFER 0x33c00000

lcd_params lcd_4_3_params = {

	.name = "lcd_4.3",
	.pins_pol = {
    	.de    = NORMAL,	   /*normal :高电平使能数据(参考Figure15-6 )*/
    	.peren = NORMAL,	   /*normal :高电平有效 */
    	.vclk  = NORMAL,       /*normal : 在下降沿获取数据*/ 
    	.rgb   = NORMAL,       /*normal : 高电平表示1*/ 
    	.hsync = INVERT,       /*normal : 低脉冲*/ 
    	.vsync = INVERT,       /*normal : 低脉冲*/ 

	},
	.time_seq = {
    	/*垂直方向*/ 
    	.tvp = 10, //vysnc脉冲宽度
    	.tvb = 2, //上边黑框
    	.tvf = 2, //下边黑框
    	
    	
    
    	/*水平方向*/
    	.thb = 41,//左边黑框
    	.thp = 2 ,//hsync脉冲宽度
    	.thf = 2 , //右边黑框
    
    	.vclk = 9, /*MHz*/

	},
	.xres = 480,
	.yres = 272,
	.bpp  = 32,  /*bpp要么取16,要么取32,不能取24*/
	.fb_base = LCD_FRAME_BUFFER,
	

};


void lcd_4_3_add(void)
{
	register_lcd_params(&lcd_4_3_params);/*通过这个函数告诉上层的管理者*/
}


