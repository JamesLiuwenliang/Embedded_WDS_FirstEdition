
#include "s3c2440_soc.h"

/*SRCPND ������ʾ�ĸ��жϲ�����,��Ҫ�����Ӧλ
 * bit0 - eint0
 * bit2 - eint2
 * bit5 - eint8-23
 */

/* INTMASK ���������ж�, 1-masked
 * bit0 - eint0
 * bit2 - eint2
 * bit5 - eint8-23
 */
 
/* INTPEND ���������ȼ���ߵ�,���ڷ������ж�,��Ҫ�����Ӧλ
 * bit0 - eint0
 * bit2 - eint2
 * bit5 - eint8-23
 */

/* INTOFFSET : ������ʾINTPND����һλ������Ϊ1*/
void interrupt_init(void)
{
	INTMSK &= ~((1<<0)|(1<<2)|(1<<5));
	
	INTMSK &= ~(1<<10);


}
void key_eint_init(void)
{

	/*�ж�Դ��ʼ��*/
	/*����GPIOΪ�ж�����*/

	
	GPFCON &= ~((3<<0)|(3<<4));		/*��0*/
	GPFCON |=  ((2<<0)|(2<<4));		/*S2,S3*/
	
	GPGCON &= ~((3<<6)|(3<<22));	/*Ϊʲô�?*/
	GPGCON |=  ((2<<6)|(2<<22));	/*S4,S5*/

	/*�����жϴ�����ʽ:˫���ش���*/
	EXTINT0  |=  ((7<<0) | (7<<8));

	EXTINT1  |=  ((7<<12));
	
	EXTINT2  |=  ((7<<12));

	/* ʹ���ж� */
	EINTMASK &= ~((1<<11) | (1<<19));  /*�ⲿ�ж�0,1,2ֱ�������жϿ�����*/
	
	/*�жϿ�����,datasheet chap14*/



	/**/


}
/*��EINTPEND�ֱ��ĸ�EINT�����ж�
 *����ж�ʱ,дEINTPEND����Ӧλ
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
		 	/*�ɿ�*/
			GPFDAT |= (1<<6);
		}
		else
		{
			/*����*/
			GPFDAT &= ~(1<<6);
		}

	}
	else if(irq==2)
	{
		if(val1&(1<<2))
		{		
		 	/*�ɿ�*/
			GPFDAT |= (1<<5);
		}
		else
		{
			/*����*/
			GPFDAT &= ~(1<<5);
		}

	}
	else if(irq==5)
	{
		if(val & (1<<19)){

     		if(val2&(1<<11))
          	{		
          		/*�ɿ�,Ϩ�����е�*/
				GPFDAT |= ((1<<4) | (1<<5) | (1<<6));
          	}
          	else
          	{
          		/*����,�������е�*/
          		GPFDAT &=  ~((1<<4) | (1<<5) | (1<<6));
          	}

		}
		else if(val & (1<<11)){

          	if(val2&(1<<3))
          	{		
          		/*�ɿ�*/
          		GPFDAT |= (1<<4);
          	}
          	else
          	{
          		/*����*/
          		GPFDAT &= ~(1<<4);
          	}


		}



	}

	EINTPEND = val;  /*���ж�*/
}


void handle_irq_c(void)
{
	/*�ֱ��ж�Դ*/
	int bit = INTOFFSET;



	/*���ö�Ӧ�Ĵ�����*/
	if((bit==0) || (bit==2) || (bit==5)) /*eint0,2,eint8_23*/
	{
		key_eint_irq(bit); /*�����ж�,���ж�Դ*/

	}
	else if(bit == 10 ){

		timer_irq();
	}
	
	/*���ж�:��Դͷ��ʼ�� */
	SRCPND = (1<<bit);
	INTPND = (1<<bit);

}



