
#include "lcd.h"
#include "font.h"

/*根据字母的点阵在LCD上描画文字*/

/*获得LCD参数*/
extern const unsigned char fontdata_8x16[];
static unsigned int fb_base;
static int xres , yres, bpp ;

/*fontdata_8x16 意味着一个字符占8*16个像素;
 * 如果要打印字符串的话,就需要移动8位来打印下一个字符
*/
void font_init(void)
{
	get_lcd_params(&fb_base,&xres,&yres,&bpp);

}

void fb_print_char(int x,int y,char c,unsigned int colour)
{	
	int i,j;
	/*根据字符的ASCII码在fontdata_8x16中得到点阵数据*/
	unsigned char *dots = &fontdata_8x16[c*16];

	unsigned char data ; 
	int bit;

	/*根据点阵来设计对应像素的颜色*/
	for(j=y;j<y+16;j++)
	{
		data = *dots++;
		bit = 7;
		for(i=x;i<x+8;i++)
		{
			/*根据点阵的某位决定是否描颜色*/
			if(data & (1<<bit))
			{
				fb_put_pixel(i,j,colour);
			}
			bit--;/*像素处理完之后,要移动到下一位*/	
		}

	}

}

void fb_print_string (int x,int y,char *str ,unsigned int colour)
{
	int i=0,j;

	while(str[i])
	{
		if(str[i]== '\n')
		{
			y += 16;
		}
		else if(str[i]=='\r')
		{
			x = 0;
		}
		else
		{
    		fb_print_char(x,y,str[i],colour);
    		x+=8;
    		if(x>xres) /*换行*/
    		{
    			y += 16;
    			x=0;
    		}
		}
		i++;

	}

}



