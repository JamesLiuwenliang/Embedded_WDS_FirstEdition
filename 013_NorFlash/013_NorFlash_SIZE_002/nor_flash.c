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
void do_scan_nor_flash()
{
	char str[4];
	unsigned int size;
	
	/*��ӡ����ID,�豸ID*/
	nor_cmd(0x55,0x98); /*����CFIģʽ*/
	
	str[0] = nor_data(0x10);
	str[1] = nor_data(0x11);
	str[2] = nor_data(0x12);
	str[3] = '\0';	

	printf("str = %s \n\r",str);

	/*��ӡ����*/
	size = 1<<(nor_data(0x27)); /*0x27��ַ���������*/
	printf("nor size = 0x%x,%dM\r\n",size ,size/1024/1024);
	

	/*��ӡ������������ʼ��ַ*/


	/*����CFIģʽ*/
	nor_cmd(0x0,0xf0); /*�˳�CFIģʽ*/


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





