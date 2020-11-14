#include "usart.h"	 
struct __FILE 
{ 
	int handle; 
}; 
FILE __stdout;
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
	
u8 USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.
u16 USART_RX_STA=0; //接收状态标记	  //接收状态//bit15，接收完成标志//bit14，接收到0x0d//bit13~0，接收到的有效字节数目
  
void uart_init(u32 bound)
{
	RCC->APB2ENR |= 0x4004;
	GPIOA->CRH &= 0xFFFFF00F;	GPIOA->CRH |= 0X000004B0;	
	NVIC->IP[37] = 0xF0;	NVIC->ISER[1] = 1<<5;	
	USART1->BRR = 0x271;	USART1->CR1 |= 0x202C;
}
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	if((USART1->SR&1<<5) != 0)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART1->DR;//(USART1->DR);	//读取接收到的数据		
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1)) USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   		 
	} 
} 

