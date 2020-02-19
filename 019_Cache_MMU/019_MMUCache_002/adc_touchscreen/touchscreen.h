
#ifndef _TOUCHSCREEN_H
#define _TOUCHSCREEN_H
void enter_wait_pen_down_mode(void);
void enter_wait_pen_up_mode(void);
void enter_auto_measure_mode(void);
void Isr_Tc(void);
void report_ts_xy(int x,int y,int pressure );
void Isr_Adc(void);
void AdcTsIntHandle(int irq);
void adc_ts_int_init(void);
void adc_ts_reg_init(void);
void touchscreen_init(void);







#endif


