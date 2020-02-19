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

//ԭ����char ����volatile
static int g_ts_data_valid = 0;/*charһ���Ծͻᱻ�����,����ʹ��volatile��Ŀ����ֱ�Ӳ����ڴ�,���ⱻ�Ż�*/


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

		/* ����"�Զ�����"ģʽ */
		enter_auto_measure_mode();

		/* ����ADC */
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

void ts_read_raw(int *px,int *py,int *ppressure)/*��¼����������*/
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

/*�������ÿ��10ms�����һ��*/
void touchscreen_timer_irq()
{
	int x = ADCDAT0;
	int y = ADCDAT1;

	/*����������Ա�����,����ADC,�����Զ�����ģʽ*/
	if(get_status_of_ts_timer()==0)
		return ;

	if (x & (1<<15))  /*�ɿ�*/
	{
		ts_timer_disable();
		enter_wait_pen_down_mode();
		//report_ts_xy(0,0,0);  /*ע�͵���*/
		return ;
	}
	else /*����״̬*/
	{
		/* ����"�Զ�����"ģʽ */
		enter_auto_measure_mode();

		/* ����ADC */
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


	if (!(x & (1<<15))) /* �����Ȼ���²Ŵ�ӡ */
	{
		/*��1������ADC���:
		 * a. Ҫ��������n��,�ٻ��n������,��ƽ��ֵ���ϱ�
		 * b. �õ�N�����ݺ�,������Timer
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

			/*������ʱ�����ٴζ�ȡ����*/
			ts_timer_enable();/*��־λ,��϶�ʱ��*/
		}
		else 
		{
			/*�ٴ�����ADC*/
			
			/* ����"�Զ�����"ģʽ */
			enter_auto_measure_mode();
			
			/* ����ADC */
			ADCCON |= (1<<0);	
			
		}
#if 0
		/*���ַ�ʽ��ȱ��,����ɿ���һ�ο��ܻ���һ�δ�������ݶ�ǰ������ݱ��ӵ�,�����������Ĳ�������ȫ�����*/
		x &= 0x3ff;
		y &= 0x3ff;

		report_ts_xy(&x,&y,1);

		/*������ʱ�����ٴζ�ȡ����*/
		ts_timer_enable();/*��־λ,��϶�ʱ��*/
#endif
	}
	else
	{
		adc_cnt = 0;
		adc_x  = 0;
		adc_y = 0;
	
		ts_timer_disable();
		enter_wait_pen_down_mode();
		report_ts_xy(0,0,0);/*������1*/
	}

	enter_wait_pen_up_mode();
}

void AdcTsIntHandle(int irq)
{
	if (SUBSRCPND & (1<<TC_INT_BIT))  /* ����Ǵ������ж� */
		Isr_Tc();

	if (SUBSRCPND & (1<<ADC_INT_BIT))  /* ADC�ж� */
		Isr_Adc();
	SUBSRCPND = (1<<TC_INT_BIT) | (1<<ADC_INT_BIT);
}

void adc_ts_int_init(void)
{
	SUBSRCPND = (1<<TC_INT_BIT) | (1<<ADC_INT_BIT);

	/* ע���жϴ������� */
	register_irq(31, AdcTsIntHandle);	

	/* ʹ���ж� */
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

	/*  ���´�����, ��ʱһ���ٷ���TC�ж�
	 *  ��ʱʱ�� = ADCDLY * �������� = ADCDLY * 1 / 12000000 = 5ms
	 */
	ADCDLY = 60000;	
}


void touchscreen_init(void)
{
	/* ���ô������ӿ�:�Ĵ��� */
	adc_ts_reg_init();

	printf("ADCUPDN = 0x%x, SUBSRCPND = 0x%x, SRCPND = 0x%x\n\r", ADCUPDN, SUBSRCPND, SRCPND);

	/* �����ж� */
	adc_ts_int_init();

	/*ע�ᶨʱ����������*/
	register_timer("timerscreen",touchscreen_timer_irq);

	/* �ô���������������"�ȴ��ж�ģʽ" */
	enter_wait_pen_down_mode();
}

