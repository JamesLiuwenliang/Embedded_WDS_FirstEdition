
�#ifndef _LCD_H
#define _LCD_H

enum {
	NORMAL = 0;
	INVERT = 1;
};

typedef struct time_sequence{
    /*��ֱ����*/ 
	int tvp; //vysnc������
	int tvb; //�ϱߺڿ�
	int tvf; //�±ߺڿ�
	
	

	/*ˮƽ����*/
	int thb;//��ߺڿ�
	int thp;//hsync������
	int thf; //�ұߺڿ�

	int clk;

}time_sequence,*p_time_sequence;

/*	0 : ��������
 *  1 : ��ת����
 */
typedef struct pins_polarity{
	int de;		   /*normal :�ߵ�ƽʹ������(�ο�Figure15-6 )*/
	int peren;	   /*normal :�ߵ�ƽ��Ч */
	int vclk;     /*normal : ���½��ػ�ȡ����*/ 
	int rgb;      /*normal : �ߵ�ƽ��ʾ1*/ 
	int hsync;    /*normal : ������*/ 
	int vsync;    /*normal : ������*/ 
	
}pins_polarity,*p_pins_polarity;


typedef struct lcd_params{

	char *name;
	
	/*���ż���*/
	pins_polarity pins_pol;
	/*ʱ��*/
	time_sequence time_seq;
	

	/*�ֱ���*/
	int xres;
	int yres;
	int bpp;
	

	/*framebuffer�ĵ�ַ*/
	unsigned int fb_base;
	

}lcd_params,*p_lcd_params;



#endif


