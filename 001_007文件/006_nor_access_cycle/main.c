
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
			bank0_tacc_set(c-'0');/*必须烧写到Nor Flash 上 通弄改变tacc的值 读取Nor Flash的速度会不同*/
			/*因为tacc的时间必须大于70ns,所以只有输入5的时候才会大于,小于5就会不闪烁 (HCLK = 12 MHz)*/
			led_test();

		}
		else
		{
			puts("Error, val should between: 0-7 \r\n");
		}



	}
	return 0;
}



