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

//原来是char 还有volatile
static int g_ts_data_valid = 0;/*char一次性就会被拆分完,这里使用volatile的目的是直接操作内存,避免被优化*/


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

		/* 进入"自动测量"模式 */
		enter_auto_measure_mode();

		/* 启动ADC */
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

void ts_read_raw(int *px,int *py,int *ppressure)/*记录触摸屏坐标*/
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
 int is_in_auto_mode()
{
	return ADCTSC & AUTO_PST;
}

/*这个函数每隔10ms会调用一次*/
void touchscreen_timer_irq()
{
	int y = ADCDAT1;

	/*如果触摸屏仍被按下,启动ADC,进入自动测量模式*/
	if(get_status_of_ts_timer()==0)
		return ;


	if (is_in_auto_mode ())
		return;

	/* 只有在等待中断模式下,才可以使用ADCDAT0bit'15来测量,
	 *原文: Up or Down state of stylus at waiting for interrupt mode. 
	 */
	if (ADCDAT0 & (1<<15))  /*松开*/
	{
		ts_timer_disable();
		enter_wait_pen_down_mode();
		report_ts_xy(0,0,0);  /*上报当前值*/
		return ;
	}
	else /*按下状态*/
	{
		/* 进入"自动测量"模式 */
		enter_auto_measure_mode();

		/* 启动ADC */
		ADCCON |= (1<<0);
	}


}
void Isr_Adc(void)
{
	

	
	static int adc_cnt = 0;
	static int adc_x = 0 ;
	static int adc_y = 0;

	enter_wait_pen_up_mode();


	if (!(ADCDAT0 & (1<<15))) /* 如果仍然按下才打印 */
	{
		/*第1次启动ADC后�:
		 * a. 要连续启动n次,再获得n个数据,求平均值再上报
		 * b. 得到N次数据后,再启动Timer
		 */
		adc_cnt ++;
		adc_x += ( ADCDAT0 & 0x3ff );
		adc_y += ( ADCDAT1 & 0x3ff );	

		if(adc_cnt == 16)
		{
			adc_x >> 4;
			adc_y >> 4;

			report_ts_xy(adc_x,adc_y,1);

			adc_cnt = 0;
			adc_x = 0;
			adc_y = 0;

			/*启动定时器已再次读取数据*/
			/*先设置TS进入'等待中断模式'*/
			enter_wait_pen_up_mode();
			ts_timer_enable();
		}
		else 
		{
			/*再次启动ADC*/
			/* 进入"自动测量"模式 */
			enter_auto_measure_mode();
			
			/* 启动ADC */
			ADCCON |= (1<<0);	
			
		}
#if 0
		/*这种方式有缺陷,最后松开的一次可能会是一次错误的数据而前面的数据被扔掉,所以最后算出的参数是完全错误的*/
		x &= 0x3ff;
		y &= 0x3ff;

		report_ts_xy(&x,&y,1);

		/*启动定时器已再次读取数据*/
		ts_timer_enable();/*标志位,配合定时器*/
#endif
	}
	else
	{
		adc_cnt = 0;
		adc_x  = 0;
		adc_y = 0;
	
		ts_timer_disable();
		enter_wait_pen_down_mode();
		report_ts_xy(0,0,0); 
	}

	//enter_wait_pen_up_mode();//这里多次一举,启动ADC时不能进入这个模式,会影响数据
}

void AdcTsIntHandle(int irq)
{
	if (SUBSRCPND & (1<<TC_INT_BIT))  /* 如果是触摸屏中断 */
		Isr_Tc();

	if (SUBSRCPND & (1<<ADC_INT_BIT))  /* ADC中断 */
		Isr_Adc();
	SUBSRCPND = (1<<TC_INT_BIT) | (1<<ADC_INT_BIT);
}

void adc_ts_int_init(void)
{
	SUBSRCPND = (1<<TC_INT_BIT) | (1<<ADC_INT_BIT);

	/* 注册中断处理函数 */
	register_irq(31, AdcTsIntHandle);	

	/* 使能中断 */
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

	/*  按下触摸屏, 延时一会再发出TC中断
	 *  延时时间 = ADCDLY * 晶振周期 = ADCDLY * 1 / 12000000 = 5ms
	 */
	ADCDLY = 60000;	
}


void touchscreen_init(void)
{
	/* 设置触摸屏接口:寄存器 */
	adc_ts_reg_init();

	printf("ADCUPDN = 0x%x, SUBSRCPND = 0x%x, SRCPND = 0x%x\n\r", ADCUPDN, SUBSRCPND, SRCPND);

	/* 设置中断 */
	adc_ts_int_init();

	/*注册定时器处理函数*/
	register_timer("timerscreen",touchscreen_timer_irq);

	/* 让触摸屏控制器进入"等待中断模式" */
	enter_wait_pen_down_mode();
}


