#include "../my_printf.h"
#include "touchscreen.h"
#include "../s3c2440_soc.h"

#define ADC_INIT_BIT (10)
#define TC_INIT_BIT (9)
#define INT_ADC (31)

/*ADCTSC's bits*/
#define WAIT_PEN_DOWN    (0<<8)
#define WAIT_PEN_UP      (1<<8)


#define YM_ENABLE (1<<7)
#define YM_DISENABLE (0<<7)

#define YP_ENABLE (0<<6)
#define YP_DISENABLE (1<<6)

#define XM_ENABLE (1<<5)
#define XM_DISENABLE (0<<5)

#define XP_ENABLE (0<<4)
#define XP_DISENABLE (1<<4)

#define PULL_UP_ENABLE  (0<<3)
#define PULL_UP_DISENABLE (1<<3)

#define AUTO_PST (1<<2)

#define WAIT_INT_MODE (3)
#define NO_OPR_MODE (0)
void ISR_Tc()
{
	/*ADCUPDN */
	if(ADCDAT0 & (1<<15) )
	{
		printf("pen up \r\n");
		enter_wait_pen_down_mode();
	}
	else
	{ 
		printf("pen down \r\n");
		/*进入"等待触摸笔松开模式"*/
		enter_wait_pen_up_mode();
		
	}

	

}

void IRQ_Adc()
{



}

void AdcTsIntHandle(int irq )
{
	if (SUBSRCPND & (1<<TC_INIT_BIT) ) /*如果是触摸屏中断*/
	{
		ISR_Tc();
	}

	SUBSRCPND = (1<<TC_INIT_BIT) | (1<<ADC_INIT_BIT) ;
	#if 0 
 	if(SUBSRCPND & (1<<ADC_INIT_BIT)  )/*如果是ADC中断*/
	{
		IRQ_Adc();
	}

	#endif

}

void adc_ts_int_init()
{
	SUBSRCPND = (1<<TC_INIT_BIT) | (1<<ADC_INIT_BIT);

	/*注册中断初始化*/
	register_irq(31,AdcTsIntHandle);

	/*使能中断*/
	INTSUBMSK &= ~( (1<<ADC_INIT_BIT) | (1<<TC_INIT_BIT));/*0是service available*/
	//INTMSK &= ~(1<<INT_ADC); register_irq这个函数已经帮助设置了
	
}


void adc_ts_reg_init()
{
	/*	[13:6]: A/D converter prescaler value   	A/D converter freq. = 50MHz/(49+1) = 1MHz 
	 *  [5:3] : 000 = AIN 0 
	 *  [2]   : Standby mode select 
	 */
	ADCCON = (1<<14) | (49<<6) | (0<<3);

	ADCDLY = 0xff;

}

void enter_wait_pen_down_mode()/*进入等待中断*/
{
	ADCTSC = WAIT_PEN_DOWN | PULL_UP_ENABLE | YM_ENABLE | YP_DISENABLE | XP_DISENABLE | XM_DISENABLE | WAIT_INT_MODE;
}
void enter_wait_pen_up_mode()/*进入等待中断*/
{
	ADCTSC = WAIT_PEN_UP | PULL_UP_ENABLE | YM_ENABLE | YP_DISENABLE | XP_DISENABLE | XM_DISENABLE | WAIT_INT_MODE;
}

void touchscreen_init()
{
	/*设置中断*/	
	adc_ts_reg_init();

	/*设置触摸屏接口:寄存器*/
	adc_ts_int_init();

	/*让触摸屏控制器进入"等待中断模式"*/
	enter_wait_pen_down_mode();

}





