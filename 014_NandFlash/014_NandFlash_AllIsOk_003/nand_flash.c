#include "s3c2440_soc.h"
#include "string_utils.h"
#include "my_printf.h"
#include "uart.h"
#include "nand_flash.h"




/* Nand Flash ������OOB
 *
 */
#define TACLS 0
#define TWRPH0 1
#define TWRPH1 0

void wait_ready ()
{
	while(!(NFSTAT & 1));/*RnB (Read-only) �ж�nand flash�Ƿ�busy*/
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
	/*����Nand Flash ��ʱ��*/
	NFCONF = ((TACLS<<12) | (TWRPH0<<8) | (TWRPH1<<4) );
	/*ʹ��Nand  Flash ������,��ʼ��ECC,��ֹƬѡ*/
	NFCONT = ((1<<0)| (1<<1) | (1<<4));

}
void nand_deselect(void)
{
	/*��ֹƬѡ*/
	NFCONT |= (1<<1);

}

void nand_select(void)
{	
	/*ʹ��Ƭѡ*/
	NFCONT &=  ~(1<<1);

}
void nand_cmd(unsigned char cmd)
{
	volatile int i;

	NFCMD = cmd;
	for(i=0;i<10;i++);/*��ʱΪ�˱�֤�����źŵ��ȶ�*/
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
	 * ��Ϊ��Nand Flash ������ ֻ��Ҫ��עIOx�������
	 * ����Ĳ���ȫ����Nand ���������
	 */
	nand_cmd(0x90); 
	nand_addr_byte(0x00);
	for(i=0;i<5;i++)
	{
		buf[i] = nand_data();/*����5���ֽ�����:Maker Code, Device Code,3rd,4th,5th*/
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
	/*5.1 Page Read ʱ��ͼ ,ͬ��ֻ���עIO�ڵĲ���*/
	/*	Q : ��һ������,��CPU����ʵ�ַ2048�Ƿ���page1��col 0 ,���� page0��OOB��0λ(Ҳ������Ϊ��Page0��col2048)
	 *  A : �Ƿ���Page1��col 0.
	 */
	int i=0;
	int page = addr / 2048;
	int col = addr & (2048 - 1);/*0-2047*/

	nand_select();

	while( i <len)
	{
    	/*����00h����*/
    	nand_cmd(00);

		/* ������ַ(5 cycle :ִ��5��ѭ��Col. Add.1,2 & Row Add.1,2,3) */

		/*ÿ��NandFlash������ֻ�ܿ������8λ��IOx������� 
		 *�������紫��colʱ,��2��ֻ�ܿ���4��IO�����,��Ϊcolֻ��2048����С
		 */
		 
    	/* col addr ���η�����8λ,�ٷ�����8λ */
    	nand_addr_byte(col & 0xFF);
		nand_addr_byte((col >> 8) & 0xff );

		/* row/page addr */
    	nand_addr_byte(page & 0xFF);
		nand_addr_byte((page >> 8) & 0xff );//ֻ�������8λ
		nand_addr_byte((page >> 16) & 0xff );//ֻ�������8λ

		
    	/*����30h����*/
    	nand_cmd(0x30);

		wait_ready ();/*�ȴ� R/B ���ǵȴ�ʱ��*/

		/* ������ */

		/*ִ��һ��forѭ���Ƕ�1page*/
		for( ; (col <2048) & (i < len) ; col++) /*colһ��С��2048,��Ϊһ��page��2048��*/
		{
			buf [i++] = nand_data();
			
		}
		if(i ==  len) break;
		col = 0;
		page ++;
		
	}

	
	nand_deselect();

}

int nand_erase(unsigned int addr,  unsigned int len) /*len����ֻ����1,����Ȼ����Ȼ�ز�����(128k)*/
{
	
	int page ;

	page = addr / 2048;

	if(addr & (0x1ffff) )/*�鿴addr�ǲ�����128k����������*/ 
	{
		printf("nand_erase err, addr is not block align...\r\n");
		return -1;

	}
	
	/*�鿴len�Ĵ�С�ǲ���128k��������,
	 * ������ǵĻ�,��Ϊ����erase���ǻᰴ128kɾ��,
	 * ���������128k��������,��Ȼ���ܻ���ɾ*/
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

		/*erase�����ǲ���Ҫ����col��ַ��,ֻ��Ҫ�����ǵڼ�page,
		 *��Ϊerase�ǰ���128k������,ָ������Ҳûʲô����
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

		addr += (128*1024);/*ָ����һ��block*/
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

		/*������ַ*/
		/* col addr */
		nand_addr_byte(col & 0xFF);
		nand_addr_byte((col >> 8) & 0xff );
			
		/* row/page addr */
		nand_addr_byte(page & 0xFF);
		nand_addr_byte((page >> 8) & 0xff );//ֻ�������8λ
		nand_addr_byte((page >> 16) & 0xff );//ֻ�������8λ


		/*����һҳ������*/
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
			/*��ʼ��һ��ѭ��page*/
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
	
	/* ��õ�ַ */
	printf("Enter the address to read: ");
	addr = get_uint();

	nand_read(addr, buf, 64);
	p = (volatile unsigned char *)buf;

	printf("Data : \n\r");
	/* ���ȹ̶�Ϊ64 */
	for (i = 0; i < 4; i++)
	{
		/* ÿ�д�ӡ16������ */
		for (j = 0; j < 16; j++)
		{
			/* �ȴ�ӡ��ֵ */
			c = *p++;
			str[j] = c;
			printf("%02x ", c);
		}

		printf("   ; ");

		for (j = 0; j < 16; j++)
		{
			/* ���ӡ�ַ� */
			if (str[j] < 0x20 || str[j] > 0x7e)  /* �������ַ� */
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
	
	/* ��õ�ַ */
	printf("Enter the address of sector to erase: ");
	addr = get_uint();

	printf("erasing ...\n\r");
	nand_erase(addr, 128*1024);
}
void do_write_nand_flash(void) /*write nand flash ��*/
{
	unsigned int addr;
	unsigned char str[100];
	int i, j;
	unsigned int val;
	
	/* ��õ�ַ */
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
      	/*��ӡ�˵�,������ѡ���������*/
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



