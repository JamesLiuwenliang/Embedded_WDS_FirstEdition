
#include "s3c2440_soc.h"
#include "uart.h"
#include "init.h"

char g_Char = 'A';
const char g_char2 = 'B';

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



