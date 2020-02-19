#include "s3c2440_soc.h"
#include "string_utils.h"
#include "my_printf.h"
#include "uart.h"

#define NOR_FLASH_BASE 0  /* jz2440 ,nor-->cs0 ,base addr = 0*/



/* 比如: 55H 98
 * 举例 	本意是: 往 (0 + (0x55)<<1) 写入0x98
 */
void nor_write_word(unsigned int base,unsigned short offset ,unsigned int val )
{
	volatile unsigned short *p = (volatile unsigned short *)(base+(offset << 1)); /*(0 + (0x55)<<1) */
	*p = val; /*写0x98,只会用最低两个字节*/	
}

void nor_cmd(unsigned int offset , unsigned int cmd)
{
	nor_write_word(NOR_FLASH_BASE ,offset,cmd);

}
unsigned int  nor_read_word(unsigned int base,unsigned int offset  )
{
	 volatile unsigned short *p = (volatile unsigned short *)(base+(offset << 1)); 
	 return *p;
}

unsigned int nor_data(unsigned int offset )
{
	return nor_read_word(NOR_FLASH_BASE ,offset);
}

/*进入Nor Flash 的CFI模式
 *读取各类信息
 */
void do_scan_nor_flash()
{
	char str[4];
	unsigned int size;
	
	/*打印厂家ID,设备ID*/
	nor_cmd(0x55,0x98); /*进入CFI模式*/
	
	str[0] = nor_data(0x10);
	str[1] = nor_data(0x11);
	str[2] = nor_data(0x12);
	str[3] = '\0';	

	printf("str = %s \n\r",str);

	/*打印容量*/
	size = 1<<(nor_data(0x27)); /*0x27地址处存放容量*/
	printf("nor size = 0x%x,%dM\r\n",size ,size/1024/1024);
	

	/*打印各个扇区的起始地址*/


	/*读出CFI模式*/
	nor_cmd(0x0,0xf0); /*退出CFI模式*/


}

void do_erase_nor_flash()
{

}
void do_read_nor_flash()
{

}

void do_write_nor_flash()
{

}

void nor_flash_test()
{
	char c;
	
	while(1)
	{
      	/*打印菜单,供我们选择测试内容*/
      	printf("[s] Scan nor flash\r\n");
      	printf("[e] Erase nor flash\r\n");
      	printf("[w] Write nor flash\r\n");
      	printf("[r] Read nor flash\r\n");
      	printf("[q] Quite\r\n");
      	printf("Enter selection\r\n");

      
      	c=getchar();
      	
      	printf("%c\r\n",c);
      	/* 测试内容:
      		1. 识别nor_flash
      		2. 擦除nor_flash 某个扇区
      		3. 编写某个地址
      		4. 读某个地址
      	*/
      	switch(c)
      	{
			case 'q':
			case 'Q':
				return ;
				break;
				
			case 's':
			case 'S':

				do_scan_nor_flash();
				break;
				
			case 'E':
			case 'e':
				do_erase_nor_flash();
				break;

				
			case 'W':
			case 'w':
				do_write_nor_flash();
				break;
				

				
			case 'R':
			case 'r':
				do_read_nor_flash();
				break;

			default:
				break;
				

		}
	}


	

}





