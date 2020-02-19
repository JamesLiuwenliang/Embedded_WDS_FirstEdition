
#include "s3c2440_soc.h"
#include "string_utils.h"
#include "my_printf.h"
#include "uart.h"
#include "nand_flash.h"


#define TACLS 0
#define TWRPH0 1
#define TWRPH1 0


void nand_flash_init(void)
{
	/*设置Nand Flash 的时序*/
	NFCONF = ((TACLS<<12) | (TWRPH0<<10) | (TWRPH1<<4) );
	/*使能Nand  Flash 控制器,初始化ECC,禁止片选*/
	NFCONT = ((1<<0)| (1<<1) | (1<<4));

}
void nand_deselect(void)
{
	/*禁止片选*/
	NFCONT |= (1<<1);

}

void nand_select(void)
{	
	/*使能片选*/
	NFCONT &=  ~(1<<1);

}
void nand_cmd(unsigned char cmd)
{
	volatile int i;

	NFCMD = cmd;
	for(i=0;i<10;i++);/*延时为了保证数据信号的稳定*/
}
void nand_addr_byte(unsigned char addr)
{
	volatile int i;
	NFADDR = addr;
	for(i=0;i<10;i++);
}
unsigned char  nand_data(void)
{
	return NFDATA;
}

void nand_chipid()
{
	int i=0;
	unsigned char buf[5]={0};
	
	nand_select();
	nand_cmd(0x90); 
	nand_addr_byte(0x00);
	for(i=0;i<5;i++)
	{
		buf[i] = nand_data();
	}
	nand_deselect();
	
	printf("maker ID  : 0x%x\r\n",buf[0]);
	printf("device ID : 0x%x\r\n",buf[1]);
	printf("3rd byte  : 0x%x\r\n",buf[2]);
    printf("4th       : 0x%x\r\n",buf[3]);
    printf("5th       : 0x%x\r\n",buf[4]);
  

}


void nand_flash_test()
{
	char c;
	
	while(1)
	{
      	/*打印菜单,供我们选择测试内容*/
      	printf("[s] Scan nand flash\r\n");
      	printf("[e] Erase nand flash\r\n");
      	printf("[w] Write nand flash\r\n");
      	printf("[r] Read nand flash\r\n");
      	printf("[q] Quite\r\n");
      	printf("Enter selection\r\n");

      
      	c=getchar();
      	
      	printf("%c\r\n",c);

      	switch(c)
      	{
			case 'q':
			case 'Q':
				return ;
				break;
				
			case 's':
			case 'S':

				nand_chipid();
				break;
				
			case 'E':
			case 'e':
				break;

				
			case 'W':
			case 'w':
				break;
				

				
			case 'R':
			case 'r':
				break;

			default:
				break;
				

		}
	}


	

}



