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
void do_scan_nor_flash(void)
{
	char str[4];
	unsigned int size;
	int i=0, j=0;
	int regions; /*从0x24的位置得到有多少region*/
	int blocks; /*block的数量*/
	int block_size;/*block的大小*/
	int cnt=0; 

	int block_addr;
	int region_info_base ;
	
	/*打印厂家ID,设备ID*/
	nor_cmd(0x55,0x98); /*进入CFI模式*/


	str[0] = nor_data(0x10);
	str[1] = nor_data(0x11);
	str[2] = nor_data(0x12);
	str[3] = '\0';	

	printf("str = %s \n\r",str);//QRY

	/*打印容量*/
	size = 1<<(nor_data(0x27)); /*0x27地址处存放容量,容量 = (2 ^返回的值)*/
	printf("nor size = 0x%x,%dM\r\n",size ,size/1024/1024);


	/* 打印各个扇区的起始地址 */
	/* 名词解释:
	 *    erase block region : 里面含有1个或多个block, 它们的大小一样
	 * 一个nor flash含有1个或多个region
	 * 一个region含有1个或多个block(扇区)

	 * Erase block region information:
	 *    前2字节+1    : 表示该region有多少个block 
	 *    后2字节*256  : 表示block的大小
	 */

	regions = nor_data(0x2c);/*这里应该获得的是4*/
	region_info_base = 0x2d;
	block_addr = 0 ;
	printf("Block/Sector start Address : \r\n");
	

	
	for(i=0;i<regions;i++)
	{	
		/* 0x2d - 0x30 显示的是bank area 1  1  64*256
		 * 0x31 - 0x34 显示的是bank area 2	2  32*256
		 * 0x35 - 0x38 显示的是bank area 3	1 128*256
		 * 0x39 - 0x3c 显示的是bank area 4 30 256*256
		 */
		blocks = nor_data(region_info_base) + (nor_data(region_info_base + 1)<<8 )+ 1;/*block size*/
		block_size = 256 * (nor_data(region_info_base+2) + (nor_data(region_info_base+3)<<8));
		region_info_base += 4;
		

//		printf("\n\rregion %d, blocks = %d, block_size = 0x%x, block_addr = 0x%x\n\r", i, blocks, block_size, block_addr);

		for (j = 0; j < blocks; j++)
		{
			/* 打印每个block的起始地址 */
			//printf("0x%08x ", block_addr);
			printHex(block_addr);
			putchar(' ');
			cnt++;
			block_addr += block_size;
			//if (cnt % 5 == 0)
			//	printf("\n\r");
		}
		printf("\n\r");
	}
	printf("\n\r");
	/* 退出CFI模式 */
	nor_cmd(0, 0xf0);
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





