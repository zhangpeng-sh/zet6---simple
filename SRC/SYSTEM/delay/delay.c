#include "sys.h"
void delay_us(u32 nus)
{ 	 
	SysTick->LOAD=nus*9; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL=0x01 ;          //开始倒数	 
	while(!(SysTick->CTRL&(1<<16))&&SysTick->CTRL)//等待时间到达   
	SysTick->CTRL=0x00;       //关闭计数器
}
void delay_ms(u16 nms)
{ 	 
	SysTick->LOAD=nms*9000; //时间加载	  		 
	SysTick->VAL=0x00;        //清空计数器
	SysTick->CTRL=0x01 ;          //开始倒数	 
	while(!(SysTick->CTRL&(1<<16))&&SysTick->CTRL);//等待时间到达   
	SysTick->CTRL=0x00;       //关闭计数器
}	 
void Stm32_Clock_Init()
{
 	RCC->APB1RSTR = 0x00000000;	RCC->APB2RSTR = 0x00000000; 	  
  	RCC->AHBENR = 0x00000014;   RCC->APB2ENR = 0x00000000;	RCC->APB1ENR = 0x00000000;
	RCC->CR = 0x00000081; 		RCC->CIR = 0x00000000;  	SCB->VTOR = 0x08000000;
 	RCC->CR|=1<<16;  		while(!(RCC->CR>>17));				//等待外部时钟就绪
	RCC->CFGR=0X001D8400;	FLASH->ACR|=0x32;	  				//FLASH 2个延时周期
	RCC->CR|=1<<24;			while(!(RCC->CR>>25));				//等待PLL锁定
	RCC->CFGR|=0x02;		while((RCC->CFGR&0x08)!=0x08);		//等待PLL作为系统时钟设置成功
}
void Periphery_Init(void)
{
	Stm32_Clock_Init();
	SCB->AIRCR = 0x05FA0500;							//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	RCC->APB2ENR |= 0x0D|1<<7;		AFIO->MAPR |= 0x02000000;		//禁用JTAG
	GPIOA->CRH &= 0x0FFFFFF0;	GPIOA->CRH |= 0x30000003;
	GPIOA->CRL &= 0x00000000;	GPIOA->CRL |= 0x30333333;	
	GPIOB->CRH &= 0x00000000;	GPIOB->CRH |= 0x333333BB;
	GPIOB->CRL &= 0x0000FF0F;	GPIOB->CRL |= 0x43330000;
	GPIOA->ODR &= 0x7EFF;		GPIOB->ODR &= 0x1F4F;									//ds1302
	GPIOF->CRL =  0x38333333;	GPIOF->ODR = 0x00FF;
}
