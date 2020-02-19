
/*���ļ�ֱ�Ӳ����ײ�Ӳ��,�ο�datasheet��Figure 20-1. IIC-Bus Block Diagram ,�����漰�Ĵ����Ĳ���*/
/*IIC�������ĺ��ľ���i2c_interrupt_func ����жϺ���,�ο����������ͼ*/
/*�����оƬ,ʵ���Լ���i2c_controller
 *						.init
 *						.master_xfer //�������������IIC ����
 *						.name        //����
 */

#include "../s3c2440_soc.h"
#include "iic_controller.h"

static p_i2c_msg p_cur_msg;/*���洫��״̬*/


/*�˺�����IIC�������ĺ���*/
void i2c_interrupt_func(int irq )
{
	int index;
	unsigned int iicstat = IICSTAT;
	/*ÿ������һ������,������һ���ж�*/

	p_cur_msg->cnt_transferred ++;


	/*����ÿ�δ���,��һ���ж�ʱ"�Ѿ������豸��ַ"*/
	if(p_cur_msg ->flags == 0) /*write*/
	{
		/*���ڵ�һ���ж�,���Ƿ����豸��ַ�������
		 *��Ҫ�ж��Ƿ���ACK
		 * ��ACK : �豸����
		 * ��ACK : ���豸,����,ֱ�ӽ�������
		 */
		/*���������ж�,Ҫ����������һ���豸*/
		if(p_cur_msg->cnt_transferred == 0)/*�����ǵ�һ���ж�,Ҳ����Ӧ�ý���ASK�ź�*/
		{
			if(iicstat & (1<<0))
			{
				/*NO ASK*/
				/*ֹͣ����*/
				IICSTAT = 0xD0 ;
				IICCON &= ~(1<<4);/*���penging bit*/
				p_cur_msg->err = -1;				
				delay(1000);	
				return ;
			}
		}

		if(p_cur_msg->cnt_transferred < p_cur_msg->len)
		{
			/*���������ж�,Ҫ����������һ������*/
			IICDS = p_cur_msg->buf[p_cur_msg->cnt_transferred]; /*����Ҫע��һ���߼�����,*/
		
			IICCON &= ~(1<<4);/*���penging bit*/
		}
		else
		{				
			/*ֹͣ����*/
			IICSTAT = 0xD0 ;
			IICCON &= ~(1<<4);/*���penging bit*/
			p_cur_msg->err = -1;				
			delay(1000);	
			return ;
		}
				
	}
	
	else /*read*/
	{
		/*���ڵ�һ���ж�,���Ƿ����豸��ַ�������
		 *��Ҫ�ж��Ƿ����豸
		 * ��ACK : �豸����,�ָ�IIC����,��������һ���жϲſ��Եõ�����
		 * ��ACK : ���豸,����,ֱ�ӽ�������
		 */
		/*���������ж�,Ҫ����������һ���豸*/


		if(p_cur_msg->cnt_transferred == 0)/*�����ǵ�һ���ж�,Ҳ����Ӧ�ý���ASK�ź�*/
		{
			if(iicstat & (1<<0))
			{
				/*NO ASK*/
				/*ֹͣ����*/
				IICSTAT = 0x90 ;
				IICCON &= ~(1<<4);/*���penging bit*/
				p_cur_msg->err = -1;				
				delay(1000);	
				return ;
			}
			else
			{
				/* ack */
				/*�ָ�IIC����*/
				IICCON &= ~(1<<4);/*���penging bit*/
				return ;
			
				

			}
		}

		
		/*�ǵ�һ���жϱ�ʾ�õ���һ��������
		 *Ҫ��IICDS�Ĵ�������������ݲ�����
		 */
		if(p_cur_msg->cnt_transferred < p_cur_msg->len)
		{
			/*	�ǵ�һ���ж�,��ʾ�õ�һ���µ�����
			 *	��IICDS����,����
			 */
			index = p_cur_msg->cnt_transferred - 1;
			p_cur_msg->buf[index] = IICDS;
			IICCON &= ~(1<<4);/*���penging bit*/

		}
		else
		{				
			/*ֹͣ����*/
			IICSTAT = 0x90 ;
			IICCON &= ~(1<<4);/*���penging bit*/
			delay(1000);	
			return ;
		}




	}

}

/*Figure 20-6. Operations for Master/Transmitter Mode */
int do_master_tx(p_i2c_msg msgs)
{
	p_cur_msg = msgs;

	msgs->cnt_transferred = -1; /*������Ϊ-1*/
	msgs->err = 0;
	
	/*���üĴ�����������*/
	/*1. ����Ϊmaster tx mode
	 *2. �Ѵ��豸��ַд��IICCDS
	 *3. IICSTAT = 0xf0 ,���ݼ������ͳ�ȥ,�������жϲ���
	 */

	IICDS= msgs->addr << 1; /*���ݵĸ�7λ��ʾ�豸��ַ,���λ��0��ʱ���ʾд*/

	IICSTAT = 0xf0;




	/*�����Ĵ������ж����*/

	/*ѭ���ȴ��жϴ������*/
	while(!msgs->err && msgs->cnt_transferred != msgs->len);
	if(msgs->err)
		return -1;
	else 
		return 0;

}


/*�ο�Figure 20-7. Operations for Master/Receiver Mode */
void do_master_rx(p_i2c_msg msgs)
{
	p_cur_msg = msgs;
	
	msgs->cnt_transferred = -1; /*������Ϊ0*/
	msgs->err = 0;

	/*���üĴ�����������*/
	/*1. ����Ϊmaster rx mode
	 *2. �Ѵ��豸��ַд��IICCDS
	 *3. IICSTAT = 0xb0 ,���豸��ַ�����ͳ�ȥ,�������жϲ���
	 */

	IICDS= (msgs->addr << 1 ) | (1<<0); /*���ݵĸ�7λ��ʾ�豸��ַ,���λ��0��ʱ���ʾд*/

	IICSTAT = 0xb0;

	/*�����Ĵ������ж����*/

	/*ѭ���ȴ��жϴ������*/
	while(!msgs->err && msgs->cnt_transferred != msgs->len);
	if(msgs->err)
		return -1;
	else 
		return 0;


}



int s3c2440_master_xfer(p_i2c_msg msgs,int num)//����ŵ���ָ��p_i2c_msg,���Ǵ�����������С�ܶ�
{
	int i;
	int err;
	for(i=0;i<num;i++)
	{

		if(msgs[i]->flags == 0)/*write*/
			err = do_master_tx(msgs[i]);/*master����transmit,��д*/
		else 
			err = do_master_rx(msgs[i]);/*master����receive,����*/

		if(err)
			return err;
	}
	return 0;
}


void s3c2440_i2c_con_init()
{
	/*����ʱ��:PCLK=50MHz,����IIC����̫����*/	
	/* [4] : ����Ϊ1ʱ,��ʾ�жϷ�����,д��0������ָ�IIC����
	 * [3:0] : Tx clock = IICCLK/(IICCON[3:0]+1). 
	 * Tx clock = 100KHz = 	50MHz / 16 /(IICCON[3:0] + 1)
	 */
	IICCON = (1<<7) | (0<<6) | (1<<5) | (30<<0) ;

	/*ע���жϴ�����*/
	register_irq(27,i2c_interrupt_func);

}








static i2c_controller s3c2440_i2c_con = {
	.name = "s3c2440",
	.init = s3c2440_i2c_con_init,
	.master_xfer = s3c2440_master_xfer,
};

void s3c2440_i2c_con_add()
{
	register_i2c_controller(&s3c2440_i2c_con);/*���ע��*/
}



