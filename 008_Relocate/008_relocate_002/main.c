
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
		g_Char++; /*Nor ����ʱ �˴�����Ч,��ΪNor�ϵ��ڴ治�ܱ��޸�*/
		delay(1000000);
	}


	return 0;

}



