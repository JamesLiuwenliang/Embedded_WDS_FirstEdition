
#include "s3c2440_soc.h"

void delay(volatile int d)
{
	while (d--);
}



/*ÿ10ms�ú���������һ��(֮ǰ��ʱ�����õ�,Ϊ�˷��㴥��������)
 *	ÿ500ms����һ��LEDʵ�ּ���
 */
void led_timer_irq(void)/*��ʱ����רע�ڵ�Ʋ���*/
{
	/* ��Ƽ��� */
	static int cnt = 0;
	static int timer_num = 0;
	
	int tmp;
	timer_num++;

	if(timer_num < 50)
	{
		return ;
	}
	timer_num = 0;
	cnt++;

    tmp = ~cnt;
    tmp &= 7;
    GPFDAT &= ~(7<<4);
    GPFDAT |= (tmp<<4);


}

int led_init(void)
{
	/* ����GPFCON��GPF4/5/6����Ϊ������� */
	GPFCON &= ~((3<<8) | (3<<10) | (3<<12));
	GPFCON |=  ((1<<8) | (1<<10) | (1<<12));


	register_timer("Led" , led_timer_irq);
}


int led_test(void)
{
	int val = 0;  /* val: 0b000, 0b111 */
	int tmp;

	/* ����GPFCON��GPF4/5/6����Ϊ������� */
	GPFCON &= ~((3<<8) | (3<<10) | (3<<12));
	GPFCON |=  ((1<<8) | (1<<10) | (1<<12));

	/* ѭ������ */
	while (1)
	{
		tmp = ~val;
		tmp &= 7;
		GPFDAT &= ~(7<<4);
		GPFDAT |= (tmp<<4);
		delay(100000);
		val++;
		if (val == 8)
			val =0;
		
	}

	return 0;
}


