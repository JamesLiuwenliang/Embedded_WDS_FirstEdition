#include "geometry.h"
#include "framebuff.h"




void lcd_test(void)
{
    unsigned int fb_base;
    int xres , yres, bpp ;	
	int i,j,k;
	unsigned short *p;
	unsigned int *p2;
	
	/* ��ʼ��LCD */
	lcd_init();

	/* ʹ��LCD */
	lcd_enable();
	
	/* �Ȼ��LCD����: unsigned int *fb_base,int *xres ,int *yres,int *bpp  */
	get_lcd_params(&fb_base,&xres,&yres,&bpp);
	fb_get_lcd_params();


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


	delay(100000);

	//delay(1000000);
	
	/* ���� */
	draw_line(0, 0, xres - 1, 0, 0xff0000);
	draw_line(xres - 1, 0, xres - 1, yres - 1, 0xffff00);
	draw_line(0, yres - 1, xres - 1, yres - 1, 0xff00aa);
	draw_line(0, 0, 0, yres - 1, 0xff00ef);
	draw_line(0, 0, xres - 1, yres - 1, 0xff4500);
	draw_line(xres - 1, 0, 0, yres - 1, 0xff0780);

	delay(100000);

	/* ��Բ */
	draw_circle(xres/2, yres/2, yres/4, 0xff00);




}
