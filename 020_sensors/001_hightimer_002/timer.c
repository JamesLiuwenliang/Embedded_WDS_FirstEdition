
#include "s3c2440_soc.h"

#define TIMER_NUM  32
#define NULL  ((void *)0)

typedef void(*timer_func)(void);

typedef struct timer_desc {
	char *name;
	timer_func fp;
}timer_desc, *p_timer_desc;

timer_desc timer_array[TIMER_NUM];


int register_timer(char *name, timer_func fp)
{
	int i;
	for (i = 0; i < TIMER_NUM; i++)
	{
		if (!timer_array[i].fp)
		{
			timer_array[i].name = name;
			timer_array[i].fp   = fp;
			return 0;
		}
	}
	return -1;
}

void unregister_timer(char *name)
{
	int i;
	for (i = 0; i < TIMER_NUM; i++)
	{
		if (!strcmp(timer_array[i].name, name))
		{
			timer_array[i].name = NULL;
			timer_array[i].fp   = NULL;
			return 0;
		}
	}
	return -1;
}


void timer_irq(void)
{
	int i;
	for (i = 0; i < TIMER_NUM; i++)
	{
		if (timer_array[i].fp)
		{
			timer_array[i].fp();
		}
	}	
}


void timer_init(void)
{
	/* ����TIMER0��ʱ�� */
	/* Timer clk = PCLK / {prescaler value+1} / {divider value} 
	             = 50000000/(4+1)/2
	             = 5000000
	   ÿ��1,��Ӧ0.2us
	   ÿ��5,��Ӧ1us
	   
	 */
	TCFG0 = 4;  /* Prescaler 0 = 49, ����timer0,1 */
	TCFG1 &= ~0xf; /* MUX0 : 1/2 */

	/* ����TIMER0�ĳ�ֵ */
	TCNTB0 = 50000;  /* 10Ms�ж�һ�� */

	/* ���س�ֵ, ����timer0 */
	TCON |= (1<<1);   /* Update from TCNTB0 & TCMPB0 */

	/* ����Ϊ�Զ����ز����� */
	TCON &= ~(1<<1);
	TCON |= (1<<0) | (1<<3);  /* bit0: start, bit3: auto reload */

	/* �����ж� */
	register_irq(10, timer_irq);
}


/*���ú�������Ҳ��ʱ��,������ò��þ������ú���*/
void udelay(int n)
{
	int pre = TCNTO0 ;
	int cur ; 
	int cnt = n * 5;/* u us ��Ӧn*5������ֵ*/
	int delta;

	while(cnt>0)
	{
		cur = TCNTO0;
		if(cur>pre)
		{
			delta = pre + (50000 - cur);
		}
		else 
		{
			delta = pre - cur;
		}

		cnt = cnt - delta;
		pre = cur;
	}
}

void mdelay(int m)/*��ʱ֮ǰ���ж�,��ʱ֮���ж�*/
{
	/* m ms ��Ӧn*5000��ֵ*/
	udelay(m * 1000);
}


void hrtimer_test()/*����ʱ��*/
{
	int cnt =0;
	while(1)
	{
		printf("delay one minute: ");
		mdelay(60000); /*��ʱ1����*/
		printf("%d \r\n",++cnt);
		
	}

}


