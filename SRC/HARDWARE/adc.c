#include "sys.h"
void  Adc_Init(void)
{ 	
	RCC->APB2ENR |= 1<<9;	RCC->APB2RSTR |= 1<<9;	RCC->APB2RSTR &= ~(1<<9);
	ADC1->CR2 |= 0x001E0001;
	ADC1->CR2 |= 1<<3;	while(ADC1->CR2&1<<3);	
	ADC1->CR2 |= 1<<2; 	while(ADC1->CR2&1<<2);
}				  
u16 Get_Adc(u8 ch)   
{
	ADC1->SMPR2 |= 7<<(3*ch);	
	ADC1->CR2 |= 1<<22;	while(!(ADC1->SR & 0x02));
	return ADC1->DR;
}
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u8 t;
	u32 temp_val=0;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);		delay_us(200);
	}
	return temp_val/times;
} 	 

void OC_Detec(void)					//过流检测
{
	if(Get_Adc_Average(9,10)>OC_Compare)
	{
		Ctrl_STA |= 1<<15;	Ctrl_STA &= ~1; 
		PAout(5) = 0;	PBout(11) = 1;
		while(1)
		{
			IWDG->KR = 0xAAAA;
			LCD_Disp(17,17,14,1);	BEEP_Cmd(b_ENABLE);		write_5026(Disp_Buffer);	delay_ms(1000);
			LCD_Disp(17,17,17,17);	BEEP_Cmd(b_DISABLE);	write_5026(Disp_Buffer);	delay_ms(1800);
		}
	}
}
	
void MOS_Detec(void)					//MOS击穿检测
{
	if(Get_Adc_Average(9,10)>SC_Compare)						//220V继电器控制
	{
		if((!(Ctrl_STA&1)&&!(Delay_STA&1))||((Ctrl_STA&1)&&(MOC1 == 0)))		//两种情况：1.不在run期间，2.run在无效区间
		{
			delay_ms(SC_Time*1000);		delay_ms(SC_Time*1000);		delay_ms(SC_Time*1000);		delay_ms(SC_Time*1000);
			delay_ms(SC_Time*1000);		delay_ms(SC_Time*1000);		delay_ms(SC_Time*1000);		delay_ms(SC_Time*1000);
			if(Get_Adc_Average(9,10)>SC_Compare)
			{
				TIM2->CR1 &= ~(0x01);	PAout(5) = 0;
				while(1)
				{
					IWDG->KR = 0xAAAA;
					LCD_Disp(17,17,14,2);	BEEP_Cmd(b_ENABLE);	write_5026(Disp_Buffer);	delay_ms(1000);			//MOS击穿E2
					LCD_Disp(17,17,17,17);	BEEP_Cmd(b_DISABLE);write_5026(Disp_Buffer);	delay_ms(1800);
				}
			}
		}
	}
}

