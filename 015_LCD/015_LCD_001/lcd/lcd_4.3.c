/*��Щ���е���Ŀ,��Ҫ����LCDоƬ�ֲ�������;
 *��һ�����õ�c�ļ�lcd-controller.c�Ǹ���LCD������(S3C2440)�����õ�
 */
/*��Ҫָ��һ��û��ʹ�õ��ڴ�*/
#define LCD_FRAME_BUFFER 0x33c00000

lcd_params lcd_4_3_params = {

	.name = "lcd_4.3"
	.pins_polarity = {
    	.de    = NORMAL ;	   /*normal :�ߵ�ƽʹ������(�ο�Figure15-6 )*/
    	.peren = NORMAL;	   /*normal :�ߵ�ƽ��Ч */
    	.vclk  = NORMAL;       /*normal : ���½��ػ�ȡ����*/ 
    	.rgb   = NORMAL;       /*normal : �ߵ�ƽ��ʾ1*/ 
    	.hsync = INVERT;       /*normal : ������*/ 
    	.vsync = INVERT        /*normal : ������*/ 

	}
	.time_sequence = {
    	/*��ֱ����*/ 
    	.tvp = 10; //vysnc������
    	.tvb = 2; //�ϱߺڿ�
    	.tvf = 2; //�±ߺڿ�
    	
    	
    
    	/*ˮƽ����*/
    	.thb = 41;//��ߺڿ�
    	.thp = 2 ;//hsync������
    	.thf = 2 ; //�ұߺڿ�
    
    	.clk = 9;/*MHz*/

	}
	.xres = 480;
	.yres = 272;
	.bpp  = 16;
	.fb_base = LCD_FRAME_BUFFER;
	

};


void lcd_4_3_add(void)
{
	register_lcd_params(&lcd_4_3_params);/*ͨ��������������ϲ�Ĺ�����*/
}


