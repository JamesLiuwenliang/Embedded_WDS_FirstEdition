
#include "s3c2440_soc.h"
#include "uart.h"
#include "init.h"
int main(void)
{
	unsigned char c;
	
	uart0_init();
	puts("Enter the Tacc Value : \r\n");
	
	while(1)
	{
		c = getchar();
		
		putchar(c);

		if(c>='0'&&c<='7')
		{
			bank0_tacc_set(c-'0');/*������д��Nor Flash �� ͨŪ�ı�tacc��ֵ ��ȡNor Flash���ٶȻ᲻ͬ*/
			/*��Ϊtacc��ʱ��������70ns,����ֻ������5��ʱ��Ż����,С��5�ͻ᲻��˸ (HCLK = 12 MHz)*/
			led_test();

		}
		else
		{
			puts("Error, val should between: 0-7 \r\n");
		}



	}
	return 0;
}



