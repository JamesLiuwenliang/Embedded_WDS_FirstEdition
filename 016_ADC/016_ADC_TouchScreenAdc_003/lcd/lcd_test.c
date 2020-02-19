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
	
	/* ��ʼ��LCD */
	lcd_init();

	/* ʹ��LCD */
	lcd_enable();
	
	/* �Ȼ��LCD����: unsigned int *fb_base,int *xres ,int *yres,int *bpp  */
	get_lcd_params(&fb_base,&xres,&yres,&bpp);
	fb_get_lcd_params();
	font_init();

	/*��frame_buffer��д����*/
	if(bpp==16)
	{
		/*��LCD��������ĺ�ɫ*/
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
		/*��LCD��������ĺ�ɫ*/
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
		/*��LCD��������ĺ�ɫ*/
		/*bpp : palette[12] */
		p0 = (unsigned char*)fb_base;
		for(i=0;i<xres;i++)
		{
			for(j=0;j<yres;j++)
			{
				*p0++ = 12; //���12�ᱻ���������ĵ�palette[]��
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
	
	/* ���� */
	/* 8bppֻ�õ���͵�һ�ֽ� */
	draw_line(0, 0, xres - 1, 0, 0xff);
	draw_line(xres - 1, 0, xres - 1, yres - 1, 0xff);
	draw_line(0, yres - 1, xres - 1, yres - 1, 0xff);
	draw_line(0, 0, 0, yres - 1, 0xff);
	draw_line(0, 0, xres - 1, yres - 1, 0xff);
	draw_line(xres - 1, 0, 0, yres - 1, 0xff);

	delay(100000);

	/* ��Բ */
	draw_circle(xres/2, yres/2, yres/4, 0xff);


	/*�������*/
	fb_print_string(10,10,"liuwenliang \r\n HEU",0xff);



}
