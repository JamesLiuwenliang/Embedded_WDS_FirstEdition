#include "s3c2440_soc.h"
#include "string_utils.h"
#include "my_printf.h"
#include "uart.h"
#include "nand_flash.h"




/* Nand Flash 上面有OOB
 *
 */
#define TACLS 0
#define TWRPH0 1
#define TWRPH1 0

void wait_ready ()
{
	while(!(NFSTAT & 1));/*RnB (Read-only) 判断nand flash是否busy*/
}
unsigned char nand_data()
{
		return NFDATA;
}
unsigned char nand_write_data(unsigned char val)
{
	NFDATA = val;
}


void nand_flash_init(void)
{
	/*设置Nand Flash 的时序*/
	NFCONF = ((TACLS<<12) | (TWRPH0<<8) | (TWRPH1<<4) );
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


void nand_chipid()
{

	unsigned char buf[5]={0};
	int i;
	nand_select();

	/*4.14 
	 * 因为有Nand Flash 控制器 只需要关注IOx的命令即可
	 * 上面的才做全部由Nand 控制器完成
	 */
	nand_cmd(0x90); 
	nand_addr_byte(0x00);
	for(i=0;i<5;i++)
	{
		buf[i] = nand_data();/*返回5个字节数据:Maker Code, Device Code,3rd,4th,5th*/
	}
	nand_deselect();
	
	printf("maker ID  : 0x%x\r\n",buf[0]);
	printf("device ID : 0x%x\r\n",buf[1]);
	printf("3rd byte  : 0x%x\r\n",buf[2]);
    printf("4th       : 0x%x\r\n",buf[3]);
    printf("5th       : 0x%x\r\n",buf[4]);
  

}
void nand_read(unsigned int addr , unsigned char *buf ,unsigned int len)
{
	/*5.1 Page Read 时序图 ,同样只需关注IO口的操作*/
	/*	Q : 有一个问题,当CPU想访问地址2048是访问page1的col 0 ,还是 page0的OOB第0位(也可以认为是Page0的col2048)
	 *  A : 是访问Page1的col 0.
	 */
	int i=0;
	int page = addr / 2048;
	int col = addr & (2048 - 1);/*0-2047*/

	nand_select();

	while( i <len)
	{
    	/*发出00h命令*/
    	nand_cmd(00);

		/* 发出地址(5 cycle :执行5次循环Col. Add.1,2 & Row Add.1,2,3) */

		/*每次NandFlash控制器只能看到最低8位的IOx操作情况 
		 *甚至比如传输col时,第2次只能看到4个IO的情况,因为col只有2048个大小
		 */
		 
    	/* col addr 依次发出低8位,再发出高8位 */
    	nand_addr_byte(col & 0xFF);
		nand_addr_byte((col >> 8) & 0xff );

		/* row/page addr */
    	nand_addr_byte(page & 0xFF);
		nand_addr_byte((page >> 8) & 0xff );//只关心最低8位
		nand_addr_byte((page >> 16) & 0xff );//只关心最低8位

		
    	/*发出30h命令*/
    	nand_cmd(0x30);

		wait_ready ();/*等待 R/B 的是等待时间*/

		/* 读数据 */

		/*执行一次for循环是读1page*/
		for( ; (col <2048) & (i < len) ; col++) /*col一定小于2048,因为一个page才2048列*/
		{
			buf [i++] = nand_data();
			
		}
		if(i ==  len) break;
		col = 0;
		page ++;
		
	}

	
	nand_deselect();

}

int nand_erase(unsigned int addr,  unsigned int len) /*len哪怕只传入1,他依然会自然地擦除块(128k)*/
{
	
	int page ;

	page = addr / 2048;

	if(addr & (0x1ffff) )/*查看addr是不是在128k的整数倍上*/ 
	{
		printf("nand_erase err, addr is not block align...\r\n");
		return -1;

	}
	
	/*查看len的大小是不是128k的整数倍,
	 * 如果不是的话,因为操作erase还是会按128k删除,
	 * 所以最好是128k的整数倍,不然可能会误删*/
	if(len & (0x1ffff) ) 
	{
		printf("nand_erase err,  len is not block align...\r\n");
		return -1;
	}

	printf("\r\n");
	
	nand_select();

	

	while(1)
	{
		nand_cmd(0x60);

		/*erase操作是不需要发出col地址的,只需要给出是第几page,
		 *因为erase是按照128k操作的,指定列数也没什么意义
		 */
		/* row(page) addr*/
		nand_addr_byte(page & 0xff);
		nand_addr_byte( (page>>8) & 0xff);
		nand_addr_byte( (page>>16) & 0xff);

		nand_cmd(0xD0);

		wait_ready();

		len -= (128*1024);

		if(len == 0)
		{
			break;
		}

		addr += (128*1024);/*指向下一个block*/
	}

	nand_deselect();
	return 0;


}
void nand_write(unsigned int addr , unsigned char *buf,unsigned int len )
{
	int page = addr / 2048;
	int col = addr & (2048-1);
	int i = 0;
	

	nand_select();

	while(1)
	{
		nand_cmd(0x80);

		/*发出地址*/
		/* col addr */
		nand_addr_byte(col & 0xFF);
		nand_addr_byte((col >> 8) & 0xff );
			
		/* row/page addr */
		nand_addr_byte(page & 0xFF);
		nand_addr_byte((page >> 8) & 0xff );//只关心最低8位
		nand_addr_byte((page >> 16) & 0xff );//只关心最低8位


		/*发出一页的数据*/
		for(i=0; (col < 2048) & (i<len) ;i++)
		{
			nand_write_data (buf[i]); 
	
		}
		nand_cmd(0x10);
		wait_ready();

		if(i==len)
		{
			break;
		}
		else
		{
			/*开始下一个循环page*/
			col = 0;
			page++;
		}
		
		
	}




	nand_deselect();


}

void do_read_nand_flash(void)
{
	unsigned int addr;
	volatile unsigned char *p;
	int i, j;
	unsigned char c;
	unsigned char str[16];
	unsigned char buf[64];
	
	/* 获得地址 */
	printf("Enter the address to read: ");
	addr = get_uint();

	nand_read(addr, buf, 64);
	p = (volatile unsigned char *)buf;

	printf("Data : \n\r");
	/* 长度固定为64 */
	for (i = 0; i < 4; i++)
	{
		/* 每行打印16个数据 */
		for (j = 0; j < 16; j++)
		{
			/* 先打印数值 */
			c = *p++;
			str[j] = c;
			printf("%02x ", c);
		}

		printf("   ; ");

		for (j = 0; j < 16; j++)
		{
			/* 后打印字符 */
			if (str[j] < 0x20 || str[j] > 0x7e)  /* 不可视字符 */
				putchar('.');
			else
				putchar(str[j]);
		}
		printf("\n\r");
	}
}

void do_erase_nand_flash(void)
{
	unsigned int addr;
	
	/* 获得地址 */
	printf("Enter the address of sector to erase: ");
	addr = get_uint();

	printf("erasing ...\n\r");
	nand_erase(addr, 128*1024);
}
void do_write_nand_flash(void) /*write nand flash 比*/
{
	unsigned int addr;
	unsigned char str[100];
	int i, j;
	unsigned int val;
	
	/* 获得地址 */
	printf("Enter the address of sector to write: ");
	addr = get_uint();

	printf("Enter the string to write: ");
	gets(str);

	printf("writing ...\n\r");
	nand_write(addr, str, strlen(str)+1);

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
				do_erase_nand_flash();
				break;

				
			case 'W':
			case 'w':
				do_write_nand_flash();
				break;
				

				
			case 'R':
			case 'r':
				do_read_nand_flash();
				break;

			default:
				break;
				

		}
	}


	

}



