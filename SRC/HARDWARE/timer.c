#include "timer.h"
#include "stdio.h"
#include "d5026.h"
#include "ds1302.h"
#include "switch.h"
#include "lcd.h"
#include "delay.h"
#include "config.h"
#include "adc.h"
#include "remote.h"

//版本号
u8 vs[4] = VERSERL;

//配置号 L 22 X 23
u8 ct[4] = CONTAB;

//序列号	0001
u8 sn[4] = SERLNUM;

u16 fan_Half_Second = 60;

u8	quit_Set_Second = 0,
	hour_Set_Buffer[6]={0x25,0x25,0x25,0x25,0x25,0x25},
	minute_Set_Buffer[6]={0x00,0x00,0x00,0x00,0x00,0x00},
	hour_Set_Buf[6]={0x25,0x25,0x25,0x25,0x25,0x25},
	minute_Set_Buf[6]={0x00,0x00,0x00,0x00,0x00,0x00},
	hour_Delay_Set = 0,
	minute_Delay_Set = 0,
	half_Second = 0,
	H_Half_Second,
	minute = 0,
	hour = 0,
	count =60;
	
	u32 Delay_STA = 0;//[26:23] quit_Set_Second, [22:17] quit_Half_Second, [16] quit_Blink_Flag, \
					[15:12] b_Flag,1 [11:9] blink2_Flag,2 [8:5] blink_Flag, 3\
					[4] fan_LED_Flag, [3] fan_Flag, \
					[2] near_Flag, [1] delay_Flag_BKP, [0] delay_Flag\
	
u32		run_Half_Second = 0,
		delay_Set = DEFUALT_DELAY_TIME;	
	
void main_Out_OFF(void)
{	MOC1 =0;	delay_us(4);	MOC = 1;	delay_ms(1);	MOC = 0;}

void main_Out_ON(void)
{	MOC1 =1;	MOC = 0;}
	
void TIM_Init(void)
{
	RCC->APB2ENR |= 1<<11;	NVIC->IP[25] = 0xA0; NVIC->ISER[0] = 1<<25;
	TIM1->ARR = 4999;	TIM1->PSC = 7199;	TIM1->DIER = 0x01;	TIM1->CR1 = 0x01;
	
	RCC->APB1ENR = 1<<0;	NVIC->IP[28] = 0xF0;	NVIC->ISER[0] = 1<<28;
	TIM2->ARR = 9999; TIM2->PSC = 7199; TIM2->DIER = 0x01;	TIM2->CR1 = 0x01;	
	
	RCC->APB1ENR |= 1<<2;	RCC->APB2ENR |= 9;
	TIM4->ARR = 900;	TIM4->PSC = 999;	TIM4->CCMR2 = 0x6868;	TIM4->CCER = 0x1100;	TIM1->CR1 = 1<<7|1;
	fan_Duty_Set(DUTY_DEFAULT);
}

