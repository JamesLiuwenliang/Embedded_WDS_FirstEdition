#include "../s3c2440_soc.h"
#include "touchscreen.h"



#define ADC_INT_BIT (10)
#define TC_INT_BIT  (9)

#define INT_ADC_TC   (31)


/* ADCTSC's bits */
#define WAIT_PEN_DOWN    (0<<8)
#define WAIT_PEN_UP      (1<<8)

#define YM_ENABLE        (1<<7)
#define YM_DISABLE       (0<<7)

#define YP_ENABLE        (0<<6)
#define YP_DISABLE       (1<<6)

#define XM_ENABLE        (1<<5)
#define XM_DISABLE       (0<<5)

#define XP_ENABLE        (0<<4)
#define XP_DISABLE       (1<<4)

#define PULLUP_ENABLE    (0<<3)
#define PULLUP_DISABLE   (1<<3)

#define AUTO_PST         (1<<2)

#define WAIT_INT_MODE    (3)
#define NO_OPR_MODE      (0)

static volatile int g_ts_timer_enable = 0;

static int g_ts_x;
static int g_ts_y;
static int g_ts_pressure;

//Ô­À´ÊÇchar »¹ÓÐvolatile
static int g_ts_data_valid = 0;/*charÒ»´ÎÐÔ¾Í»á±»²ð·ÖÍê,ÕâÀïÊ¹ÓÃvolatileµÄÄ¿µÄÊÇÖ±½Ó²Ù×÷ÄÚ´æ,±ÜÃâ±»ÓÅ»¯*/


void enter_wait_pen_down_mode(void)
{
	ADCTSC = WAIT_PEN_DOWN | PULLUP_ENABLE | YM_ENABLE | YP_DISABLE | XP_DISABLE | XM_DISABLE | WAIT_INT_MODE;
}

void enter_wait_pen_up_mode(void)
{
	ADCTSC = WAIT_PEN_UP | PULLUP_ENABLE | YM_ENABLE | YP_DISABLE | XP_DISABLE | XM_DISABLE | WAIT_INT_MODE;
}

void enter_auto_measure_mode(void)
{
	ADCTSC = AUTO_PST | NO_OPR_MODE;
}

void Isr_Tc(void)
{
	//printf("ADCUPDN = 0x%x, ADCDAT0 = 0x%x, ADCDAT1 = 0x%x, ADCTSC = 0x%x\n\r", ADCUPDN, ADCDAT0, ADCDAT1, ADCTSC);
	
	if (ADCDAT0 & (1<<15))
	{
		//printf("pen up\n\r");
		enter_wait_pen_down_mode();
		report_ts_xy(0,0,0);

		
	}
	else	
	{
		//printf("pen down\n\r");

		/* ½øÈë"×Ô¶¯²âÁ¿"Ä£Ê½ */
		enter_auto_measure_mode();

		/* Æô¶¯ADC */
		ADCCON |= (1<<0);
	}
}

void report_ts_xy(int x,int y,int pressure )
{
	if(g_ts_data_valid == 0) /**/
	{
		g_ts_x = x;
		g_ts_y = y;
		g_ts_pressure = pressure;
		g_ts_data_valid = 1;
	}
}

void ts_read_raw(int *px,int *py,int *ppressure)/*¼ÇÂ¼´¥ÃþÆÁ×ø±ê*/
{
	while(g_ts_data_valid == 0);
	
	*px = g_ts_x;
	*py = g_ts_y;

	*ppressure = g_ts_pressure;
	g_ts_data_valid = 0;
	
}
static void ts_timer_enable()
{
	g_ts_timer_enable = 1;
}
static void ts_timer_disable()
{
	g_ts_timer_enable = 0;
}
static int get_status_of_ts_timer(void)
{
	return g_ts_timer_enable;
}

