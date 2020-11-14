#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 


#define KB_Plus PAin(0)  		//PA0
#define KB_OnOff PAin(1)		//PA1 		
#define KB_Time PAin(2)			//PA2
#define KB_Minus PAin(3)		//PA3  
#define KB_CH2O PAin(4)			//PA4
#define KB_Mode PBin(12)		//PB12

#define KB_Plus_PRES 	1		//KEY0按下
#define KB_OnOff_PRES	2		//KEY1按下
#define KB_Minus_PRES	3		//KEY2按下
#define KB_Time_PRES  	4		//KEY_UP按下(即WK_UP/KEY_UP)
#define KB_CH2O_PRES   	5
#define KB_Mode_PRES	6

//#define KB_PRESS	0 			//老面板1 ，新面板0
//#define KB_UP		1

extern u16	KB_STA;


void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8);  	//按键扫描函数	
void my_KEY_Prcss(void);
void Key_Band_Jump(void);
void Mode_Sel(void);
#endif
