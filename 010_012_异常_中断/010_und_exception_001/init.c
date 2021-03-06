
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

void copy2sdram(void)
{
	/* 要从lds文件中获得 __code_satrt , __bss_start
	 * 然后从0地址把数据复制到 __code_start
	 */

	extern int __code_start,__bss_start; 
	/*C函数怎么使用lds文件中的变量abc? �
	 *C函数中声明改变量为extern类型,比如extern int abc
	 * 使用时,要取地址,比如 int *p = &abc
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
	/* 要从lds文件中获得 __bss_start , _end
	 * 然后从0地址把数据复制到 __code_start
	 */
	extern int __bss_start,_end;
	volatile unsigned int *start = (volatile unsigned int *)& __bss_start;
	volatile unsigned int *end = (volatile unsigned int *) & _end;


	while(start <= end)
	{
		*start ++ = 0 ;
	}
}


