#include "adc.h"
#include "../my_printf.h"
#include "../lcd/font.h"
void adc_test()
{
	int val;
	double vol;
	int m;//��������
	int n;//С������

	adc_init();

	while(1)
	{
		val = adc_read_ain0();
		vol = (double)val/1023*3.3;   /* 1023----3.3v */

		 m  = (int)vol;
		vol = vol -m;
		  n = vol*1000;
		 

		/*�ڴ����ϴ�ӡ*/
		printf("vol: %d.%03dv\r", m, n);  /* 3.010v */
		
	}


}



