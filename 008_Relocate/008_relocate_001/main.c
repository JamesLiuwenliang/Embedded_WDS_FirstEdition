
#include "s3c2440_soc.h"
#include "uart.h"
#include "init.h"

char g_Char = 'A';
const char g_char2 = 'B';
/* elf�ļ��е�0x800�����д��һ��λ��,��������bin�ļ�����;
 * ͬʱ��Ϊ0x800��2048,Ȼ����һ�����ݶεı���g_Char,ռһ���ֽ�,�������յ�bin�ļ�Ӧ����2049�Ĵ�С
 */
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



