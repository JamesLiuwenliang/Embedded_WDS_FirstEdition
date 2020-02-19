
/*该文件直接操作底层硬件,参考datasheet的Figure 20-1. IIC-Bus Block Diagram ,里面涉及寄存器的操作*/
/*IIC控制器的核心就是i2c_interrupt_func 这个中断函数,参考上面的两张图*/
/*具体的芯片,实现自己的i2c_controller
 *						.init
 *						.master_xfer //由这个函数启动IIC 传输
 *						.name        //名字
 */

#include "../s3c2440_soc.h"
#include "iic_controller.h"

static p_i2c_msg p_cur_msg;/*保存传输状态*/

int isLastData(void)
{
	if(p_cur_msg->cnt_transferred == p_cur_msg->len -1)
		return 1;	/*正要开始传输最后一个字节*/
	else 
		return 0;


}
void resume_iic_with_ack()
{
	unsigned int iiccon  = IICCON;
	iiccon |= (1<<7); /*回应ACK*/
	iiccon &= ~(1<<4); /*恢复IIC*/
	IICCON = iiccon;
}
void resume_iic_without_ack()
{
	unsigned int iiccon  = IICCON;
	iiccon &= ~((1<<7) | (1<<4)); /*不回应ACK,恢复IIC*/
	IICCON = iiccon;
}




/*此函数是IIC控制器的核心*/
void i2c_interrupt_func(int irq )
{
	int index;
	unsigned int iicstat = IICSTAT;
	
	
	/*每传输完一个数据,将产生一个中断*/

	p_cur_msg->cnt_transferred ++;


	/*对于每次传输,第一个中断时"已经发出设备地址"*/
	if(p_cur_msg ->flags == 0) /*write*/
	{
		/*对于第一个中断,它是发送设备地址后产生的
		 *需要判断是否有ACK
		 * 有ACK : 设备存在
		 * 无ACK : 无设备,出错,直接结束传输
		 */
		/*对于其他中断,要继续发送下一个设备*/
		if(p_cur_msg->cnt_transferred == 0)/*表明是第一次中断,也就是应该接受ASK信号*/
		{
			if(iicstat & (1<<0))
			{
				/*NO ASK*/
				/*停止传输*/
				IICSTAT = 0xD0 ;
				IICCON &= ~(1<<4);/*清除penging bit*/
				p_cur_msg->err = -1;				
				delay(1000);	
				return ;
			}
			else
			{
				/*如果是最后一个数据,启动传输的时候要设置为不回应ACK*/
				if(isLastData())
				{
					resume_iic_without_ack();
				}
				else
				{
					resume_iic_with_ack();				
				}
				return ;

			}
		}

		if(p_cur_msg->cnt_transferred < p_cur_msg->len)
		{
			/*对于其他中断,要继续发送下一个数据*/
			IICDS = p_cur_msg->buf[p_cur_msg->cnt_transferred]; /*这里要注意一个逻辑错误,*/
		
			IICCON &= ~(1<<4);/*清除penging bit*/
		}
		else
		{				
			/*停止传输*/
			IICSTAT = 0xD0 ;
			IICCON &= ~(1<<4);/*清除penging bit*/	
			delay(1000);	
		}
				
	}
	
	else /*read*/
	{
		/*对于第一个中断,它是发送设备地址后产生的
		 *需要判断是否有设备
		 * 有ACK : 设备存在,恢复IIC传输,这样在下一个中断才可以得到数据
		 * 无ACK : 无设备,出错,直接结束传输
		 */
		/*对于其他中断,要继续发送下一个设备*/


		if(p_cur_msg->cnt_transferred == 0)/*表明是第一次中断,也就是应该接受ASK信号*/
		{
			if(iicstat & (1<<0))
			{
				/*NO ASK*/
				/*停止传输*/
				IICSTAT = 0x90 ;
				IICCON &= ~(1<<4);/*清除penging bit*/
				p_cur_msg->err = -1;				
				delay(1000);	
				return ;
			}
			else
			{
				/*如果是最后一个数据,是不能回应ACK信号的*/
				IICCON |= (0<<7);/* 不回应ACK信号 */

				
				/* ack */
				/*恢复IIC传输*/
				IICCON &= ~(1<<4);/*清除penging bit*/
				return ;
			
				

			}
		}

		
		/*非第一个中断表示得到了一个新数据
		 *要从IICDS寄存器里面读出数据并保存
		 */
		if(p_cur_msg->cnt_transferred < p_cur_msg->len )
		{
			/*	非第一个中断,表示得到一个新的数据
			 *	从IICDS读出,保存
			 */
			index = p_cur_msg->cnt_transferred - 1;
			p_cur_msg->buf[index] = IICDS;
			
			/*如果是最后一个数据,启动传输时要设置为不回应ASK
			 *恢复IIC传输
			 */
			if(isLastData())
			{
				resume_iic_without_ack();
			}
			else
			{
				resume_iic_with_ack();
			}
			
		}
		else if(isLastData())
		{
			resume_iic_without_ack();
		}
		else
		{				
			/*停止传输*/
			IICSTAT = 0x90 ;
			IICCON &= ~(1<<4);/*清除penging bit*/
			delay(1000);	
			return ;
		}




	}

}

