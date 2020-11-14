#include "d5026.h"
#include "stdio.h"
#include "delay.h"
#include "timer.h"
#include "ds1302.h"
#include "remote.h"
#include "key.h"
#include "string.h"
#include "lcd.h"
#include "config.h"
#include "stmflash.h"
#include "switch.h"

#define GAP 6
#define HIGHT (GAP+16)
#define H_POS 24
#define V_POS 40
#define INIT 5

u8 Disp_Buffer[6]={0x11,0x11,0x11,0x11,0x00,0x00};
u8 Tab_1[] =  {0x77,0x06,0xb3,0x97,0xC6,0xD5,0xF5,0x07,0xF7,0xD7,0xE7,0xF4,0xB0,0xB6,0xF1,0xE1,0x80,0x00,0xF0,0x76,0x68,0xbb,0x70,0xC4,0xE6,0x9F}; 
				//数码管字模数组0~9，a~f. 16'-', 17' ', 18't',19'U',20,'1.',21,'2.',22'L',23'X',24,'H',25,'3.'

extern u8 	State,
			RTC_State,
			sub_State;
extern u16	Ctrl_STA;

u8 str[10]={0};

void d5026_init(void)   
{
	LCD_Disp(17,17,17,17);
	cLED_Cmd(LED1,OFF);
	cLED_Cmd(LED2,OFF);
	BEEP_Cmd(b_DISABLE);
	write_5026(Disp_Buffer);
}  

//写一个字节的数据sck上升沿写数据  
void write_5026(u8* arry)                               
{
    u8 i = 0, j = 0, dat;
    PAout(8)=1;  					//BLANK
    delay_us(2);//延时大约2us
	for(i = 0;i < 6;i ++)
	{   
		dat=arry[i];
		for(j = 0;j < 8;j ++)
		{    
			PBout(14)=0;   			//SCLK
			if((dat&0x80)==0x80)    
				PBout(13) = 1; 		//SIN
			else 
				PBout(13) = 0;
			delay_us(2);     
			PBout(14) = 1;   
			dat <<= 1;
			delay_us(2);
		}
	} 
	PBout(14) = 0;
	PBout(13) = 0;	
	delay_us(3);
	PBout(15) = 1;					//LAT
	delay_us(5);
	PBout(15) = 0;
	delay_us(5);	
	PAout(8) = 0;		
}

void LCD_Disp(u8 b1,u8 b2,u8 b3,u8 b4)
{
		Disp_Buffer[3]=Disp_Buffer[2]=Disp_Buffer[1]=Disp_Buffer[0]=0x00;
	if(b1<10&&b2<10&&b3<10&&b4<10)
		sprintf((char*)str,"%d%d:%d%d", b1, b2, b3, b4);
	else if(b1==13&&b2==11&&b3==17&&b4==17)
	{			
		Gui_DrawChar_GBK48(H_POS,V_POS,FC,BC,'D');
		Gui_DrawChar_GBK48(H_POS+24+5,V_POS,FC,BC,'B');
		return;
	}
	else if(b1==13&&b2==11&&b3!=17)	
	{		
		Gui_DrawChar_GBK48(H_POS,V_POS,FC,BC,'D');
		Gui_DrawChar_GBK48(H_POS+24+5,V_POS,FC,BC,'B');
		Gui_DrawChar_GBK48(H_POS+48+10,V_POS,FC,BC,b3+'0');
		Gui_DrawChar_GBK48(H_POS+72+15,V_POS,FC,BC,b4+'0');
		return;
	}
	else if(b1==17&&b2==17&b3==17&&b4==17)
		strcpy((char*)str,"         ");
	else if(b1==17&&b2==17&&b3==18)
		sprintf((char*)str,"  T%d", b4);
	else if(b1==17&&b2==17)
		sprintf((char*)str,"  :%d%d", b3, b4);
	else if(b3==17&&b4==17)
		sprintf((char*)str,"%d%d:  ", b1, b2);		
	Gui_DrawFont_GBK48(H_POS,V_POS,FC,BC,str);
}

