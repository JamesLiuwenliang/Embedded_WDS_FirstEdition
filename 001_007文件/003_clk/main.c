
void delay(volatile int d)
{
	while(d--);
}

int main(int which)
{
	volatile unsigned int *pGPFCON = (volatile unsigned int *)0x56000050;
	volatile unsigned int *pGPFDAT = (volatile unsigned int *)0x56000054;
	int val =0; /*val : 0b000, 0b111/
	int tmp;

	*pGPFCON &= ~((3<<8) | (3<<10) | (3<<12));/*设置GPF456为输出引脚*/
	*pGPFCON |= ~((1<<8) | (1<<10) | (1<<12));/*设置GPF456为输出引脚*/

	/* 配置3个按键引脚为输入引脚
	 * GPF0(S2),GPF2(S3),GPG3(S4)
	 * 本次没有加按键!
	 */
	 
	*pGPFCON &= ~((3<<0) | (3<<4));/*GPF0,2*/
	*pGPGCON &= ~((3<<6));/*GPG3*/


	/*循环点亮*/
	while(1)
	{

		tmp = ~val;
		tmp &= 7;
		
		*pGPFDAT &= ~(7<<4);
		*pGPFDAT |= (val<<4);
		val++;

		delay(100000);

		if(val == 8)
		{
			val=0;
		}
	
	}



	return 0;

}