/*Figure 20-6. Operations for Master/Transmitter Mode */
int do_master_tx(p_i2c_msg msgs)
{
	p_cur_msg = msgs;

	msgs->cnt_transferred = -1; /*先设置为-1*/
	msgs->err = 0;
	
	/*设置寄存器启动传输*/
	/*1. 配置为master tx mode
	 *2. 把从设备地址写入IICCDS
	 *3. IICSTAT = 0xf0 ,数据即被发送出去,将导致中断产生
	 */
	IICCON |= (1<<7);/*表示在TXmode里:在ACK周期释放SDA */
	
	IICSTAT = (1<<4);

	IICDS= msgs->addr << 1; /*数据的高7位表示设备地址,最低位是0的时候表示写*/

	IICSTAT = 0xf0;




	/*后续的传输由中断完成*/

	/*循环等待中断处理完毕*/
	while(!msgs->err && msgs->cnt_transferred != msgs->len);
	if(msgs->err)
		return -1;
	else 
		return 0;

}


/*参考Figure 20-7. Operations for Master/Receiver Mode */
void do_master_rx(p_i2c_msg msgs)
{
	p_cur_msg = msgs;
	
	msgs->cnt_transferred = -1; /*先设置为0*/
	msgs->err = 0;

	/*设置寄存器启动传输*/
	/*1. 配置为master rx mode
	 *2. 把从设备地址写入IICCDS
	 *3. IICSTAT = 0xb0 ,从设备地址被发送出去,将导致中断产生
	 */
	
	IICCON |= (1<<7);/*表示在RXmode里:在ACK周期回应ACK给从设备*/
	IICSTAT = (1<<4);

	IICDS= (msgs->addr << 1 ) | (1<<0); /*数据的高7位表示设备地址,最低位是0的时候表示写*/

	IICSTAT = 0xb0;

	/*后续的传输由中断完成*/

	/*循环等待中断处理完毕*/
	while(!msgs->err && msgs->cnt_transferred != msgs->len);
	if(msgs->err)
		return -1;
	else 
		return 0;


}



int s3c2440_master_xfer(p_i2c_msg msgs,int num)//这里放的是指针p_i2c_msg,会是传过来的数据小很多
{
	int i;
	int err;
	for(i=0;i<num;i++)
	{

		if(msgs[i].flags == 0)/*write*/
			err = do_master_tx(&msgs[i]);/*master机做transmit,即写*/
		else 
			err = do_master_rx(&msgs[i]);/*master机做receive,即读*/

		if(err)
			return err;
	}
	return 0;
}


void s3c2440_i2c_con_init()
{
	/*配置引脚用于IIC*/
	GPECON &= ~((3<<28) | (3<<30));
	GPECON |= ((2<<28) | (2<<30));


	/*设置时钟:PCLK=50MHz,对于IIC来讲太快了*/	
	/* [4] : 读出为1时,表示中断发生了,写入0来清除恢复IIC操作
	 * [3:0] : Tx clock = IICCLK/(IICCON[3:0]+1). 
	 * Tx clock = 100KHz = 	50MHz / 16 /(IICCON[3:0] + 1)
	 */
	IICCON = (1<<7) | (0<<6) | (1<<5) | (30<<0) ;

	/*注册中断处理函数*/
	register_irq(27,i2c_interrupt_func);

}








static i2c_controller s3c2440_i2c_con = {
	.name = "s3c2440",
	.init = s3c2440_i2c_con_init,
	.master_xfer = s3c2440_master_xfer,
};

void s3c2440_i2c_con_add()
{
	register_i2c_controller(&s3c2440_i2c_con);/*添加注册*/
}



