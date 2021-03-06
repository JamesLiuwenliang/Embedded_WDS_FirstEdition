
#include "s3c2440_soc.h"

/*SRCPND 用来显示哪个中断产生了,需要清除对应位
 * bit0 - eint0
 * bit2 - eint2
 * bit5 - eint8-23
 */

/* INTMASK 用来屏蔽中断, 1-masked
 * bit0 - eint0
 * bit2 - eint2
 * bit5 - eint8-23
 */
 
/* INTPEND 用来读优先级最高的,正在发生的中断,需要清除对应位
 * bit0 - eint0
 * bit2 - eint2
 * bit5 - eint8-23
 */

/* INTOFFSET : 用来显示INTPND中哪一位被设置为1*/
void interrupt_init(void)
{
	INTMSK &= ~((1<<0)|(1<<2)|(1<<5));
	


}
void key_eint_init(void)
{

	/*中断源初始化*/
	/*配置GPIO为中断引脚*/

	
	GPFCON &= ~((3<<0)|(3<<4));		/*清0*/
	GPFCON |=  ((2<<0)|(2<<4));		/*S2,S3*/
	
	GPGCON &= ~((3<<6)|(3<<22));	/*为什么�?*/
	GPGCON |=  ((2<<6)|(2<<22));	/*S4,S5*/

	/*设置中断触发方式:双边沿触发*/
	EXTINT0  |=  ((7<<0) | (7<<8));

	EXTINT1  |=  ((7<<12));
	
	EXTINT2  |=  ((7<<12));

	/* 使能中断 */
	EINTMASK &= ~((1<<11) | (1<<19));  /*外部中断0,1,2直接连接中断控制器*/
	
	/*中断控制器,datasheet chap14*/



	/**/


}
/*读EINTPEND分辨哪个EINT产生中断
 *清除中断时,写EINTPEND的相应位
 */
void key_eint_irq(int irq)
{
	unsigned int val = EINTPEND;
	unsigned int val1 = GPFDAT;
	unsigned int val2 = GPGDAT;
	
	if(irq==0)
	{
		if(val1&(1<<0))
		{		
		 	/*松开*/
			GPFDAT |= (1<<6);
		}
		else
		{
			/*按下*/
			GPFDAT &= ~(1<<6);
		}

	}
	else if(irq==2)
	{
		if(val1&(1<<2))
		{		
		 	/*松开*/
			GPFDAT |= (1<<5);
		}
		else
		{
			/*按下*/
			GPFDAT &= ~(1<<5);
		}

	}
	else if(irq==5)
	{
		if(val & (1<<19)){

     		if(val2&(1<<11))
          	{		
          		/*松开,熄灭所有灯*/
				GPFDAT |= ((1<<4) | (1<<5) | (1<<6));
          	}
          	else
          	{
          		/*按下,点亮所有灯*/
          		GPFDAT &=  ~((1<<4) | (1<<5) | (1<<6));
          	}

		}
		else if(val & (1<<11)){

          	if(val2&(1<<3))
          	{		
          		/*松开*/
          		GPFDAT |= (1<<4);
          	}
          	else
          	{
          		/*按下*/
          		GPFDAT &= ~(1<<4);
          	}


		}



	}

	EINTPEND = val;  /*清中断*/
}


void handle_irq_c(void)
{
	/*分辨中断源*/
	int bit = INTOFFSET;



	/*调用对应的处理函数*/
	if((bit==0) || (bit==2) || (bit==5)) /*eint0,2,eint8_23*/
	{
		key_eint_irq(bit); /*处理中断,清中断源*/

	}
	
	/*清中断:从源头开始清 */
	SRCPND = (1<<bit);
	INTPND = (1<<bit);

}



