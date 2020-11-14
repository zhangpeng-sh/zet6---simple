#include "remote.h"
#include "delay.h"
#include "d5026.h"
#include "timer.h"
#include "config.h"
#include "stmflash.h"
#include "switch.h"
#include "ds1302.h"

u8	cnt_Backup=0;
u16 Rmt_VAL = 0;		//[14]key_Flag	[13]press_Flag [7:0]key
u16 Rmt_STA = 0;		//[15]	同步标志 [14]处理标志	[13] 上升沿捕获标志 [11:8] 溢出计数器 [7:0] 连按计数器
	
u32 data;				//4字节红外数据
void Remote_Init(void)	//外部中断3和定时器3
{
	RCC->APB2ENR |= 9; 	NVIC->IP[9] = 0x00; NVIC->ISER[0] = 1<<9;
	AFIO->EXTICR[0] |= 0x1000; 	EXTI->IMR = 0x0008;	EXTI->RTSR = 0x0008; EXTI->FTSR = 0x0008;
	RCC->APB1ENR |= 1<<1;	NVIC->IP[29]=0x30;	NVIC->ISER[0]= 1<<29;
	TIM3->ARR = 19999;	TIM3->PSC = 71;	TIM3->DIER = 0x01;	TIM3->CR1 |= 0x01;	//20ms
}

void EXTI3_IRQHandler(void)
{	
	u16 Dval;				//下降沿时计数器的值		
	EXTI->PR = 1<<3;  				//清除LINE3上的中断标志位	
	if(PBin(3))//上升沿
	{
		TIM3->CNT = 0;
		Rmt_STA |= 1<<13;	
	}	
	else	//下降沿
	{
		Dval = TIM3->CNT;	
		if(Rmt_STA & (1<<13))
		{
			if(Dval>4200&&Dval<4700)						//450为标准值，4.5ms	判断同步码
				Rmt_STA = 1<<15;								//标记成功接收到了同步码
			else if(Rmt_STA & 1<<15)							//开始接收数据及判断连发码
			{			
				if(Dval>300&&Dval<800)						//56为标准值,560us
				{
					data<<=1;								//左移一位.
					data|=0;								//接收到0	   
				}
				else if(Dval>1400&&Dval<1800)				//168为标准值,1680us
				{
					data<<=1;								//左移一位.
					data|=1;								//接收到1
				}
				else if(Dval>2000&&Dval<3200)				//连发码 250为标准值2.5ms
				{
					Rmt_STA &= 0xF0FF;
					Rmt_STA++; 							//连发计数增加1次	
				}
			}
		}
		Rmt_STA &= ~(1<<13);
	}		
}


//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断			计时达到200ms，认为按键弹起
{
	TIM3->SR = 0;
	if(Rmt_STA&1<<15)
	{
		Rmt_STA |= 1<<14;
		Rmt_STA &= ~(1<<13);
		Rmt_STA += 0x0100;
		if( (Rmt_STA & 0x0F00) > 0x0B00)
			Rmt_STA = 0;
	}
}

u8 Remote_Scan(void)					//处理4字节红外数据
{
	u8 sta=0;
    u8 t1,t2;
	if(Rmt_STA & 1<<14)									//有数据需要处理
	{
	    t1=data>>24;									//得到地址码
	    t2=(data>>16)&0xff;								//得到地址反码 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)				//检验遥控识别码(ID)及地址 
	    {
	        t1=data>>8;
	        t2=data;
	        if(t1==(u8)~t2)
				sta=t1;									//键值正确
		}
		if(sta==0||!(Rmt_STA&(1<<15)))						//按键数据错误/遥控已经没有按下了
		{
		 	Rmt_STA=0;
			data = 0;
		}
	}
    return sta;
}

u8 my_Remote_Scan(u8 mode)
{
	u8 dat=0;
	static u8 up_Flag=1;		
	if(mode)
			up_Flag = 1;
	
	if((Remote_Scan()!=0)&&(up_Flag == 1))
	{
		dat = Remote_Scan();
		up_Flag = 0;
	}
	else if(Remote_Scan()==0)
		up_Flag = 1;
	return dat;	
}

void IR_H_Det(void)
{
	if((PBin(7)!=0x00)&& (run_Half_Second>=20)&&!(Delay_STA&1))			//关闭 3个条件，
	{
		Rmt_VAL |= 1<<15;
		Ctrl_STA &= ~1;
		PAout(5) = 0;
	}
}

