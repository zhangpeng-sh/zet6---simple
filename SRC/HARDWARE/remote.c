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
u16 Rmt_STA = 0;		//[15]	ͬ����־ [14]�����־	[13] �����ز����־ [11:8] ��������� [7:0] ����������
	
u32 data;				//4�ֽں�������
void Remote_Init(void)	//�ⲿ�ж�3�Ͷ�ʱ��3
{
	RCC->APB2ENR |= 9; 	NVIC->IP[9] = 0x00; NVIC->ISER[0] = 1<<9;
	AFIO->EXTICR[0] |= 0x1000; 	EXTI->IMR = 0x0008;	EXTI->RTSR = 0x0008; EXTI->FTSR = 0x0008;
	RCC->APB1ENR |= 1<<1;	NVIC->IP[29]=0x30;	NVIC->ISER[0]= 1<<29;
	TIM3->ARR = 19999;	TIM3->PSC = 71;	TIM3->DIER = 0x01;	TIM3->CR1 |= 0x01;	//20ms
}

void EXTI3_IRQHandler(void)
{	
	u16 Dval;				//�½���ʱ��������ֵ		
	EXTI->PR = 1<<3;  				//���LINE3�ϵ��жϱ�־λ	
	if(PBin(3))//������
	{
		TIM3->CNT = 0;
		Rmt_STA |= 1<<13;	
	}	
	else	//�½���
	{
		Dval = TIM3->CNT;	
		if(Rmt_STA & (1<<13))
		{
			if(Dval>4200&&Dval<4700)						//450Ϊ��׼ֵ��4.5ms	�ж�ͬ����
				Rmt_STA = 1<<15;								//��ǳɹ����յ���ͬ����
			else if(Rmt_STA & 1<<15)							//��ʼ�������ݼ��ж�������
			{			
				if(Dval>300&&Dval<800)						//56Ϊ��׼ֵ,560us
				{
					data<<=1;								//����һλ.
					data|=0;								//���յ�0	   
				}
				else if(Dval>1400&&Dval<1800)				//168Ϊ��׼ֵ,1680us
				{
					data<<=1;								//����һλ.
					data|=1;								//���յ�1
				}
				else if(Dval>2000&&Dval<3200)				//������ 250Ϊ��׼ֵ2.5ms
				{
					Rmt_STA &= 0xF0FF;
					Rmt_STA++; 							//������������1��	
				}
			}
		}
		Rmt_STA &= ~(1<<13);
	}		
}


//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�			��ʱ�ﵽ200ms����Ϊ��������
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

u8 Remote_Scan(void)					//����4�ֽں�������
{
	u8 sta=0;
    u8 t1,t2;
	if(Rmt_STA & 1<<14)									//��������Ҫ����
	{
	    t1=data>>24;									//�õ���ַ��
	    t2=(data>>16)&0xff;								//�õ���ַ���� 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)				//����ң��ʶ����(ID)����ַ 
	    {
	        t1=data>>8;
	        t2=data;
	        if(t1==(u8)~t2)
				sta=t1;									//��ֵ��ȷ
		}
		if(sta==0||!(Rmt_STA&(1<<15)))						//�������ݴ���/ң���Ѿ�û�а�����
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
	if((PBin(7)!=0x00)&& (run_Half_Second>=20)&&!(Delay_STA&1))			//�ر� 3��������
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
	if((key!=0)&&(key!=V_Word[j_pass-1])&&((State == 0)||(State == 7)))		//�汾��ָ���ж�
	{
		if(key==V_Word[j_pass])
		{
			j_pass++;
			if(j_pass == 6)
			{
				LCD_Disp(13,11,0,9);
				BEEP_Cmd(b_ENABLE);				//��������
				write_5026(Disp_Buffer);
				delay_ms(100);
				BEEP_Cmd(b_DISABLE);				//��������
				LCD_Disp(17,17,17,17);
				write_5026(Disp_Buffer);				
				delay_ms(1000);
				State = STATE_8;        //�汾����ʾ
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
	
	if((key!=0)&&(key!=P_Word[i_pass-1]))		//������ָ���ж�
	{
		if(key==P_Word[i_pass])
		{
			i_pass++;
			if(i_pass == 3)
			{
				__set_FAULTMASK(1);				//���ж�
				NVIC_SystemReset();				//��λ
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
			State = STATE_1;			//�Զ�����
			tim_Mod[0] = tim_Mod[1];
			pwr_Mod[0] = pwr_Mod[1];
			while(my_Remote_Scan(1)==KEY_power);
			sub_State = S_STA_0;
			fan_Half_Second = 60;
			LCD_Disp(13,11,0,(pwr_Mod[0]-1)*3+tim_Mod[0]);			
			Delay_STA &= 0xF<<5; Delay_STA |=  4<<5;	
			TIM1->CNT = 0;
			for(j=0;j<4;j++)			//���úñȽ�����
			{
				hour_Set_Buffer[j] = PARA_Buffer[2*j];
				minute_Set_Buffer[j] = PARA_Buffer[2*j+1];
			}
			break;
		case KEY_mode:			//�ֶ����� ��һ��
			State = STATE_2;
			sub_State = S_STA_0;
			sh_Set[2] = 13,g_Set[2] = 11,sh_Set[1] = 17,g_Set[1] = 17;
			LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);
			delay_Set = DEFUALT_DELAY_TIME;		
			Quit_Sec(WAIT_TIME,NBLK);
			break;
		case KEY_menu:			// RTC����   ************
			State = STATE_6;
			sub_State = S_STA_0;
			TIM1->CNT = 0;
			LCD_Disp(sh[2],g[2],sh[1],g[1]);		//��ʾʵ��ʱ��
			RTC_State = RTC_2;
			cLED_Cmd(LED2,LED_RED);
			TIM1->CNT = 0;
			Delay_STA &= ~(0xF<<12);	Delay_STA |= 1<<12;
			break;
		case KEY_mute:				//ת��ʱ����
			State = STATE_5;
			sub_State = S_STA_0;
			Quit_Sec(WAIT_TIME,BLK);
			set_Num = 0;
			cLED_Cmd(LED2,LED_BLUE);
			sh_Set[2] = 17;	g_Set[2] = 17;sh_Set[1] = 18;g_Set[1] = 1;
			LCD_Disp(17,17,18,1);
			for(j=0;j<4;j++)			//���úø�ֵ�������ʾ����
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
