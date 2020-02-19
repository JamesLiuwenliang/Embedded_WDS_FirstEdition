/*最接近硬件的一层,直接操作硬件,
 *和它处于同一层的是plcdparams 这个结构体 ,
 *这个结构体也是操作硬件,但是是操作LCD
 */
#include "lcd_controller.h"
#include "../s3c2440_soc.h"
#include "lcd.h"
#define HCLK 100

//配置LCD所用到的引脚
void jz2440_lcd_pin_init(void)
{
	/*初始化引脚: 背光引脚 */
	GPBCON &= ~0x3;
	GPBCON |= 0x01;

	/*LCD专用引脚*/
	GPCCON = 0xaaaaaaaa;
	GPDCON = 0xaaaaaaaa;

	/*PWREN*/
	GPGCON |= (3<<8);
	

	

}

/*根据传入的LCD参数设置LCD控制器*/
void s3c2440_lcd_controller_init(p_lcd_params plcdparams)
{
	int pixelplace ;
	unsigned int addr;

	jz2440_lcd_pin_init();
	
	/*[17:8]: clkval,vclk = HCLK/[(CLKVAL+1)*2]
	 * 					9 = 100M/[(CLKVAL+1)*2],CLKVAL = 4.5 = 5
	 *				 vclk = 100/[(5+1)*2] = 8.3M
	 *			   CLKVAL = 100/vclk/2-1 ;
	 *[6:5] : TFT
	 *[4:1] : bppmode
	 *[0]   : LCD video output and the logic enable /disable
	 */
	/*需要重新找除法:
	 * a.去uboot中查找
	 * b.去内核源码查找
	 * c.去库函数查找(一般编辑器自带很多库函数)
	     进入工具链的目录:grep "__floatsisf" * -nR  找到.a文件*/ 
	int clkval = (double)HCLK /plcdparams->time_seq.vclk/2-1+0.5; /*除法需要重定义,调用gcc里面的函数*/

	int bppmode = plcdparams->bpp == 8 ? 0xb :\
		           plcdparams->bpp == 16 ? 0xc :\
		           0xd;
	LCDCON1 = (clkval<<8) | (1<<5) | (1<<6) | (bppmode<<1);


	/*	[31:24] : VBPD = tvb - 1
	 *  [23:14] : LINEVAL = line - 1
	 *  [13:6]  : VFPD = tvf - 1
	 *  [5:0]   : VSPW = tvp - 1
	 */
	LCDCON2 = ((plcdparams->time_seq.tvb-1)<<24) | \
			  ((plcdparams->yres -1)<<14)        | \
	          ((plcdparams->time_seq.tvf-1)<<6)  | \
	          ((plcdparams->time_seq.tvp)-1<<0)  ; 

	/* [25:19] : HBPD = thb - 1
	 * [18:8]  : HOZVAL = line - 1
	 * [7:0]   : HFPD = thf
	 *
	 */	
	LCDCON3  = ((plcdparams->time_seq.thb - 1)<<19) | \
			   ((plcdparams->xres-1)<<8)            | \
	           ((plcdparams->time_seq.thf-1)<<0);

     /*
      * [7:0]   :  HSPW  = thp -1
      *
      */ 
     LCDCON4  = ((plcdparams->time_seq.thp - 1)<<0);
	 
     /* 用来设置引脚极性,设置16bpp
      * [12]   :  BPP24BL 设置的参数是pixelplace,用来设置像素的摆放方法
      * [11]   :  FRM565 
      * 反转的寄存器
      * []
      */ 
     pixelplace = plcdparams->bpp == 32 ? (0) : \
	              plcdparams->bpp == 16 ? (1) : \
	             (1<<1);  /* 8bpp */
	 if (plcdparams->bpp==32)
	 	pixelplace = 0;
	 
#if 0
	 pixelplace = plcdparams->bpp == 32 ? (0) :\       /*bpp=24 全部在低字节*/
     			  plcdparams->bpp == 16 ? (1) :\       /*bpp=16 第一个像素在低字节,第二个像素在高字节*/
     			  (1<<1); /* 8bpp */
#endif
     LCDCON5  = (plcdparams->pins_pol.vclk << 10 ) |\
    			(plcdparams->pins_pol.hsync<< 9)   |\
    			(plcdparams->pins_pol.vsync<< 8)   |\
     			(plcdparams->pins_pol.rgb  << 7)   |\
				(plcdparams->pins_pol.de   << 6)   |\
				(plcdparams->pins_pol.peren<< 5)   |\
				(1<< 11) | pixelplace; //11bits表示565

	/*Frame Buffer
	 * [29:21] : LCDBANK,A[30:22] of fb
	 * [20:0]  : LCDBASEU,A[21:1] of fb
	 */
	addr = plcdparams->fb_base & ~(1<<31); //去掉最高位
	
	
	LCDSADDR1 = (addr>>1);//因为是从bit1开始的


	/*	Frame Buffer
	 *	[20:0] : LCDBASEL ,A[21:1] of end addr
	 *
	 */
	addr = plcdparams->fb_base + plcdparams->xres * plcdparams->yres * plcdparams->bpp / 8;
	addr >>= 1;
	addr &= 0x1fffff;
	LCDSADDR2 = addr;
	
	
}

void s3c2440_lcd_controller_enable(void)
{
	/*背光引脚: GPB0*/
	GPBDAT |= (1<<0);
	
	/*pwren  : 给LCD提供AVDD */
	LCDCON5 |= (1<<3); //第3位输出高电平
	
	/*LCDCON1'BIT0 : 设置LCD控制器是否输出信号*/
	LCDCON1 |= (1<<0);
	
}
void s3c2440_lcd_controller_disable()
{
	/*背光引脚: GPB0*/
	GPBDAT |= (1<<0);
	
	/*pwren  : 给LCD提供AVDD */
	LCDCON5 |= (1<<3); //第3位输出高电平

	/*LCDCON1'BIT0 : 设置LCD控制器是否输出信号*/
	LCDCON1 &= ~(1<<0);


}

/*使用调色板的目的是为了防止bpp=8时,用的引脚较少,但是颜色也会向某一颜色偏,具体要看内存的分配情况*/
/*设置调色板之前,先关闭lcd_controller*/
void s3c2440_lcd_controller_init_palette() //调色板
{
	volatile unsigned int *palette_base = 0X4D000400 ;//调色板肯定对应一块内存
	int i;
	int bit = LCDCON1 &(1<<0);
	/*LCDCON1'BIT 0 :设置lcd控制器是否输出信号 */
	if(bit)
	{
		LCDCON1 &= ~(1<<0);
	}
		
	for(i=0;i<256;i++)
	{
		/*低16位:RGB565 */
		*palette_base++ = i;
	}

	if(bit)
	{
		LCDCON1 |= (1<<0);
	}


	



}
struct lcd_controller s3c2440_lcd_controller={
	.name = "s3c2440",
	.init = s3c2440_lcd_controller_init,
	.enable = s3c2440_lcd_controller_enable,
	.disable =s3c2440_lcd_controller_disable,
	.init_palette = s3c2440_lcd_controller_init_palette, /*调色板*/
};

void s3c2440_lcd_controller_add(void)
{
	register_lcd_controller(&s3c2440_lcd_controller);/*通过这个函数告诉上层的管理者*/

}



