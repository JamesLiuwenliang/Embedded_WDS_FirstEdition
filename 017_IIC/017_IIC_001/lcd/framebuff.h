
#ifndef _FRAMEBUFF_H
#define _FRAMEBUFF_H


void fb_get_lcd_params(void);
unsigned short convert32bppto16bpp(unsigned int rgb);
void fb_put_pixel(int x,int y,unsigned int colour);
void clear_screen(unsigned int color);

void fb_disp_cross(int x,int y,unsigned int colour);



#endif



