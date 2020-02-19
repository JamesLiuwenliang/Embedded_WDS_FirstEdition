
#include "s3c2440_soc.h"
#include "uart.h"

/*115200 8n1*/
void uart0_init()
{


	/*�����������ڴ���*/
	/*GPH2,3����TxD0,RxD0*/
	GPHCON &= ~((3<<4) | (3<<6));
	GPHCON |= ~((2<<4) | (2<<6));

	GPHUP &= ~((1<<2) | (1<<3));/*ʹ���ڲ�����*/

	/*���ò�����*/
	/* UBRDIVn  = (int)( UART clock / ( buad rate * 16) ) �C1  
	 * UART clock = 50M
	 * UBRDIV0 = (int)( 50000000 / (115200 * 16) - 1)=26
	 */
	UCON0 =  0x00000005; /*ʹ��PCLK ʱ��,�ж�ģʽ*/
	UBRDIV0 = 26;
	

	/*�������ݸ�ʽ*/
	ULCON0 = 0x00000003; /*8n1 : 8������λ û��У��λ 1��ֹͣλ*/


	
	


}
int putchar(int c)
{
	/* UTRSTAT0 */
	/* UTXH0 */

	while(! (UTRSTAT0 & (1<<2))); /*ѭ���ȴ�*/
	UTXH0 = (unsigned char)c; 

}

int getchar(void)
{
	/* read URXH0 */
	/* URXH0 */

	while(! (UTRSTAT0 & (1<<0))); /*ѭ���ȴ�*/
	return URXH0 ; 

}
int puts(const char *s)
{
	while(*s)
	{
		putchar(*s);
		s++;
	}


}





