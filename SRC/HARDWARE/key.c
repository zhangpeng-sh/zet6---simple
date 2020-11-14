#include "key.h"
#include "delay.h"
#include "d5026.h"
#include "timer.h"
#include "stmflash.h"
#include "config.h"
#include "switch.h"
#include "ds1302.h"

extern u8 State,
		sub_State;

extern u32 Delay_STA;

u16 KB_STA = 0xC000;		//[15] key_up=1, [14] OnOff_Up_Flag=1, [13:7] KB_Cnt2, [6:0] KB_Cnt

typedef void(*pFunction)(void);
u32 JumpAddress;
pFunction Jump_To_Application;


u8 KEY_Scan(u8 mode)
{
	if(mode)KB_STA|=1<<15;  //支持连按
	if((KB_STA&1<<15)&&(KB_Mode==KB_PRESS||KB_Time==KB_PRESS||KB_Plus==KB_PRESS||KB_Minus==KB_PRESS||KB_OnOff==KB_PRESS))
	{
		delay_ms(10);		//去抖动
		KB_STA &= ~(1<<15);
		if		(KB_Mode==KB_PRESS)return KB_Mode_PRES;
		else if(KB_Time==KB_PRESS)return  KB_Time_PRES;
		else if(KB_Plus==KB_PRESS)return  KB_Plus_PRES;
		else if(KB_Minus==KB_PRESS)return  KB_Minus_PRES;
		else if(KB_OnOff==KB_PRESS)return  KB_OnOff_PRES;
	}else if(KB_Mode==KB_UP&&KB_Time==KB_UP&&KB_Plus==KB_UP&&KB_Minus==KB_UP&&KB_OnOff==KB_UP)KB_STA |= 1<<15;
 	return 0;// 无按键按下
}
void my_KEY_Prcss(void)
{
	switch(KEY_Scan(1))
	{
		case KB_OnOff_PRES:
			KB_STA += 1<<7;
			TIM1->CNT = 0;
			Delay_STA &= ~(0xF<<12);	Delay_STA |= 1<<12;
			if((State != 9) && ((KB_STA&0x3F80) >= 50<<7))
			{
				Rmt_VAL &= ~(1<<15);	
				H_Half_Second =0 ;
				Delay_STA &= ~1;
				hour_Delay_Set = 0;
				minute_Delay_Set = 0;
				delay_Set = 0;
				Delay_STA &= ~(1<<1);
				Delay_STA &= ~(1<<2);
				Quit();
				KB_STA &= ~(1<<14);
			}
			break;
		default:
			KB_STA &= 0xC07F;
			break;
	}
}

void Key_Band_Jump(void)
{
	if(KB_OnOff != KB_PRESS)
		KB_STA |= 1<<14;
	if((KB_STA&1<<14) == 0)
		return;		
	if(KB_OnOff == KB_PRESS)
	{		
		delay_ms(20);
		if(KB_OnOff == KB_PRESS)
		{
			pwr_Mod[0] = pwr_Mod[3];
			tim_Mod[0] = tim_Mod[3];
			State = STATE_3;			
			TIM1->CNT = 0;
			Delay_STA &= 0xF<<5; Delay_STA |=  3<<5;	
			LCD_Disp(13,11,0,(pwr_Mod[0]-1)*3+tim_Mod[0]);
			cLED_Cmd(LED1,LED_RED);
			if((delay_Set == 0)||(delay_Set == DEFUALT_DELAY_TIME))
			{
				minute = DEFUALT_DELAY_TIME;
				half_Second = 255;
				delay_Set = DEFUALT_DELAY_TIME;
			}
		}
	}
	if(KB_Mode == KB_PRESS)			// 转到模式设置
	{
		sh_Set[2]= 13;g_Set[2]=11;sh_Set[1]= 0;g_Set[1]=1;
		LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);
		State = STATE_9;
		sub_State = S_STA_2;
		Quit_Sec(WAIT_TIME*2,BLK);
		hour=0;
		minute=0;					
		hour_Delay_Set = 0;
		minute_Delay_Set = 0;					
		pwr_Mod[0]=1;
		tim_Mod[0]=1;
		delay_Set = 0;
		while(KB_Mode == KB_PRESS);
	}
	else if(KB_Time== KB_PRESS)			// 转到延时设置
	{
		sh_Set[2]=0;g_Set[2]=0;sh_Set[1]=0;g_Set[1]=0;
		State = STATE_9;
		sub_State = S_STA_1;	//小时调节
		LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);
		Quit_Sec(WAIT_TIME*2,BLK);
		hour=0;
		minute=0;					
		hour_Delay_Set = 0;
		minute_Delay_Set = 0;					
		pwr_Mod[0]=3;
		tim_Mod[0]=3;
		delay_Set = 0;
		while(KB_Time== KB_PRESS);
	}
}

void Jump_To_Test(void)
{
	__disable_irq();
	JumpAddress = *(__IO u32*)(Test_APP_Address + 4);
	Jump_To_Application = (pFunction)JumpAddress;
	__set_MSP(*(__IO u32*)Test_APP_Address);
	Jump_To_Application();
}

void Mode_Sel(void)
{	
	if(SAVED_MODE == 0xFFFF)
	{
		LCD_Disp(0,0,0,0);
		delay_ms(1500);
		Jump_To_Test();
	}
	if(KB_OnOff == KB_PRESS)
	{
		delay_ms(1500);	
		delay_ms(1500);	
		if(KB_OnOff == KB_PRESS)
		{
			TR_Mode = 0xFFFF;
			MODE_Write();			
			BEEP_Cmd(b_ENABLE);
			delay_ms(1000);
			BEEP_Cmd(b_DISABLE);	
			LCD_Disp(0,0,0,0);
			delay_ms(1500);			
			while(KB_OnOff == KB_PRESS);
			Jump_To_Test();
		}
	}
}

