#ifndef __CONFIG_H
#define __CONFIG_H

#define WAIT_TIME	4					//退出等待单位时间
#define PLAY_SEC 	2					//下一段等待时间

#define DEFUALT_DELAY_TIME 10			//手动开启延时时间

#define D_TIME 1		//继电器延时秒数
		
#define PMOS_ON 	0	// 0交流控制板(老)  1 直流控制板（新）
#define PMOS_OFF 	1

#define KB_PRESS	0 	// 1老显示板，0新显示板
#define KB_UP		1

#define RL_ON 		1	//	0常闭继电器（老） 1常开继电器（新）	
#define RL_OFF		0

#define OC_Volt		2.5		//过流电压
#define SC_Volt		0.8		//短路电压
#define SC_Time		1.5		//短路延迟时间

#define VERSERL		{19,25,2,4 }		//V 3. 2 4			版本号
#define CONTAB		{22,23,23,17}	//L X X				硬件
#define SERLNUM		{0,0,0,1}		//0001				序列号

#define DUTY_DEFAULT 100				//风扇占空比

void GPIO_Config(void); 

#endif
