
#include "s3c2440_soc.h"
#include "uart.h"
#include "init.h"

char g_Char = 'A';
const char g_char2 = 'B';
/* elf文件中的0x800是随便写的一个位置,否则容易bin文件过大;
 * 同时因为0x800是2048,然后有一个数据段的变量g_Char,占一个字节,所以最终的bin文件应该是2049的大小
 */
int main(void)
{

	uart0_init();

	while(1)
		{

		putchar(g_Char);
		g_Char++; /*Nor 启动时 此代码无效,因为Nor上的内存不能被修改*/
		delay(1000000);
	}


	return 0;

}



