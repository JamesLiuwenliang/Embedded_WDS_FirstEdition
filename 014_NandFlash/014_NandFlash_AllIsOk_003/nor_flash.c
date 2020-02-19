#include "s3c2440_soc.h"
#include "string_utils.h"
#include "my_printf.h"
#include "uart.h"

#define NOR_FLASH_BASE 0  /* jz2440 ,nor-->cs0 ,base addr = 0*/

/*CPU ����Addr ,Nor Flash�������� Addr>>1 */

/* ����: 55H 98
 * ���� 	������: �� (0 + (0x55)<<1) д��0x98
 */
void nor_write_word(unsigned int base,unsigned int offset ,unsigned int val )
{

	volatile unsigned short *p = (volatile unsigned short *)(base+(offset << 1)); /*(0 + (0x55)<<1) */
	*p = val; /*д0x98,ֻ������������ֽ�*/	
}

void nor_cmd(unsigned int offset , unsigned int cmd)
{
	nor_write_word(NOR_FLASH_BASE ,offset,cmd);

}
unsigned int  nor_read_word(unsigned int base,unsigned int offset  )
{
	 volatile unsigned short *p = (volatile unsigned short *)(base+(offset << 1)); 
	 /*������Ҫ��makefile�������Ż� Ҫ����-march=armv4 ��Ϊ������һ��int�ͱ���,����ȥ�����δ���,������ϣ��һ�δ���ֻ�������λ*/
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


/*����Nor Flash ��CFIģʽ
 *��ȡ������Ϣ
 */
void do_scan_nor_flash(void)
{
	char str[4];
	unsigned int size;
	int i=0, j=0;
	int regions; /*��0x24��λ�õõ��ж���region*/
	int blocks; /*block������*/
	int block_size;/*block�Ĵ�С*/
	int cnt=0; 

	int block_addr;
	int region_info_base ;


	int vendor,device;
	
	
	/*��ӡ����ID,�豸ID*/

	//����:����رն�ʱ��,��Ϊ��ʱ���ж�������,��ȥNor Flash�����ȡ����,��Ҳ����Ϊʲô�����ڴ��������ջ�����
	nor_cmd(0x555,0xaa); /*��Щ����word����,����byte����*/
	nor_cmd(0x2aa,0x55);
	nor_cmd(0x555,0x90);	/*read id*/
	vendor = nor_data(0);
	device = nor_data(1);
	nor_cmd(0,0xf0);		/*reset*/

	
	
	nor_cmd(0x55,0x98); /*����CFIģʽ*/


	str[0] = nor_data(0x10);
	str[1] = nor_data(0x11);
	str[2] = nor_data(0x12);
	str[3] = '\0';	

	printf("str = %s \n\r",str);//QRY

	/*��ӡ����*/
	size = 1<<(nor_data(0x27)); /*0x27��ַ���������*/
	printf("vendor id = 0x%x ,device id = 0x%x ,nor size = 0x%x,%dM\r\n",vendor,device,size ,size/1024/1024);


	/* ��ӡ������������ʼ��ַ */
	/* ���ʽ���:
	 *    erase block region : ���溬��1������block, ���ǵĴ�Сһ��
	 * һ��nor flash����1������region
	 * һ��region����1������block(����)

	 * Erase block region information:
	 *    ǰ2�ֽ�+1    : ��ʾ��region�ж��ٸ�block 
	 *    ��2�ֽ�*256  : ��ʾblock�Ĵ�С
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
			/* ��ӡÿ��block����ʼ��ַ */
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
	/* �˳�CFIģʽ */
	nor_cmd(0, 0xf0);
}

void do_erase_nor_flash()
{
	unsigned int addr;
	
	unsigned char str[16];
	/* ��õ�ַ */
	printf("Enter the Addr of Select to Erase: ");

	addr = get_uint ();
	printf("Erasing......\r\n");
	nor_cmd(0x555,0xaa);  /*����*/
	nor_cmd(0x2aa,0x55);
	nor_cmd(0x555,0x80);   /*Erase*/
	
	nor_cmd(0x555,0xaa);  /*����*/
	nor_cmd(0x2aa,0x55);
	nor_cmd(addr>>1,0x30);   
	/*����������ַ ,����Ҫ����һλ��ԭ����CPU�����ĵ�ַ��Nor Flash�ĵ�ַ����һ���� ,����һλ*/

	wait_ready(addr);



}
void do_read_nor_flash()
{
	unsigned int addr;
	volatile unsigned char *p;
	int i,j;
	unsigned char c;
	unsigned char str[16];
	/* ��õ�ַ */
	printf("Enter the Addr to Read : ");

	addr = get_uint ();
	
	p = (volatile unsigned char *)addr;
	printf("Data: \r\n");
	/*���ȹ̶�Ϊ64*/
	for(i=0;i<4;i++)
	{
		/*ÿ�д�ӡ16���ַ�*/
		for(j=0;j<16;j++)
		{
		
			/*�ȴ�ӡ��ֵ*/
			c = *p++;
			str[j] = c;
			printf("%02x ",c);/*02x����˼�ǲ���c��ʲô����ӡ2λ*/
		}
		printf("\t; ");
		
		for(j=0;j<16;j++)
		{
			/*�ȴ�ӡ��ֵ,���ӡ�ַ�*/
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

	/*str[0],str[1]  ==> 16bit����*/
	i = 0;
	j = 1;
	while(str[i] && str[j])
	{
		value = str[i] + (str[j]<<8);
		/*��д*/
		nor_cmd(0x555,0xaa);  /*����*/
		nor_cmd(0x2aa,0x55);
		nor_cmd(0x555,0xa0);  /*program*/
		nor_cmd(addr>>1,value);/*nor_cmd�ĵ�һ��������ռ��Nor Flash �Ͽ��ĵ�ַ*/

		/*�ȴ���д���:������, Q6�ޱ仯ʱ��ʾ����  */
		wait_ready(addr);
		
		i += 2;
		j += 2;
		addr += 2;
	}

	/* �Է������������:
	 * str[i] != 0 ,str[j] == 0 
	 * val  = str[i] + (str[j]<<8);
	 */
	value = str[i];
	/*��д*/
	nor_cmd(0x555,0xaa);  /*����*/
	nor_cmd(0x2aa,0x55);
	nor_cmd(0x555,0xa0);  /*program*/
	nor_cmd(addr>>1,value);/*nor_cmd�ĵ�һ��������ռ��Nor Flash �Ͽ��ĵ�ַ*/

	/*�ȴ���д���:������, Q6�ޱ仯ʱ��ʾ����  */
	wait_ready(addr);


}

void nor_flash_test()
{
	char c;
	
	while(1)
	{
      	/*��ӡ�˵�,������ѡ���������*/
      	printf("[s] Scan nor flash\r\n");
      	printf("[e] Erase nor flash\r\n");
      	printf("[w] Write nor flash\r\n");
      	printf("[r] Read nor flash\r\n");
      	printf("[q] Quite\r\n");
      	printf("Enter selection\r\n");

      
      	c=getchar();
      	
      	printf("%c\r\n",c);
      	/* ��������:
      		1. ʶ��nor_flash
      		2. ����nor_flash ĳ������
      		3. ��дĳ����ַ
      		4. ��ĳ����ַ
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





