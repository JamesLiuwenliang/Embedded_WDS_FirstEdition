

/*协处理命令必须用汇编代码写*/
//将晶振的时钟通过PLL后达到400M
.text
.global _start

_start:

	/*关闭看门狗*/
	ldr r0,=0x53000000
	ldr r1,=0
	str r1,[r0]

	/*设置PLL,FCLK : HCLK : PCLK = 400m : 100m : 50m */
	/* LOCKTIME(0x4c000000) = 0xFFFFFFFF */
	ldr r0,=0x4c000000
	ldr r1,=0xFFFFFFFF
	str r1,[r0]

	/* CLKDIVN(0x4c000014): 0x5,tFCLK : tHCLK : tPCLK = 1 : 4 : 8 */
	ldr r0,=0x4c000014
	ldr r1,=0x5
	str r1,[r0]

	/* 设置CPU工作异步模式 */
	mrc  p15,0,r0,c1,c0,0   
	orr  r0,r0,#0xc0000000 // R1_nF:OR:R1_iA  
	mcr  p15,0,r0,c1,c0,0 


	/* 设置MPLL(0x4C000004) = (92<<12) | (1<<4) | (1<<0)
	 * m = MDIV + 8 = 92+8=100
	 * p = (PDIV + 2) = 1+2 =3
	 * s = SDIV = 1
	 * FCLK = 2*m*Fin/(p*2^S) =(2*100*12)/(3*2^1) = 400M
	 */
	ldr r0,=0x4C000004
	ldr r1,= (92<<12) | (1<<4) | (1<<0)
	str r1,[r0]
	
	/* 一旦设置PLL ,就会锁定lock time 直达PLL输出稳定
	 * 然后CPU工作于新的频率FCLK
	 */

	
	/*设置内存: sp栈*/
	/* 分辨是nor启动还是nand启动
	 * 写0到0地址,再读出来(nor 上面是无法写数据的)
	 * 如果得到0,表示0地址上的内容被修改了,它对应的ram,就是nand启动
	 * 否则是nor
	 */

	mov r1,#0
	ldr r0,[r1] /*读出0地址上的数据,来备份*/
	str r1,[r1] /*0 -> [0]*/
	ldr r2,[r1] /* r2 = [0] */
	cmp r1,r2
	ldr sp,=0x40000000+4096 /*nor 启动*/ 
	moveq sp,#4096 /*nand 启动(r1 r2 相等时)*/
	streq r0,[r1] /*恢复原来的值*/


	bl main

halt:
	b halt










	


