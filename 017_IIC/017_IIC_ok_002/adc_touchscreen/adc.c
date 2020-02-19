#include "../s3c2440_soc.h"
#include "adc.h"
void adc_init()
{
	/*	[13:6]: A/D converter prescaler value   	A/D converter freq. = 50MHz/(49+1) = 1MHz 
	 *  [5:3] : 000 = AIN 0 
	 *  [2]   : Standby mode select 
	 */
	ADCCON = (1<<14) | (49<<6) | (0<<3);

	ADCDLY = 0xff;
}

int adc_read_ain0(void)
{
	/*Æô¶¯ADC*/
	ADCCON |= (1<<0);

	while( !(ADCCON & (1<<15))); /*µÈ´ýADC½áÊø*/

	return ADCDAT0 & 0x3ff ;

}