void cLED_Cmd(u8 led, u8 color)
{
	if(led==LED1)
	{
		Disp_Buffer[5]&=~0x7;
		Disp_Buffer[5]|=color;
	}
	else
	{
		Disp_Buffer[5]&=~(7<<3);
		Disp_Buffer[5]|=color<<3;
	}
	write_5026(Disp_Buffer);
}
void BEEP_Cmd(u8 beep)
{	
	Disp_Buffer[4]= beep;
	write_5026(Disp_Buffer);
}
void Quit()			//退出到待机状态
{
	State = 0;	RTC_State = RTC_0;
	LCD_Disp(17,17,17,17);	cLED_Cmd(LED1,OFF);	cLED_Cmd(LED2,OFF);	BEEP_Cmd(b_DISABLE);
	Ctrl_STA &= ~1;
	PAout(5) = 0;	PAout(7) =0;	delay_us(4);	PBout(11) = 1;	delay_ms(1);	PBout(11) = 0;
	run_Half_Second = 0;
	write_5026(Disp_Buffer);	TIM1->CNT = 0;
	LCD_Fill(0,10,160,118,DARKBLUE);
}
void Quit_Sec(u8 t, u8 mode)			//分闪烁和常亮
{
	Delay_STA &= ~(0xF<<23); 	Delay_STA |= t<<23;
	Delay_STA &= ~(1<<16); 		Delay_STA |= mode;	
	Delay_STA &= ~(0x3F<<17);
	RTC_State = RTC_7;			TIM1->CNT = 0;
}
void blinkNWait(u8 sec)		//闪烁一次后暂停几秒
{
	LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);	delay_ms(50);	LCD_Disp(17,17,17,17);	delay_ms(50);
	LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);	TIM1->CNT = 0;	Delay_STA &= ~(0xF<<12);	Delay_STA |= 1<<12;
	switch(sec)
	{
		case 4:			delay_ms(1000);		TIM1->CNT = 0;
		case 3:			delay_ms(1000);		TIM1->CNT = 0;
		case 2:			delay_ms(1000);		TIM1->CNT = 0;
		case 1:			delay_ms(1000);		TIM1->CNT = 0;
		case 0:
			break;
	}
}

//遥控器+
void remote_Plus_Act(u8 seg)
{
	if((((Rmt_STA&0x00FF)<=1)&&((Rmt_VAL&1<<14)==0))||(((Rmt_STA&0x00FF)>5)&&cnt_Backup!=(Rmt_STA>>8)))
	{
		cnt_Backup=(Rmt_STA>>8);
		Rmt_VAL |= 1<<14;
		g_Set[seg]++;		
		if(g_Set[seg] >= 10)
		{
			g_Set[seg] = 0;
			sh_Set[seg]++;
		}
		if(seg == HOUR)
		{
			if(((sh_Set[seg] == 2)&&(g_Set[seg] >= 4))||(sh_Set[seg] >2))
			{
				sh_Set[seg]=0;
				g_Set[seg] = 0;
			}
		}
		if(seg == MIN)
		{
			if(((sh_Set[seg] == 5)&&(g_Set[seg] >= 9))||(sh_Set[seg] >5))
			{
				sh_Set[seg]=0;
				g_Set[seg] = 0;
			}
		}
		LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);							
		TIM1->CNT = 0;
		Delay_STA &= ~(0xF<<12);	Delay_STA |= 1<<12;									
	}
}

//遥控器-
void remote_Minus_Act(u8 seg)
{
	if((((Rmt_STA&0x00FF)<=1)&&((Rmt_VAL&1<<14)==0))||(((Rmt_STA&0x00FF)>5)&&cnt_Backup!=(Rmt_STA&0x00FF)))
	{
		cnt_Backup=(Rmt_STA&0x00FF);
		Rmt_VAL |= 1<<14;	
		if(g_Set[seg]--==0)
		{
			g_Set[seg]=9;
			if(sh_Set[seg]--==0)
			{
				if(seg == HOUR)
				{
					g_Set[seg]=3;
					sh_Set[seg]=2;
				}
				else
				{
					g_Set[seg]=9;
					sh_Set[seg]=5;
				
				}
			}
		}
		LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);	
		TIM1->CNT = 0;
		Delay_STA &= ~(0xF<<12);	Delay_STA |= 1<<12;									
	}
}

//遥控器 - NUM
void remote_Num(u8 num)		
{
	sh_Set[2]= 13;g_Set[2]=11;sh_Set[1]= 0;g_Set[1]=num;
	LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);
	write_5026(Disp_Buffer);
	Delay_STA &= ~(0x3F<<17);
	RTC_State = RTC_7;							
	TIM1->CNT = 0;
}

