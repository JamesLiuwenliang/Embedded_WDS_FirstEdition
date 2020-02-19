/*��ӽ�Ӳ����һ��,ֱ�Ӳ���Ӳ��,
 *��������ͬһ�����plcdparams ����ṹ�� ,
 *����ṹ��Ҳ�ǲ���Ӳ��,�����ǲ���LCD
 */
#include "lcd_controller.h"
#include "../s3c2440_soc.h"
#include "lcd.h"
#define HCLK 100

//����LCD���õ�������
void jz2440_lcd_pin_init(void)
{
	/*��ʼ������: �������� */
	GPBCON &= ~0x3;
	GPBCON |= 0x01;

	/*LCDר������*/
	GPCCON = 0xaaaaaaaa;
	GPDCON = 0xaaaaaaaa;

	/*PWREN*/
	GPGCON |= (3<<8);
	

	

}

/*���ݴ����LCD��������LCD������*/
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
	/*��Ҫ�����ҳ���:
	 * a.ȥuboot�в���
	 * b.ȥ�ں�Դ�����
	 * c.ȥ�⺯������(һ��༭���Դ��ܶ�⺯��)
	     ���빤������Ŀ¼:grep "__floatsisf" * -nR  �ҵ�.a�ļ�*/ 
	int clkval = (double)HCLK /plcdparams->time_seq.vclk/2-1+0.5; /*������Ҫ�ض���,����gcc����ĺ���*/

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
	 
     /* �����������ż���,����16bpp
      * [12]   :  BPP24BL ���õĲ�����pixelplace,�����������صİڷŷ���
      * [11]   :  FRM565 
      * ��ת�ļĴ���
      * []
      */ 
     pixelplace = plcdparams->bpp == 32 ? (0) : \
	              plcdparams->bpp == 16 ? (1) : \
	             (1<<1);  /* 8bpp */
	 if (plcdparams->bpp==32)
	 	pixelplace = 0;
	 
#if 0
	 pixelplace = plcdparams->bpp == 32 ? (0) :\       /*bpp=24 ȫ���ڵ��ֽ�*/
     			  plcdparams->bpp == 16 ? (1) :\       /*bpp=16 ��һ�������ڵ��ֽ�,�ڶ��������ڸ��ֽ�*/
     			  (1<<1); /* 8bpp */
#endif
     LCDCON5  = (plcdparams->pins_pol.vclk << 10 ) |\
    			(plcdparams->pins_pol.hsync<< 9)   |\
    			(plcdparams->pins_pol.vsync<< 8)   |\
     			(plcdparams->pins_pol.rgb  << 7)   |\
				(plcdparams->pins_pol.de   << 6)   |\
				(plcdparams->pins_pol.peren<< 5)   |\
				(1<< 11) | pixelplace; //11bits��ʾ565

	/*Frame Buffer
	 * [29:21] : LCDBANK,A[30:22] of fb
	 * [20:0]  : LCDBASEU,A[21:1] of fb
	 */
	addr = plcdparams->fb_base & ~(1<<31); //ȥ�����λ
	
	
	LCDSADDR1 = (addr>>1);//��Ϊ�Ǵ�bit1��ʼ��


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
	/*��������: GPB0*/
	GPBDAT |= (1<<0);
	
	/*pwren  : ��LCD�ṩAVDD */
	LCDCON5 |= (1<<3); //��3λ����ߵ�ƽ
	
	/*LCDCON1'BIT0 : ����LCD�������Ƿ�����ź�*/
	LCDCON1 |= (1<<0);
	
}
void s3c2440_lcd_controller_disable()
{
	/*��������: GPB0*/
	GPBDAT |= (1<<0);
	
	/*pwren  : ��LCD�ṩAVDD */
	LCDCON5 |= (1<<3); //��3λ����ߵ�ƽ

	/*LCDCON1'BIT0 : ����LCD�������Ƿ�����ź�*/
	LCDCON1 &= ~(1<<0);


}

/*ʹ�õ�ɫ���Ŀ����Ϊ�˷�ֹbpp=8ʱ,�õ����Ž���,������ɫҲ����ĳһ��ɫƫ,����Ҫ���ڴ�ķ������*/
/*���õ�ɫ��֮ǰ,�ȹر�lcd_controller*/
void s3c2440_lcd_controller_init_palette() //��ɫ��
{
	volatile unsigned int *palette_base = 0X4D000400 ;//��ɫ��϶���Ӧһ���ڴ�
	int i;
	int bit = LCDCON1 &(1<<0);
	/*LCDCON1'BIT 0 :����lcd�������Ƿ�����ź� */
	if(bit)
	{
		LCDCON1 &= ~(1<<0);
	}
		
	for(i=0;i<256;i++)
	{
		/*��16λ:RGB565 */
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
	.init_palette = s3c2440_lcd_controller_init_palette, /*��ɫ��*/
};

void s3c2440_lcd_controller_add(void)
{
	register_lcd_controller(&s3c2440_lcd_controller);/*ͨ��������������ϲ�Ĺ�����*/

}



