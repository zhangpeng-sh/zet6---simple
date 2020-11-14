#ifndef __SWITCH_H
#define __SWITCH_H	 
#include "sys.h" 


#define FAN PBout(10)
#define PMOS PBout(11)
#define MOC PBout(11)
#define MOC1 PAout(7)
#define RELAY PAout(5)

#define FAN_ON 1
#define FAN_OFF 0

extern u8	PWM_Flag,
			preheat_Flag,
			pwr_Mod[],
			tim_Mod[],
			set_Num,
			RTC_State,
			sub_State,
			State;
extern u16	time_Set[3],TR_Mode,pwr[3],Ctrl_STA;

#define FAN_ON 1
#define FAN_OFF 0

extern u8 RL_Flag;
			

void Switch_Init(void);//IO≥ı ºªØ
void Gen_Fan_CTRL(void);


#endif
