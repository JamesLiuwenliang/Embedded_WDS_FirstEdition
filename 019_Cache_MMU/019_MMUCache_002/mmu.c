
/*设置CP15寄存器C3的格式*/


#define MMU_SECDESC_AP 		(3<<10)	//确认为可读可写
#define MMU_SECDESC_DOMAIN    (0<<5)	//域
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




/*	CB :为00时,表示即不使用Cache也不使用write buffer 
 *	创建一级页表
 *
 *	VA			PA			CB
 *  0			0			11
 *	0x40000000	0x40000000	11
 *
 *	64M SDRAM全部映射
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
 *	0xB0000000	0x30000000	11  //把链接地址映射到SDRAM起始的地方,但是需要修改下Start.s文件,在重定位之前,必须使能MMU
 *
 *
 */
void create_page_table()
{
	/*1. 页表在哪 0x32000000 (占据16kb)*/
	/*ttb : translation table base*/
	unsigned int *ttb = (unsigned int *)0x32000000;
	unsigned int va ,pa;

	/*2. 根据va,pa设置页表条目*/

	/*2.1 for sdram,nor flash */
	create_secdesc( ttb ,0 ,0 ,IO);
	/*这里需要使用IO映射方式,如果启动cache,烧写到norflash会失效,因为norflash会通过写字符串判断,但是启动cache后就是往cache里面写数据,一定会不一样*/

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
    create_secdesc( ttb ,0xB0000000 ,0x30000000 ,MEM);/*0xB0000000这一块地址的空间会使用cache*/


}



