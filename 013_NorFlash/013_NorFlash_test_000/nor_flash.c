#include "s3c2440_soc.h"
#include "string_utils.h"
#include "my_printf.h"
#include "uart.h"

#define NOR_FLASH_BASE 0  /* jz2440 ,nor-->cs0 ,base addr = 0*/



/* ����: 55H 98
 * ���� 	������: �� (0 + (0x55)<<1) д��0x98
 */
void nor_write_word(unsigned int base,unsigned short offset ,unsigned int val )
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
	 return *p;
}

unsigned int nor_data(unsigned int offset )
{
	return nor_read_word(NOR_FLASH_BASE ,offset);
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
	
	/*��ӡ����ID,�豸ID*/
	nor_cmd(0x55,0x98); /*����CFIģʽ*/


	str[0] = nor_data(0x10);
	str[1] = nor_data(0x11);
	str[2] = nor_data(0x12);
	str[3] = '\0';	

	printf("str = %s \n\r",str);//QRY

	/*��ӡ����*/
	size = 1<<(nor_data(0x27)); /*0x27��ַ���������,���� = (2 ^���ص�ֵ)*/
	printf("nor size = 0x%x,%dM\r\n",size ,size/1024/1024);


	/* ��ӡ������������ʼ��ַ */
	/* ���ʽ���:
	 *    erase block region : ���溬��1������block, ���ǵĴ�Сһ��
	 * һ��nor flash����1������region
	 * һ��region����1������block(����)

	 * Erase block region information:
	 *    ǰ2�ֽ�+1    : ��ʾ��region�ж��ٸ�block 
	 *    ��2�ֽ�*256  : ��ʾblock�Ĵ�С
	 */

	regions = nor_data(0x2c);/*����Ӧ�û�õ���4*/
	region_info_base = 0x2d;
	block_addr = 0 ;
	printf("Block/Sector start Address : \r\n");
	

	
	for(i=0;i<regions;i++)
	{	
		/* 0x2d - 0x30 ��ʾ����bank area 1  1  64*256
		 * 0x31 - 0x34 ��ʾ����bank area 2	2  32*256
		 * 0x35 - 0x38 ��ʾ����bank area 3	1 128*256
		 * 0x39 - 0x3c ��ʾ����bank area 4 30 256*256
		 */
		blocks = nor_data(region_info_base) + (nor_data(region_info_base + 1)<<8 )+ 1;/*block size*/
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
			//if (cnt % 5 == 0)
			//	printf("\n\r");
		}
		printf("\n\r");
	}
	printf("\n\r");
	/* �˳�CFIģʽ */
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





