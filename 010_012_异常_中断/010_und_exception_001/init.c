
#include "s3c2440_soc.h"
void sdram_init(void)
{
	/*¸ù¾Ý±Ê¼ÇÉèÖÃ¼Ä´æÆ÷*/
	BWSCON = 0x22000000;

	BANKCON6 = 0x18001;
	BANKCON7 = 0x18001;

	REFRESH = 0x8404F5;
	
	BANKSIZE = 0xB1;

	MRSRB6 = 0x20;
	MRSRB7 = 0x20;

}

int sdram_test (void)
{
	volatile unsigned char *p = (volatile unsigned char *)0x30000000; /*Ö¸ÏòSDRAMµÄ»ùµØÖ·*/
	int i;

	// write sdram
	for(i=0;i<1000;i++)
	{
		p[i]=0x55;
	}

    // read sdram
    for(i=0;i<1000;i++)
    {
    	if(p[i]!=0x55){
			return -1;
		}
	}

	return 0;



}

void copy2sdram(void)
{
	/* Òª´ÓldsÎÄ¼þÖÐ»ñµÃ __code_satrt , __bss_start
	 * È»ºó´Ó0µØÖ·°ÑÊý¾Ý¸´ÖÆµ½ __code_start
	 */

	extern int __code_start,__bss_start; 
	/*Cº¯ÊýÔõÃ´Ê¹ÓÃldsÎÄ¼þÖÐµÄ±äÁ¿abc? Ô
	 *Cº¯ÊýÖÐÉùÃ÷¸Ä±äÁ¿ÎªexternÀàÐÍ,±ÈÈçextern int abc
	 * Ê¹ÓÃÊ±,ÒªÈ¡µØÖ·,±ÈÈç int *p = &abc
	 */

	volatile unsigned int *dest = (volatile unsigned int *)& __code_start;
	volatile unsigned int *end = (volatile unsigned int *) & __bss_start;
	volatile unsigned int *src = (volatile unsigned int *) 0;

	
	unsigned int i=0;
	
	while(dest < end)
	{
		*dest++ = *src++;
	}
}


void clean_bss(void)
{
	/* Òª´ÓldsÎÄ¼þÖÐ»ñµÃ __bss_start , _end
	 * È»ºó´Ó0µØÖ·°ÑÊý¾Ý¸´ÖÆµ½ __code_start
	 */
	extern int __bss_start,_end;
	volatile unsigned int *start = (volatile unsigned int *)& __bss_start;
	volatile unsigned int *end = (volatile unsigned int *) & _end;


	while(start <= end)
	{
		*start ++ = 0 ;
	}
}


