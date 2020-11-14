#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

#define STATE_0 0
#define STATE_1 1
#define STATE_2 2
#define STATE_3 3
#define STATE_4 4
#define STATE_5 5
#define STATE_6 6
#define STATE_7 7
#define STATE_8 8
#define STATE_9 9
#define STATE_10 10

#define RTC_0 0
#define RTC_1 1
#define RTC_2 2
#define RTC_3 3
#define RTC_4 4
#define RTC_5 5
#define RTC_6 6
#define RTC_7 7
#define RTC_8 8

#define S_STA_0 0
#define S_STA_1 1
#define S_STA_2 2
#define S_STA_3 3
#define S_STA_4 4


extern u32	run_Half_Second,
			delay_Set,
			Delay_STA;

extern u16 	fan_Half_Second;

extern u8	hour_Set_Buffer[6],
			minute_Set_Buffer[6],
			hour_Set_Buf[6],
			minute_Set_Buf[6],
			H_Half_Second,
			quit_Set_Second,
			count,
			hour,minute,half_Second,
			hour_Delay_Set,
			minute_Delay_Set,
			RTC_State;

extern u8	vs[4],ct[4],sn[4];

void TIM_Init(void);
void fan_Duty_Set(u8 dut);
 
#endif
