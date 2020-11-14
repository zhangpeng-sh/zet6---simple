#include "usart.h"	 
struct __FILE 
{ 
	int handle; 
}; 
FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
	
u8 USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.
u16 USART_RX_STA=0; //����״̬���	  //����״̬//bit15��������ɱ�־//bit14�����յ�0x0d//bit13~0�����յ�����Ч�ֽ���Ŀ
  
void uart_init(u32 bound)
{
	RCC->APB2ENR |= 0x4004;
	GPIOA->CRH &= 0xFFFFF00F;	GPIOA->CRH |= 0X000004B0;	
	NVIC->IP[37] = 0xF0;	NVIC->ISER[1] = 1<<5;	
	USART1->BRR = 0x271;	USART1->CR1 |= 0x202C;
}
void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
	if((USART1->SR&1<<5) != 0)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART1->DR;//(USART1->DR);	//��ȡ���յ�������		
		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			if(USART_RX_STA&0x4000)//���յ���0x0d
			{
				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1)) USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}   		 
	} 
} 

