
.text
.global _start

_start:

	/*关闭看门狗*/
	ldr r0,=0x53000000
	ldr r1,=0
	str r1,[r0]

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