void ReSet_Check(void)
{
	static u8 	P_Word[6]={KEY_2,KEY_5,KEY_8,KEY_2,KEY_5,KEY_8}, 
				V_Word[6]={KEY_1,KEY_2,KEY_3,KEY_7,KEY_8,KEY_9},
				i_pass = 0,
				j_pass = 0;
	u8 key;
//	key = Remote_Scan();	
	if((key!=0)&&(key!=V_Word[j_pass-1])&&((State == 0)||(State == 7)))		//版本号指令判断
	{
		if(key==V_Word[j_pass])
		{
			j_pass++;
			if(j_pass == 6)
			{
				LCD_Disp(13,11,0,9);
				BEEP_Cmd(b_ENABLE);				//蜂鸣器开
				write_5026(Disp_Buffer);
				delay_ms(100);
				BEEP_Cmd(b_DISABLE);				//蜂鸣器关
				LCD_Disp(17,17,17,17);
				write_5026(Disp_Buffer);				
				delay_ms(1000);
				State = STATE_8;        //版本号显示
				RTC_State = RTC_6;
				TIM1->CNT = 0;
				Delay_STA &= ~(0xF<<12);	Delay_STA |= 3<<12;
				LCD_Disp(vs[0],vs[1],vs[2],vs[3]);
			}
		}
		else
		{
			j_pass=0;
		}
	}
	
	if((key!=0)&&(key!=P_Word[i_pass-1]))		//软启动指令判断
	{
		if(key==P_Word[i_pass])
		{
			i_pass++;
			if(i_pass == 3)
			{
				__set_FAULTMASK(1);				//关中断
				NVIC_SystemReset();				//复位
			}
		}
		else
			i_pass=0;
	}
}

void Remote_Jump(void)
{
	u8 j;
	switch(my_Remote_Scan(0))
	{
		case KEY_power:
			State = STATE_1;			//自动运行
			tim_Mod[0] = tim_Mod[1];
			pwr_Mod[0] = pwr_Mod[1];
			while(my_Remote_Scan(1)==KEY_power);
			sub_State = S_STA_0;
			fan_Half_Second = 60;
			LCD_Disp(13,11,0,(pwr_Mod[0]-1)*3+tim_Mod[0]);			
			Delay_STA &= 0xF<<5; Delay_STA |=  4<<5;	
			TIM1->CNT = 0;
			for(j=0;j<4;j++)			//设置好比较数组
			{
				hour_Set_Buffer[j] = PARA_Buffer[2*j];
				minute_Set_Buffer[j] = PARA_Buffer[2*j+1];
			}
			break;
		case KEY_mode:			//手动运行 第一键
			State = STATE_2;
			sub_State = S_STA_0;
			sh_Set[2] = 13,g_Set[2] = 11,sh_Set[1] = 17,g_Set[1] = 17;
			LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);
			delay_Set = DEFUALT_DELAY_TIME;		
			Quit_Sec(WAIT_TIME,NBLK);
			break;
		case KEY_menu:			// RTC调节   ************
			State = STATE_6;
			sub_State = S_STA_0;
			TIM1->CNT = 0;
			LCD_Disp(sh[2],g[2],sh[1],g[1]);		//显示实际时间
			RTC_State = RTC_2;
			cLED_Cmd(LED2,LED_RED);
			TIM1->CNT = 0;
			Delay_STA &= ~(0xF<<12);	Delay_STA |= 1<<12;
			break;
		case KEY_mute:				//转定时设置
			State = STATE_5;
			sub_State = S_STA_0;
			Quit_Sec(WAIT_TIME,BLK);
			set_Num = 0;
			cLED_Cmd(LED2,LED_BLUE);
			sh_Set[2] = 17;	g_Set[2] = 17;sh_Set[1] = 18;g_Set[1] = 1;
			LCD_Disp(17,17,18,1);
			for(j=0;j<4;j++)			//设置好赋值数组和显示数组
			{
				hour_Set_Buf[j]=PARA_Buffer[2*j];
				minute_Set_Buf[j]=PARA_Buffer[2*j+1];
				hour_Set_Buffer[j] = 0x25;
				minute_Set_Buffer[j] = 0x00;
			}
			break;
			Remote_NumKey_Act
	}
	switch(Remote_Scan())
	{
		case KEY_1:
		case KEY_2:
		case KEY_3:
		case KEY_4:
		case KEY_5:
		case KEY_6:
		case KEY_7:
		case KEY_8:
		case KEY_9:
			sub_State = S_STA_1;
			Quit_Sec(WAIT_TIME*2,BLK);
			State = 7;
			
	}
}