void TIM1_UP_IRQHandler(void)   //TIM1中断
{
	if (TIM1->SR & 0x01)  //检查TIM1更新中断发生与否
	{	
		TIM1->SR = 0;  //清除TIMx更新中断标志
		ds1302_data();
		if(Ctrl_STA&1)							//运行时间
		{
			if(!(Delay_STA&1))
			{
				Delay_STA |= 1<<3;
				fan_Half_Second =0;					//用于风扇关断延迟
			}
			run_Half_Second++;
		}	
		if((fan_Half_Second>1)&&(Delay_STA&1<<3)&&(State==0))			//风扇灯
			{
				Delay_STA |= 1<<4;
				Delay_STA += 1<<9;
				if((Delay_STA&7<<9)==3<<9)	
				{
					cLED_Cmd(LED2,BLUE);
				}
				else if((Delay_STA&7<<9) >= 4<<9)
				{
					Delay_STA &= ~(7<<9);
					cLED_Cmd(LED2,OFF);
				}
			}
		else if(fan_Half_Second==120)				//风扇灯关闭
		{
			Delay_STA  &= ~(1<<4);
			cLED_Cmd(LED2,OFF);
		}
		fan_Half_Second++;
		if(fan_Half_Second >= 120)					//风扇时间
			Delay_STA &= ~(1<<3);
		if(Rmt_VAL & 1<<15)								//人体感应时间
		{
			H_Half_Second++;
			if(H_Half_Second>=20)
			{	
				Rmt_VAL &= ~(1<<15);
				Ctrl_STA &= ~(1<<5);
				Ctrl_STA |= 1;
				H_Half_Second=0;
				BEEP_Cmd(b_DISABLE);
			}				
		}
		if(Ctrl_STA&1)
			half_Second --;
		if(half_Second == 255)						//倒计时
		{
			half_Second = 119;
			minute--;
			if(minute == 255)
			{
				minute =59;
				hour--;
				if(hour == 255)
					hour =99;
			}
		}
		if(RTC_State==RTC_0)							//无字
		{
			LCD_Disp(17,17,17,17);
			cLED_Cmd(LED1,OFF);
			if(!(Delay_STA&1<<4))
				cLED_Cmd(LED2,OFF);
		}
		if(RTC_State==1)								//非设置态 静态字
		{	
			if(State == 1)
			{
				if(sub_State == 0)
				{
					if(fan_Half_Second%10 == 0x03)		//风扇时间只在运行时间内清零，其他时间一直自增。10s周期
					{
						cLED_Cmd(LED1,LED_GREEN);	cLED_Cmd(LED2,LED_GREEN);
					}
					else
					{
						cLED_Cmd(LED1,OFF);	cLED_Cmd(LED2,OFF);
					}
				}		
			}
			if(State == 2)
			{
				cLED_Cmd(LED1,OFF);	cLED_Cmd(LED2,OFF);	LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);	
			}
			else if(State == 9)
			{
				if(sub_State == 1)
				{
					cLED_Cmd(LED1,LED_BLUE);	cLED_Cmd(LED2,OFF);
				}
				else if(sub_State == 2)
				{
					cLED_Cmd(LED1,OFF);		cLED_Cmd(LED2,BLUE);
				}
				LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);			
				write_5026(Disp_Buffer);			//显示刷新
			}
			else if(State == 7)
			{
				cLED_Cmd(LED1,LED_RED);	cLED_Cmd(LED2,OFF);
			}
			else if(State == 6)
			{
				cLED_Cmd(LED1,OFF);	cLED_Cmd(LED2,LED_RED);	LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);	
				
			}
			else if(State == 5)
			{
				cLED_Cmd(LED1,OFF);	cLED_Cmd(LED2,BLUE); LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);	
			}
			else
			{
				if(!(Ctrl_STA&1))
				{
					LCD_Disp(sh[2],g[2],sh[1],g[1]);			//显示实际时间 人体报警
					if((Delay_STA & 0xF<<5)==1<<5)	
					{
						Delay_STA &= ~(0xF<<5);
						if((Ctrl_STA & 1<<15)||(Rmt_VAL & 1<<15))
						{
							BEEP_Cmd(b_ENABLE);	cLED_Cmd(LED1,LED_RED);	cLED_Cmd(LED2,LED_RED);
							LCD_Disp(hour/10,hour%10,minute/10,minute%10);	//显示运行倒计时
						}
						else
							BEEP_Cmd(b_DISABLE);
					}
					else
					{			
						Delay_STA &= ~(0xF<<5);	Delay_STA |= 1<<5;
						BEEP_Cmd(b_DISABLE);			
						if((Ctrl_STA & 1<<15)||(Rmt_VAL & 1<<15))
						{
							cLED_Cmd(LED1,OFF);	cLED_Cmd(LED2,OFF);	LCD_Disp(sh[2],g[2],sh[1],g[1]);	//显示实际时间
						}							
					}	
				}
				else 
				{
					Delay_STA += 1<<5;
					if(Delay_STA & 1<<2)			
					{
						cLED_Cmd(LED2, OFF);Delay_STA &= ~(0xF<<5);	Delay_STA |= 3<<5;
						LCD_Disp(hour/10,hour%10,minute/10,minute%10);	//显示运行倒计时
					}
					else				//倒计时和实际时间交替显示
					{
						if((Delay_STA & 0xF<<5) == 4<<5)	
							LCD_Disp(hour/10,hour%10,minute/10,minute%10);	//显示运行倒计时
						else if((Delay_STA &0xF<<5) >= 8<<5 )
						{			
							Delay_STA &= ~(0xF<<5);	LCD_Disp(sh[2],g[2],sh[1],g[1]);					//显示实际时间
						}
					}			
					if(State == 1)					//自动
					{
						cLED_Cmd(LED1, LED_GREEN);	cLED_Cmd(LED2, LED_GREEN);
					}
					else if(State ==3)				//手动
					{
						cLED_Cmd(LED1, LED_RED);
						if((Delay_STA&3<<5)&&(Delay_STA & 1))		//延时开启闪烁
							cLED_Cmd(LED2, OFF);
						else if(!(Delay_STA&3<<5))
							cLED_Cmd(LED2, LED_GREEN);
					}
				}
			}
		}
		else												//设置态   字闪烁
		{
			if(RTC_State == 2)				//调时模式，小时闪烁
			{
				if((Delay_STA&0xF<<12) == 1<<12)
				{
					LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);
					if(State == 6)
						cLED_Cmd(LED2, LED_RED);
					else if(State == 5)
						cLED_Cmd(LED2, LED_BLUE);
					else if(State == 7)
						cLED_Cmd(LED1, LED_RED);
					Delay_STA &= !(0xF<<12); Delay_STA |= 1<<12;
				}
				else
				{
					LCD_Disp(17,17,sh_Set[1],g_Set[1]);
					if((State == 6 )|| (State == 5)|| (State == 7))
					{
//						cLED_Cmd(LED1, OFF);
//						cLED_Cmd(LED2, OFF);
					}
					Delay_STA &= !(0xF<<12); Delay_STA |= 1<<12;
				}
			}
			else if(RTC_State == RTC_3)				//调时模式，分钟闪烁
			{
				if((Delay_STA&0xF<<12) == 1<<12)
				{
					LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);
					if(State == 6)
						cLED_Cmd(LED2, LED_RED);
					else if(State == 5)
						cLED_Cmd(LED2, LED_BLUE);
					else if(State == 7)
						cLED_Cmd(LED1, LED_RED);
					Delay_STA &= !(0xF<<12); 
				}
				else
				{
					if((State == 6 )|| (State == 5)|| (State == 7))
					{
//						cLED_Cmd(LED2, OFF);
//						cLED_Cmd(LED1, OFF);
					}
					if((State == 6 )|| (State == 5)|| (State == 9))
						LCD_Disp(sh_Set[2],g_Set[2],17,17);
					Delay_STA &= !(0xF<<12); Delay_STA |= 1<<12;
				}
			}
			else if(RTC_State == RTC_4)
			{
				;				
			}
			else if(RTC_State == RTC_5)							//时间闪烁  全部闪烁
			{
				if(Delay_STA & 1<<12)
				{
					if(State == 9)
						LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);
					else
						LCD_Disp(sh[2],g[2],sh[1],g[1]);
					write_5026(Disp_Buffer);
					if(State == 4)
						cLED_Cmd(LED2, LED_BLUE);
					else if(State == 2)
						cLED_Cmd(LED2, LED_RED);
					else if(State == 9)
					{
						cLED_Cmd(LED2, OFF); cLED_Cmd(LED2, OFF);
					}						
					Delay_STA &= !(0xF<<12); 
				}
				else
				{
					LCD_Disp(17,17,17,17);	cLED_Cmd(LED2, OFF); cLED_Cmd(LED2, OFF);
					Delay_STA &= !(0xF<<12); Delay_STA |= 1<<12;
				}
			}
			else if(RTC_State == RTC_6)					//显示版本号 二十秒退出
			{
				
				Delay_STA += 1<<12;	Delay_STA += 1<<17;
				if((Delay_STA & 0xF<<12)==4<<12)
				{	
					LCD_Disp(vs[0],vs[1],vs[2],vs[3]);	cLED_Cmd(LED1, OFF); cLED_Cmd(LED2, OFF);
				}
				else if((Delay_STA & 0xF<<12)==8<<12)
				{
					LCD_Disp(ct[0],ct[1],ct[2],ct[3]);	cLED_Cmd(LED1, OFF); cLED_Cmd(LED2, OFF);
				}
				else if((Delay_STA & 0xF<<12)==12<<12)
				{
					LCD_Disp(sn[0],sn[1],sn[2],sn[3]);	cLED_Cmd(LED1, OFF); cLED_Cmd(LED2, OFF);	Delay_STA &= ~(0xF<<12);
				}
				if((Delay_STA&0x3F)<<17 == 60<<17)
				{
					Delay_STA &= ~(0x3F<<17);	Delay_STA &= ~(1<<16);	Quit();					
				}
			}
			else if(RTC_State == RTC_7)				//n秒退出
			{
				Delay_STA += 1<<17;
				if((Delay_STA&0x3F<<17) >= (Delay_STA&0xF<<23)*2+1)
				{
					Delay_STA &= ~(0x3F<<17);	Delay_STA &= ~(1<<16);
					RTC_State = RTC_0;	State = 0;
					LCD_Disp(17,17,17,17);	cLED_Cmd(LED1,OFF);	cLED_Cmd(LED2,OFF);	
				}
				if(Delay_STA &1<<16)
				{
					if((Delay_STA&0x3F<<17)%2 == 0)
					{
						LCD_Disp(17,17,17,17);
						cLED_Cmd(LED1,OFF);
						cLED_Cmd(LED2,OFF);
					}
					else
					{	LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);
						if(State == 5)
							cLED_Cmd(LED2,BLUE);
					}
				}
			}
		}
	}
	TIM1->SR = 0;
}

void TIM2_IRQHandler(void)   //TIM2中断
{
	if (TIM2->SR&0x01)  //检查TIM2更新中断发生与否
	{
		count ++;		
		if(Ctrl_STA&1)			//继电器延时开启
		{
			PAout(5) = 1;
			if((RL_Flag++) >= D_TIME)
			{
				RL_Flag = D_TIME;
			}
		}
		else
		{
			RL_Flag = 0;
			PAout(5) = 0;
		}
		if(count>pwr[pwr_Mod[0]-1])
		{
			main_Out_OFF();
		}
		if(count>=60)
		{
			count = 0;
			if((Ctrl_STA&1)&&(RL_Flag>=D_TIME))
			{
				main_Out_ON();
			}
		}
	}
	TIM2->SR = 0;  //清除TIMx更新中断标志 
}

void fan_Duty_Set(u8 dut)
{	
	TIM4->CCR3 = 9*dut;
	TIM4->CCR4 = 9*dut;
}

