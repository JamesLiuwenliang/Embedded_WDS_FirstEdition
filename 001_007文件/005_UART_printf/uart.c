
#include "s3c2440_soc.h"
#include "uart.h"

/*115200 8n1*/
void uart0_init()
{


	/*设置引脚用于串口*/
	/*GPH2,3用于TxD0,RxD0*/
	GPHCON &= ~((3<<4) | (3<<6));
	GPHCON |= ~((2<<4) | (2<<6));

	GPHUP &= ~((1<<2) | (1<<3));/*使能内部上拉*/

	/*设置波特率*/
	/* UBRDIVn  = (int)( UART clock / ( buad rate * 16) ) –1  
	 * UART clock = 50M
	 * UBRDIV0 = (int)( 50000000 / (115200 * 16) - 1)=26
	 */
	UCON0 =  0x00000005; /*使用PCLK 时钟,中断模式*/
	UBRDIV0 = 26;
	

	/*设置数据格式*/
	ULCON0 = 0x00000003; /*8n1 : 8个数据位 没有校验位 1个停止位*/


	
	


}
int putchar(int c)
{
	/* UTRSTAT0 */
	/* UTXH0 */

	while(! (UTRSTAT0 & (1<<2))); /*循环等待*/
	UTXH0 = (unsigned char)c; 

}

int getchar(void)
{
	/* read URXH0 */
	/* URXH0 */

	while(! (UTRSTAT0 & (1<<0))); /*循环等待*/
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





