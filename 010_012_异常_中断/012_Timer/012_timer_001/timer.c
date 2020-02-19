
#include "s3c2440_soc.h"

/*�ο�chap10,pwm*/
void timer_init(void)
{
	/*Timer  clk = PCLK / {prescaler value+1} / {divider value}  
					50M /(99 + 1) /16 ; 99 16 ������
				 = 31250
	*/

	/*����TIMER0��ʱ��*/
	TCFG0  = 99 ; 		/*prescaler 0 =99,����timer0,1 */
	
	TCFG1 &= ~0xf;	
	TCFG1 |=  3; 		/*MUX0 : 1/16*/

	
	

	/*����TIMER0�ĳ�ֵ*/
	TCNTB0 = 15625 * 2;

	/*���س�ֵ,����timer0*/
	TCON |= (1<<1);
	
	/*����Ϊ�Զ�����,������*/
	TCON &=  ~(1<<1);
	TCON |= (1<<0) | (1<<3)  ; /*bit3 : start ; bit0 : reload*/

	/*�����ж�*/
	/*ֻ��Ҫ��interrupt�������жϿ�����*/

}

void timer_irq(void)
{
	
	volatile unsigned int *pGPFDAT = (volatile unsigned int *)0x56000054;

	/*��Ƽ���*/
	static int cnt = 0;
	int tmp ;

	cnt++;
	
	tmp = ~cnt;
	tmp &= 7;
		
	*pGPFDAT &= ~(7<<4);
	*pGPFDAT |= (tmp<<4);


}


