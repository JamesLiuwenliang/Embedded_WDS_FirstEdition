
#include "s3c2440_soc.h"
void sdram_init(void)
{
	/*根据笔记设置寄存器*/
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
	volatile unsigned char *p = (volatile unsigned char *)0x30000000; /*指向SDRAM的基地址*/
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





