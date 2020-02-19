

#include "s3c2440_soc.h"
#include "uart.h"
#include "init.h"

char g_Char = 'A';
char g_Char3 = 'a';
const char g_Char2 = 'B';
int g_A = 0;
int g_B;

int main(void)
{
	led_init();
	//interrupt_init();  /* 初始化中断控制器 */
	key_eint_init();   /* 初始化按键, 设为中断源 */
	//timer_init();
	
	puts("Hello World... ");

	nand_flash_init();
	nand_flash_test();
	
	return 0;
}




