
#include "lcd.h"
#include "font.h"

/*������ĸ�ĵ�����LCD���軭����*/

/*���LCD����*/
extern const unsigned char fontdata_8x16[];
static unsigned int fb_base;
static int xres , yres, bpp ;

/*fontdata_8x16 ��ζ��һ���ַ�ռ8*16������;
 * ���Ҫ��ӡ�ַ����Ļ�,����Ҫ�ƶ�8λ����ӡ��һ���ַ�
*/
void font_init(void)
{
	get_lcd_params(&fb_base,&xres,&yres,&bpp);

}

void fb_print_char(int x,int y,char c,unsigned int colour)
{	
	int i,j;
	/*�����ַ���ASCII����fontdata_8x16�еõ���������*/
	unsigned char *dots = &fontdata_8x16[c*16];

	unsigned char data ; 
	int bit;

	/*���ݵ�������ƶ�Ӧ���ص���ɫ*/
	for(j=y;j<y+16;j++)
	{
		data = *dots++;
		bit = 7;
		for(i=x;i<x+8;i++)
		{
			/*���ݵ����ĳλ�����Ƿ�����ɫ*/
			if(data & (1<<bit))
			{
				fb_put_pixel(i,j,colour);
			}
			bit--;/*���ش�����֮��,Ҫ�ƶ�����һλ*/	
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
    		if(x>xres) /*����*/
    		{
    			y += 16;
    			x=0;
    		}
		}
		i++;

	}

}