/*Õâ¸öº¯ÊýÃ¿¸ô10ms»áµ÷ÓÃÒ»´Î*/
void touchscreen_timer_irq()
{
	int x = ADCDAT0;
	int y = ADCDAT1;

	/*Èç¹û´¥ÃþÆÁÈÔ±»°´ÏÂ,Æô¶¯ADC,½øÈë×Ô¶¯²âÁ¿Ä£Ê½*/
	if(get_status_of_ts_timer()==0)
		return ;

	if (x & (1<<15))  /*ËÉ¿ª*/
	{
		ts_timer_disable();
		enter_wait_pen_down_mode();
		//report_ts_xy(0,0,0);  /*×¢ÊÍµôÁË*/
		return ;
	}
	else /*°´ÏÂ×´Ì¬*/
	{
		/* ½øÈë"×Ô¶¯²âÁ¿"Ä£Ê½ */
		enter_auto_measure_mode();

		/* Æô¶¯ADC */
		ADCCON |= (1<<0);
	}


}
void Isr_Adc(void)
{
	int x = ADCDAT0;
	int y = ADCDAT1;
	
	static int adc_cnt = 0;
	static int adc_x;
	static int adc_y;


	if (!(x & (1<<15))) /* Èç¹ûÈÔÈ»°´ÏÂ²Å´òÓ¡ */
	{
		/*µÚ1´ÎÆô¶¯ADCºóå:
		 * a. ÒªÁ¬ÐøÆô¶¯n´Î,ÔÙ»ñµÃn¸öÊý¾Ý,ÇóÆ½¾ùÖµÔÙÉÏ±¨
		 * b. µÃµ½N´ÎÊý¾Ýºó,ÔÙÆô¶¯Timer
		 */
		adc_cnt ++;
		adc_x += ( x & 0x3ff );
		adc_y += ( y & 0x3ff );	

		if(adc_cnt == 16)
		{
			adc_x >> 4;
			adc_y >> 4;

			report_ts_xy(adc_x,adc_y,1);

			adc_cnt = 0;
			adc_x = 0;
			adc_y = 0;

			/*Æô¶¯¶¨Ê±Æ÷ÒÑÔÙ´Î¶ÁÈ¡Êý¾Ý*/
			ts_timer_enable();/*±êÖ¾Î»,ÅäºÏ¶¨Ê±Æ÷*/
		}
		else 
		{
			/*ÔÙ´ÎÆô¶¯ADC*/
			
			/* ½øÈë"×Ô¶¯²âÁ¿"Ä£Ê½ */
			enter_auto_measure_mode();
			
			/* Æô¶¯ADC */
			ADCCON |= (1<<0);	
			
		}
#if 0
		/*ÕâÖÖ·½Ê½ÓÐÈ±ÏÝ,×îºóËÉ¿ªµÄÒ»´Î¿ÉÄÜ»áÊÇÒ»´Î´íÎóµÄÊý¾Ý¶øÇ°ÃæµÄÊý¾Ý±»ÈÓµô,ËùÒÔ×îºóËã³öµÄ²ÎÊýÊÇÍêÈ«´íÎóµÄ*/
		x &= 0x3ff;
		y &= 0x3ff;

		report_ts_xy(&x,&y,1);

		/*Æô¶¯¶¨Ê±Æ÷ÒÑÔÙ´Î¶ÁÈ¡Êý¾Ý*/
		ts_timer_enable();/*±êÖ¾Î»,ÅäºÏ¶¨Ê±Æ÷*/
#endif
	}
	else
	{
		adc_cnt = 0;
		adc_x  = 0;
		adc_y = 0;
	
		ts_timer_disable();
		enter_wait_pen_down_mode();
		report_ts_xy(0,0,0);/*ÕâÀïÊÇ1*/
	}

	enter_wait_pen_up_mode();
}

void AdcTsIntHandle(int irq)
{
	if (SUBSRCPND & (1<<TC_INT_BIT))  /* Èç¹ûÊÇ´¥ÃþÆÁÖÐ¶Ï */
		Isr_Tc();

	if (SUBSRCPND & (1<<ADC_INT_BIT))  /* ADCÖÐ¶Ï */
		Isr_Adc();
	SUBSRCPND = (1<<TC_INT_BIT) | (1<<ADC_INT_BIT);
}

void adc_ts_int_init(void)
{
	SUBSRCPND = (1<<TC_INT_BIT) | (1<<ADC_INT_BIT);

	/* ×¢²áÖÐ¶Ï´¦Àíº¯Êý */
	register_irq(31, AdcTsIntHandle);	

	/* Ê¹ÄÜÖÐ¶Ï */
	INTSUBMSK &= ~((1<<ADC_INT_BIT) | (1<<TC_INT_BIT));
	//INTMSK    &= ~(1<<INT_ADC_TC);
}


void adc_ts_reg_init(void)
{
	/* [15] : ECFLG,  1 = End of A/D conversion
	 * [14] : PRSCEN, 1 = A/D converter prescaler enable
	 * [13:6]: PRSCVL, adc clk = PCLK / (PRSCVL + 1)
	 * [5:3] : SEL_MUX, 000 = AIN 0
	 * [2]   : STDBM
	 * [0]   : 1 = A/D conversion starts and this bit is cleared after the startup.
	 */
	ADCCON = (1<<14) | (49<<6) | (0<<3);

	/*  °´ÏÂ´¥ÃþÆÁ, ÑÓÊ±Ò»»áÔÙ·¢³öTCÖÐ¶Ï
	 *  ÑÓÊ±Ê±¼ä = ADCDLY * ¾§ÕñÖÜÆÚ = ADCDLY * 1 / 12000000 = 5ms
	 */
	ADCDLY = 60000;	
}


void touchscreen_init(void)
{
	/* ÉèÖÃ´¥ÃþÆÁ½Ó¿Ú:¼Ä´æÆ÷ */
	adc_ts_reg_init();

	printf("ADCUPDN = 0x%x, SUBSRCPND = 0x%x, SRCPND = 0x%x\n\r", ADCUPDN, SUBSRCPND, SRCPND);

	/* ÉèÖÃÖÐ¶Ï */
	adc_ts_int_init();

	/*×¢²á¶¨Ê±Æ÷´¦Àíº¯Êý*/
	register_timer("timerscreen",touchscreen_timer_irq);

	/* ÈÃ´¥ÃþÆÁ¿ØÖÆÆ÷½øÈë"µÈ´ýÖÐ¶ÏÄ£Ê½" */
	enter_wait_pen_down_mode();
}


