
#include "s3c2440_soc.h"

/*SRCPND ÓÃÀ´ÏÔÊ¾ÄÄ¸öÖĞ¶Ï²úÉúÁË,ĞèÒªÇå³ı¶ÔÓ¦Î»
 * bit0 - eint0
 * bit2 - eint2
 * bit5 - eint8-23
 */

/* INTMASK ÓÃÀ´ÆÁ±ÎÖĞ¶Ï, 1-masked
 * bit0 - eint0
 * bit2 - eint2
 * bit5 - eint8-23
 */
 
/* INTPEND ÓÃÀ´¶ÁÓÅÏÈ¼¶×î¸ßµÄ,ÕıÔÚ·¢ÉúµÄÖĞ¶Ï,ĞèÒªÇå³ı¶ÔÓ¦Î»
 * bit0 - eint0
 * bit2 - eint2
 * bit5 - eint8-23
 */

/* INTOFFSET : ÓÃÀ´ÏÔÊ¾INTPNDÖĞÄÄÒ»Î»±»ÉèÖÃÎª1*/
void interrupt_init(void)
{
	INTMSK &= ~((1<<0)|(1<<2)|(1<<5));
	
	INTMSK &= ~(1<<10);


}
void key_eint_init(void)
{

	/*ÖĞ¶ÏÔ´³õÊ¼»¯*/
	/*ÅäÖÃGPIOÎªÖĞ¶ÏÒı½Å*/

	
	GPFCON &= ~((3<<0)|(3<<4));		/*Çå0*/
	GPFCON |=  ((2<<0)|(2<<4));		/*S2,S3*/
	
	GPGCON &= ~((3<<6)|(3<<22));	/*ÎªÊ²Ã´å?*/
	GPGCON |=  ((2<<6)|(2<<22));	/*S4,S5*/

	/*ÉèÖÃÖĞ¶Ï´¥·¢·½Ê½:Ë«±ßÑØ´¥·¢*/
	EXTINT0  |=  ((7<<0) | (7<<8));

	EXTINT1  |=  ((7<<12));
	
	EXTINT2  |=  ((7<<12));

	/* Ê¹ÄÜÖĞ¶Ï */
	EINTMASK &= ~((1<<11) | (1<<19));  /*Íâ²¿ÖĞ¶Ï0,1,2Ö±½ÓÁ¬½ÓÖĞ¶Ï¿ØÖÆÆ÷*/
	
	/*ÖĞ¶Ï¿ØÖÆÆ÷,datasheet chap14*/



	/**/


}
/*¶ÁEINTPEND·Ö±æÄÄ¸öEINT²úÉúÖĞ¶Ï
 *Çå³ıÖĞ¶ÏÊ±,Ğ´EINTPENDµÄÏàÓ¦Î»
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
		 	/*ËÉ¿ª*/
			GPFDAT |= (1<<6);
		}
		else
		{
			/*°´ÏÂ*/
			GPFDAT &= ~(1<<6);
		}

	}
	else if(irq==2)
	{
		if(val1&(1<<2))
		{		
		 	/*ËÉ¿ª*/
			GPFDAT |= (1<<5);
		}
		else
		{
			/*°´ÏÂ*/
			GPFDAT &= ~(1<<5);
		}

	}
	else if(irq==5)
	{
		if(val & (1<<19)){

     		if(val2&(1<<11))
          	{		
          		/*ËÉ¿ª,Ï¨ÃğËùÓĞµÆ*/
				GPFDAT |= ((1<<4) | (1<<5) | (1<<6));
          	}
          	else
          	{
          		/*°´ÏÂ,µãÁÁËùÓĞµÆ*/
          		GPFDAT &=  ~((1<<4) | (1<<5) | (1<<6));
          	}

		}
		else if(val & (1<<11)){

          	if(val2&(1<<3))
          	{		
          		/*ËÉ¿ª*/
          		GPFDAT |= (1<<4);
          	}
          	else
          	{
          		/*°´ÏÂ*/
          		GPFDAT &= ~(1<<4);
          	}


		}



	}

	EINTPEND = val;  /*ÇåÖĞ¶Ï*/
}


void handle_irq_c(void)
{
	/*·Ö±æÖĞ¶ÏÔ´*/
	int bit = INTOFFSET;



	/*µ÷ÓÃ¶ÔÓ¦µÄ´¦Àíº¯Êı*/
	if((bit==0) || (bit==2) || (bit==5)) /*eint0,2,eint8_23*/
	{
		key_eint_irq(bit); /*´¦ÀíÖĞ¶Ï,ÇåÖĞ¶ÏÔ´*/

	}
	else if(bit == 10 ){

		timer_irq();
	}
	
	/*ÇåÖĞ¶Ï:´ÓÔ´Í·¿ªÊ¼Çå */
	SRCPND = (1<<bit);
	INTPND = (1<<bit);

}



