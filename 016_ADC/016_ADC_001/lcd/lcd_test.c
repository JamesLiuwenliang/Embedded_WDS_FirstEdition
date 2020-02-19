#include "geometry.h"
#include "framebuff.h"
#include "font.h"


void lcd_test(void)
{
    unsigned int fb_base;
    int xres , yres, bpp ;	
	int i,j,k;
	unsigned char *p0;
	unsigned short *p;
	unsigned int *p2;
	
	/* 初始化LCD */
	lcd_init();

	/* 使能LCD */
	lcd_enable();
	
	/* 先获得LCD参数: unsigned int *fb_base,int *xres ,int *yres,int *bpp  */
	get_lcd_params(&fb_base,&xres,&yres,&bpp);
	fb_get_lcd_params();
	font_init();

	/*往frame_buffer中写数据*/
	if(bpp==16)
	{
		/*让LCD输出整屏的红色*/
		/*red 565 : 0xf800 */
		p = (unsigned short*)fb_base;
		for(i=0;i<xres;i++)
		{
			for(j=0;j<yres;j++)
			{
				*p++ = 0xf800;
			}
		}
		
		//green
		p = (unsigned short*)fb_base;
		for(i=0;i<xres;i++)
		{
			for(j=0;j<yres;j++)
			{
				*p++ = 0x7e0;
			}
		}

		//blue
		p = (unsigned short*)fb_base;
    	for(i=0;i<xres;i++)
    	{
    		for(j=0;j<yres;j++)
    		{
    			*p++ = 0x1f;
    		}
    	}

		//black
		p = (unsigned short*)fb_base;
    	for(i=0;i<xres;i++)
    	{
    		for(j=0;j<yres;j++)
    		{
    			*p++ = 0x0;
    		}
    	}
	}
	else if(bpp==32)
	{
		/*让LCD输出整屏的红色*/
		/*red 565 : 0xf800 */
		p2 = (unsigned int*)fb_base;
    	for(i=0;i<xres;i++)
    	{
    		for(j=0;j<yres;j++)
    		{
    			*p2++ = 0xff0000;
    		}
    	}

		//green
		p2 = (unsigned int*)fb_base;
    	for(i=0;i<xres;i++)
    	{
    		for(j=0;j<yres;j++)
    		{
    			*p2++ = 0x00ff00;
    		}
    	}

		//blue
    	p2 = (unsigned int*)fb_base;
    	for(i=0;i<xres;i++)
    	{
    		for(j=0;j<yres;j++)
    		{
    			*p2++ = 0x0000ff;
    		}
    	}

		p2 = (unsigned int*)fb_base;
    	for(i=0;i<xres;i++)
    	{
    		for(j=0;j<yres;j++)
    		{
    			*p2++ = 0x0;
    		}
    	}
	}
	else if(bpp==8)
	{
		/*让LCD输出整屏的红色*/
		/*bpp : palette[12] */
		p0 = (unsigned char*)fb_base;
		for(i=0;i<xres;i++)
		{
			for(j=0;j<yres;j++)
			{
				*p0++ = 12; //这个12会被放置在上文的palette[]中
			}
		}
		
		// palette[47]
		p0 = (unsigned char*)fb_base;
		for(i=0;i<xres;i++)
		{
			for(j=0;j<yres;j++)
			{
				*p0++ = 47;
			}
		}

		// palette[56]
		p0 = (unsigned char*)fb_base;
    	for(i=0;i<xres;i++)
    	{
    		for(j=0;j<yres;j++)
    		{
    			*p0++ = 56;
    		}
    	}

		// palette[0]
		p0 = (unsigned char*)fb_base;
    	for(i=0;i<xres;i++)
    	{
    		for(j=0;j<yres;j++)
    		{
    			*p0++ = 0;
    		}
    	}
	}


	delay(100000);
	
	/* 画线 */
	/* 8bpp只用到最低的一字节 */
	draw_line(0, 0, xres - 1, 0, 0xff);
	draw_line(xres - 1, 0, xres - 1, yres - 1, 0xff);
	draw_line(0, yres - 1, xres - 1, yres - 1, 0xff);
	draw_line(0, 0, 0, yres - 1, 0xff);
	draw_line(0, 0, xres - 1, yres - 1, 0xff);
	draw_line(xres - 1, 0, 0, yres - 1, 0xff);

	delay(100000);

	/* 画圆 */
	draw_circle(xres/2, yres/2, yres/4, 0xff);


	/*输出文字*/
	fb_print_string(10,10,"liuwenliang \r\n HEU",0xff);



}
