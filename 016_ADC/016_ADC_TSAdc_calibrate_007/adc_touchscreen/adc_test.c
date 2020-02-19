#include "adc.h"
#include "../my_printf.h"
#include "../lcd/font.h"
void adc_test()
{
	int val;
	double vol;
	int m;//整数部分
	int n;//小数部分

	adc_init();

	while(1)
	{
		val = adc_read_ain0();
		vol = (double)val/1023*3.3;   /* 1023----3.3v */

		 m  = (int)vol;
		vol = vol -m;
		  n = vol*1000;
		 

		/*在串口上打印*/
		printf("vol: %d.%03dv\r", m, n);  /* 3.010v */
		
	}


}



