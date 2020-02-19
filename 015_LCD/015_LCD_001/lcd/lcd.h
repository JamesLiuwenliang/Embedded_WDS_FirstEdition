
å#ifndef _LCD_H
#define _LCD_H

enum {
	NORMAL = 0;
	INVERT = 1;
};

typedef struct time_sequence{
    /*´¹Ö±·½Ïò*/ 
	int tvp; //vysncÂö³å¿í¶È
	int tvb; //ÉÏ±ßºÚ¿ò
	int tvf; //ÏÂ±ßºÚ¿ò
	
	

	/*Ë®Æ½·½Ïò*/
	int thb;//×ó±ßºÚ¿ò
	int thp;//hsyncÂö³å¿í¶È
	int thf; //ÓÒ±ßºÚ¿ò

	int clk;

}time_sequence,*p_time_sequence;

/*	0 : Õı³£¼«ĞÔ
 *  1 : ·´×ª¼«ĞÔ
 */
typedef struct pins_polarity{
	int de;		   /*normal :¸ßµçÆ½Ê¹ÄÜÊı¾İ(²Î¿¼Figure15-6 )*/
	int peren;	   /*normal :¸ßµçÆ½ÓĞĞ§ */
	int vclk;     /*normal : ÔÚÏÂ½µÑØ»ñÈ¡Êı¾İ*/ 
	int rgb;      /*normal : ¸ßµçÆ½±íÊ¾1*/ 
	int hsync;    /*normal : ¸ßÂö³å*/ 
	int vsync;    /*normal : ¸ßÂö³å*/ 
	
}pins_polarity,*p_pins_polarity;


typedef struct lcd_params{

	char *name;
	
	/*Òı½Å¼«ĞÔ*/
	pins_polarity pins_pol;
	/*Ê±Ğò*/
	time_sequence time_seq;
	

	/*·Ö±æÂÊ*/
	int xres;
	int yres;
	int bpp;
	

	/*framebufferµÄµØÖ·*/
	unsigned int fb_base;
	

}lcd_params,*p_lcd_params;



#endif


