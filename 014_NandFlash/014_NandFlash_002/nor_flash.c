#include "s3c2440_soc.h"
#include "string_utils.h"
#include "my_printf.h"
#include "uart.h"

#define NOR_FLASH_BASE 0  /* jz2440 ,nor-->cs0 ,base addr = 0*/

/*CPU 发出Addr ,Nor Flash看到的是 Addr>>1 */

/* 比如: 55H 98
 * 举例 	本意是: 往 (0 + (0x55)<<1) 写入0x98
 */
void nor_write_word(unsigned int base,unsigned int offset ,unsigned int val )
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
	 /*这里需要在makefile里面做优化 要加入-march=armv4 因为函数是一个int型变量,他会去分两次传输,而我们希望一次传完只用最低两位*/
	 return *p; 
}

unsigned int nor_data(unsigned int offset )
{
	return nor_read_word(NOR_FLASH_BASE ,offset);
}
void wait_ready(unsigned int addr)
{
	unsigned int val ;
	unsigned int pre ;

	pre = nor_data(addr >> 1);
	val = nor_data(addr >> 1);

	while( (val & (1<<6))  != (pre & (1<<6)) )
	{
		pre = val;
		val = nor_data(addr >> 1);
	}



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


	int vendor,device;
	
	
	/*打印厂家ID,设备ID*/

	//解锁:必须关闭定时器,因为定时器中断启动后,会去Nor Flash里面读取数据,这也就是为什么不断在串口输入终会死机
	nor_cmd(0x555,0xaa); /*这些都是word操作,不是byte操作*/
	nor_cmd(0x2aa,0x55);
	nor_cmd(0x555,0x90);	/*read id*/
	vendor = nor_data(0);
	device = nor_data(1);
	nor_cmd(0,0xf0);		/*reset*/

	
	
	nor_cmd(0x55,0x98); /*进入CFI模式*/


	str[0] = nor_data(0x10);
	str[1] = nor_data(0x11);
	str[2] = nor_data(0x12);
	str[3] = '\0';	

	printf("str = %s \n\r",str);//QRY

	/*打印容量*/
	size = 1<<(nor_data(0x27)); /*0x27地址处存放容量*/
	printf("vendor id = 0x%x ,device id = 0x%x ,nor size = 0x%x,%dM\r\n",vendor,device,size ,size/1024/1024);


	/* 打印各个扇区的起始地址 */
	/* 名词解释:
	 *    erase block region : 里面含有1个或多个block, 它们的大小一样
	 * 一个nor flash含有1个或多个region
	 * 一个region含有1个或多个block(扇区)

	 * Erase block region information:
	 *    前2字节+1    : 表示该region有多少个block 
	 *    后2字节*256  : 表示block的大小
	 */

	regions = nor_data(0x2c);
	region_info_base = 0x2d;
	block_addr = 0 ;
	printf("Block/Sector start Address : \r\n");
	

	
	for(i=0;i<regions;i++)
	{	
		blocks = nor_data(region_info_base) + (nor_data(region_info_base + 1)<<8 )+ 1;
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
			if (cnt % 5 == 0)
				printf("\n\r");
		}
	}
	printf("\n\r");
	/* 退出CFI模式 */
	nor_cmd(0, 0xf0);
}

void do_erase_nor_flash()
{
	unsigned int addr;
	
	unsigned char str[16];
	/* 获得地址 */
	printf("Enter the Addr of Select to Erase: ");

	addr = get_uint ();
	printf("Erasing......\r\n");
	nor_cmd(0x555,0xaa);  /*解锁*/
	nor_cmd(0x2aa,0x55);
	nor_cmd(0x555,0x80);   /*Erase*/
	
	nor_cmd(0x555,0xaa);  /*解锁*/
	nor_cmd(0x2aa,0x55);
	nor_cmd(addr>>1,0x30);   
	/*发出扇区地址 ,这里要右移一位的原因是CPU发出的地址和Nor Flash的地址不是一样的 ,差了一位*/

	wait_ready(addr);



}
void do_read_nor_flash()
{
	unsigned int addr;
	volatile unsigned char *p;
	int i,j;
	unsigned char c;
	unsigned char str[16];
	/* 获得地址 */
	printf("Enter the Addr to Read : ");

	addr = get_uint ();
	
	p = (volatile unsigned char *)addr;
	printf("Data: \r\n");
	/*长度固定为64*/
	for(i=0;i<4;i++)
	{
		/*每行打印16个字符*/
		for(j=0;j<16;j++)
		{
		
			/*先打印数值*/
			c = *p++;
			str[j] = c;
			printf("%02x ",c);/*02x的意思是不论c是什么都打印2位*/
		}
		printf("\t; ");
		
		for(j=0;j<16;j++)
		{
			/*先打印数值,后打印字符*/
			if( (str[j] < 0x7E) && (str[j]>0x20))
			{
				putchar(str[j]);

			}
			else 
			{
				putchar('.');
			}

		}
		printf("\r\n");

	}



}


void do_write_nor_flash()
{	
	unsigned int addr;
	unsigned char str[100];
	int i,j;
	unsigned int value;

	printf("Enter the Addr of Select to Write: ");
	addr = get_uint ();

	printf("Enter the String to Write: ");
	gets(str);

	printf("Writing ......\r\n");

	/*str[0],str[1]  ==> 16bit数据*/
	i = 0;
	j = 1;
	while(str[i] && str[j])
	{
		value = str[i] + (str[j]<<8);
		/*烧写*/
		nor_cmd(0x555,0xaa);  /*解锁*/
		nor_cmd(0x2aa,0x55);
		nor_cmd(0x555,0xa0);  /*program*/
		nor_cmd(addr>>1,value);/*nor_cmd的第一个参数是占在Nor Flash 上看的地址*/

		/*等待烧写完成:读数据, Q6无变化时表示结束  */
		wait_ready(addr);
		
		i += 2;
		j += 2;
		addr += 2;
	}

	/* 以防出现如下情况:
	 * str[i] != 0 ,str[j] == 0 
	 * val  = str[i] + (str[j]<<8);
	 */
	value = str[i];
	/*烧写*/
	nor_cmd(0x555,0xaa);  /*解锁*/
	nor_cmd(0x2aa,0x55);
	nor_cmd(0x555,0xa0);  /*program*/
	nor_cmd(addr>>1,value);/*nor_cmd的第一个参数是占在Nor Flash 上看的地址*/

	/*等待烧写完成:读数据, Q6无变化时表示结束  */
	wait_ready(addr);


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





