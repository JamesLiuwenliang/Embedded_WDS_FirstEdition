
#include "s3c2440_soc.h"

/*参考chap10,pwm*/
void timer_init(void)
{
	/*Timer  clk = PCLK / {prescaler value+1} / {divider value}  
					50M /(99 + 1) /16 ; 99 16 随便设的
				 = 31250
	*/

	/*设置TIMER0的时钟*/
	TCFG0  = 99 ; 		/*prescaler 0 =99,用于timer0,1 */
	
	TCFG1 &= ~0xf;	
	TCFG1 |=  3; 		/*MUX0 : 1/16*/

	
	

	/*设置TIMER0的初值*/
	TCNTB0 = 15625 * 2;

	/*加载初值,启动timer0*/
	TCON |= (1<<1);
	
	/*设置为自动加载,并启动*/
	TCON &=  ~(1<<1);
	TCON |= (1<<0) | (1<<3)  ; /*bit3 : start ; bit0 : reload*/

	/*设置中断*/
	/*只需要在interrupt中设置中断控制器*/

}

void timer_irq(void)
{
	
	volatile unsigned int *pGPFDAT = (volatile unsigned int *)0x56000054;

	/*点灯计数*/
	static int cnt = 0;
	int tmp ;

	cnt++;
	
	tmp = ~cnt;
	tmp &= 7;
		
	*pGPFDAT &= ~(7<<4);
	*pGPFDAT |= (tmp<<4);


}


