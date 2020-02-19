
#ifndef _TOUCHSCREEN_H
#define _TOUCHSCREEN_H
void touchscreen_init();
void enter_wait_pen_down_mode();
void enter_wait_pen_up_mode();
void adc_ts_reg_init();
void adc_ts_int_init();
void AdcTsIntHandle(int irq);
void IRQ_Adc();
void ISR_Tc();



#endif


