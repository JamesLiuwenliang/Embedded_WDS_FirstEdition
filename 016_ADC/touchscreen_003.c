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
void ISR_Tc()/*触摸屏中断,如果中断状态是按下,就触发ADC中断*/
{
	/*ADCUPDN */
	if(ADCDAT0 & (1<<15) )
	{
		printf("pen up \r\n");
		enter_wait_pen_down_mode();
	}
	else  /*测量位置*/
	{ 
		printf("pen down \r\n");

		
		/*进入"自动测量模式"*/
		enter_auto_measure_mode();

		/*启动ADC*/
		ADCCON |= (1<<0);/*跳入AdcTsIntHandle*/
	}

	

}

void ISR_Adc()/*ADC中断*/
{
	int x = ADCDAT0 ;
	int y = ADCDAT1 ;


	if(!(x & (1<<15))) /*如果仍然按下再打印*/
	{
		x &= 0x3ff;
		y &= 0x3ff;
		printf("x = %08d, y = %08d\n\r", x, y);

	}

	enter_wait_pen_up_mode(); /*清除中断标志位*/



}

void AdcTsIntHandle(int irq )
{
	if (SUBSRCPND & (1<<TC_INIT_BIT) ) /*如果是触摸屏中断*/
	{
		ISR_Tc();
	}

	SUBSRCPND = (1<<TC_INIT_BIT) | (1<<ADC_INIT_BIT) ;

 	if(SUBSRCPND & (1<<ADC_INIT_BIT)  )/*如果是ADC中断*/
	{
		ISR_Adc();
	}

	SUBSRCPND = (1<<TC_INIT_BIT) | (1<<ADC_INIT_BIT);

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

	/*按下触摸屏,延时一会在发出中断
	 * 延时时间 = ADCDELAY * 晶振周期 = ADCDLY * 1 /12M = 5ms
	 */
	ADCDLY = 60000;

}

void enter_wait_pen_down_mode()/*进入等待中断*/
{
	ADCTSC = WAIT_PEN_DOWN | PULL_UP_ENABLE | YM_ENABLE | YP_DISENABLE | XP_DISENABLE | XM_DISENABLE | WAIT_INT_MODE;
}
void enter_wait_pen_up_mode()/*进入等待中断*/
{
	ADCTSC = WAIT_PEN_UP | PULL_UP_ENABLE | YM_ENABLE | YP_DISENABLE | XP_DISENABLE | XM_DISENABLE | WAIT_INT_MODE;
}
void enter_auto_measure_mode()/*进入等待中断*/
{
	ADCTSC = AUTO_PST| NO_OPR_MODE ;
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





