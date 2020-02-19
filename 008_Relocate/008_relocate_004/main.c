
#include "s3c2440_soc.h"
#include "uart.h"
#include "init.h"

char g_Char = 'A';
char g_Char3 = 'a';
const char g_Char2 = 'B';
int g_A = 0;/*g_A算是在bss段代码* /
int g_B;

int main(void)
{
	uart0_init();

	puts("\r\ng_A = ");
	printHex(g_A);/* 会打印出一个莫名其妙的值,主要的原因是bss段没有清0*/
	puts("\n\r");
	
	while (1)
	{
		putchar(g_Char);
		g_Char++;

		putchar(g_Char3);
		g_Char3++;
		delay(1000000);
	}

	
	return 0;
}