//按键延时调整
void KB_Delay_Adj(void)
{				
	minute_Delay_Set = g_Set[2]*60+sh_Set[1]*10;
	delay_Set = minute_Delay_Set*60;
	hour_Delay_Set = minute_Delay_Set/60;
	minute_Delay_Set = minute_Delay_Set % 60;
	if(KB_Plus == KB_PRESS)			//"+"
	{
		KB_STA++;
		Delay_STA &= ~(0x3F<<17);
		if(((KB_STA&0x7F)==1)||(KB_STA&0x7F)>10)
		{
			sh_Set[1]++;	
			if(sh_Set[1]>=6)
			{
				sh_Set[1]=0;
				g_Set[2]++;
				if(g_Set[2]>=10)
					g_Set[2]=0;							
			}								
		TIM1->CNT = 0;
			LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);
		}
		Delay_STA &= ~(0xF<<12);	Delay_STA |= 1<<12;	
		delay_ms(30);
	}
	else if(KB_Minus == KB_PRESS)			//"-"
	{
		KB_STA++;
		Delay_STA &= ~(0x3F<<17);
		if((KB_STA&0x7F)==1||(KB_STA&0x7F)>10)
		{
			if(sh_Set[1]--==0)
			{
				sh_Set[1]=5;
				if(g_Set[2]--==0)
				{
					g_Set[2]=9;
				}
			}										
			TIM1->CNT = 0;
			LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);
		}
		Delay_STA &= ~(0xF<<12);	Delay_STA |= 1<<12;	
		delay_ms(30);
	}
	else if(KB_Mode == KB_PRESS)			// 转到模式设置
	{			
		sh_Set[2]= 13;g_Set[2]=11;sh_Set[1]= 0;g_Set[1]=(pwr_Mod[0]-1)*3+tim_Mod[0];
		TIM1->CNT = 0;
		LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);
		sub_State = S_STA_2;
		Quit_Sec(WAIT_TIME*2,BLK);
		cLED_Cmd(LED1,BLUE);
		cLED_Cmd(LED2,OFF);
		while(KB_Mode == KB_PRESS);
	}
	
	else if(KB_OnOff == KB_PRESS)
	{
		delay_ms(10);
		if(KB_OnOff == KB_PRESS)
		{
			State = STATE_3;							
			TIM1->CNT = 0;
			Delay_STA &= 0xF<<5; Delay_STA |=  3<<5;	
			while(KB_OnOff == KB_PRESS);
			if((delay_Set == 0)||(delay_Set == DEFUALT_DELAY_TIME))
			{
				minute = DEFUALT_DELAY_TIME;
				half_Second = 255;
				delay_Set = DEFUALT_DELAY_TIME;
			}
		}
	}
	else 
		KB_STA &= 0xFF80;
}

void KB_Mode_Adj(void)
{
	pwr_Mod[0]=pwr_Mod[3]=(g_Set[1]-1)/3+1;
	tim_Mod[0]=tim_Mod[3]=(g_Set[1]-1)%3+1;
	if(KB_Plus == KB_PRESS)			//"+"
	{
		KB_STA++;
		Delay_STA &= ~(0x3F<<17);
		if(((KB_STA&0x007F)==1)||((KB_STA&0x007F)>10))
		{
			g_Set[1]++;	
			if(g_Set[1]>=10)
			{
				g_Set[1]=1;		
			}								
			TIM1->CNT = 0;
			LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);
		}
		Delay_STA &= ~(0xF<<12);	Delay_STA |= 1<<12;		
		delay_ms(30);
	}
	else if(KB_Minus == KB_PRESS)			//"-"
	{
		KB_STA++;		
		Delay_STA &= ~(0x3F<<17);
		if((KB_STA&0x7F)==1||(KB_STA&0x7F)>10)
		{
			g_Set[1]--;
			if(g_Set[1]==0)
			{
				g_Set[1]=9;
			}							
			TIM1->CNT = 0;
			LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);
		}
		Delay_STA &= ~(0xF<<12);	Delay_STA |= 1<<12;	
		delay_ms(30);
	}
	else if(KB_Time == KB_PRESS)					// 转到延时设置
	{
		sh_Set[2]= hour_Delay_Set/10;g_Set[2]=hour_Delay_Set;
		sh_Set[1]= minute_Delay_Set/10;g_Set[1]=minute_Delay_Set%10;							
		TIM1->CNT = 0;
		LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);
		sub_State = S_STA_1;				//小时调节
		Quit_Sec(WAIT_TIME*2,BLK);
		cLED_Cmd(LED1,OFF);
		cLED_Cmd(LED2,BLUE);
		while(KB_Time == KB_PRESS);
	}	
	else if(KB_OnOff == KB_PRESS)
	{
		delay_ms(10);
		if(KB_OnOff == KB_PRESS)
		{
			State = STATE_3;
			PARA_Buffer[18] = pwr_Mod[3];
			PARA_Buffer[19] = tim_Mod[3];
			PARA_Write();							
			TIM1->CNT = 0;
			Delay_STA &= 0xF<<5; Delay_STA |=  3<<5;	
			while(KB_OnOff == KB_PRESS);
			if((delay_Set == 0)||(delay_Set == DEFUALT_DELAY_TIME))
			{
				minute = DEFUALT_DELAY_TIME;
				half_Second = 255;
				delay_Set = DEFUALT_DELAY_TIME;
			}
		}
	}				
	else 
		KB_STA &= 0xFF80;
}
