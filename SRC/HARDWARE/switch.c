#include "switch.h"
#include "d5026.h"
#include "timer.h"

u8 	RL_Flag=0,
	preheat_Flag = 1,
	pwr_Mod[4] = {3,3,3,3},
	tim_Mod[4] = {3,3,3,3},
	set_Num=0,
	RTC_State = RTC_0,
	sub_State = S_STA_0,
	State = STATE_0;

u16 Ctrl_STA = 0;		/*
						[15] OC_Flag
						[6] 
						[5] H_W_Flag
						[4] H_Flag
						[3] 
						[2] 
						[1] start_Flag
						[0] run_Flag
*/
	
u16	time_Set[3]={10*60,20*60,30*60},
	TR_Mode = 0,
	pwr[3]={20,40,60};

void Gen_Fan_CTRL(void)
{
	if(!(Delay_STA & 1<<3))
	{
		PBout(10) = 0;			//风扇关
		TIM4->CR1 &= ~0x01;  	//停止TIM4
	}
	else
	{
		PBout(10) = 1;			//风扇开
		TIM4->CR1 |= 0x01;  	//使能TIM4
	}
}

