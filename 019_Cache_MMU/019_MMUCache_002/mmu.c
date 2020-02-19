
/*����CP15�Ĵ���C3�ĸ�ʽ*/


#define MMU_SECDESC_AP 		(3<<10)	//ȷ��Ϊ�ɶ���д
#define MMU_SECDESC_DOMAIN    (0<<5)	//��
#define MMU_SECDESC_NCNB      (0<<2)	//CB
#define MMU_SECDESC_WB      	(3<<2)	//CB
#define MMU_SECDESC_TYPE     	((1<<4)|(1<<1))

#define MMU_SECDESC_FOR_IO 	(MMU_SECDESC_AP | MMU_SECDESC_DOMAIN | MMU_SECDESC_NCNB | MMU_SECDESC_TYPE)
#define MMU_SECDESC_FOR_MEM 	(MMU_SECDESC_AP | MMU_SECDESC_DOMAIN | MMU_SECDESC_WB | MMU_SECDESC_TYPE)

#define IO 1
#define MEM 0

void create_secdesc(unsigned int *ttb,unsigned int va,unsigned int pa , int io)
{

	int index;


	index = va / 0x100000; /*1M*/

	if(io)
	{
		ttb[index] = (pa & 0xfff00000) | (MMU_SECDESC_FOR_IO);
	}
	else
	{
		ttb[index] = (pa & 0xfff00000) | (MMU_SECDESC_FOR_MEM);
	}


}




/*	CB :Ϊ00ʱ,��ʾ����ʹ��CacheҲ��ʹ��write buffer 
 *	����һ��ҳ��
 *
 *	VA			PA			CB
 *  0			0			11
 *	0x40000000	0x40000000	11
 *
 *	64M SDRAMȫ��ӳ��
 *	0x30000000	0x30000000	11
 *	...
 *	0x33f00000	0x33f00000	11
 *	
 *	register: 0x48000000~0x5B00001C
 *	0x48000000	0x48000000	00
 *	...
 *	0x5B000000	0x5B000000	00
 *
 *	Framebuffer : 0x33c00000
 *	0x33c00000	0x33c00000	00
 *
 *	link address:
 *	0xB0000000	0x30000000	11  //�����ӵ�ַӳ�䵽SDRAM��ʼ�ĵط�,������Ҫ�޸���Start.s�ļ�,���ض�λ֮ǰ,����ʹ��MMU
 *
 *
 */
void create_page_table()
{
	/*1. ҳ������ 0x32000000 (ռ��16kb)*/
	/*ttb : translation table base*/
	unsigned int *ttb = (unsigned int *)0x32000000;
	unsigned int va ,pa;

	/*2. ����va,pa����ҳ����Ŀ*/

	/*2.1 for sdram,nor flash */
	create_secdesc( ttb ,0 ,0 ,IO);
	/*������Ҫʹ��IOӳ�䷽ʽ,�������cache,��д��norflash��ʧЧ,��Ϊnorflash��ͨ��д�ַ����ж�,��������cache�������cache����д����,һ���᲻һ��*/

    /*2.2 for sdram,when nor boot */
    create_secdesc( ttb ,0x40000000 ,0x40000000 ,MEM);

    /*2.3 for 64M sdram */
	va = 0x30000000;
	pa = 0x30000000;

	for(; va <= 0x33f00000 ; )
	{
		create_secdesc(ttb ,va ,pa ,MEM);
		va += 0x100000;
		pa += 0x100000;
	}

    /*2.4 for register 0x48000000~0x5B00001C*/
	va = 0x48000000;
	pa = 0x48000000;

	for(; va <= 0x5B000000 ; )
	{
		create_secdesc( ttb ,va ,pa ,IO);
		va += 0x100000;
		pa += 0x100000;
	}
	
	/*2.5 for Framebuffer 0x33c00000 */
    create_secdesc( ttb ,0x33c00000 ,0x33c00000,IO);

	/*2.6 for link address 0xB0000000	0x30000000	11*/
    create_secdesc( ttb ,0xB0000000 ,0x30000000 ,MEM);/*0xB0000000��һ���ַ�Ŀռ��ʹ��cache*/


}


