#ifndef __REMOTE_H
#define __REMOTE_H	 
#include "sys.h"

#define IRPort 	PBin(3)	 	//红外数据输入脚 
#define IR_H 	PBin(7)	 	//红外感应输入脚 

/************************************************
#define KEY_plus 		2
#define KEY_minus 		152
#define KEY_forward 	144
#define KEY_backward 	224
#define KEY_play 		168
#define KEY_test 		34
#define KEY_menu 		226
#define KEY_power		162
#define KEY_return		194
#define KEY_c			176
#define KEY_0 			104
#define KEY_1 			48
#define KEY_2 			24
#define KEY_3 			122
#define KEY_4 			16
#define KEY_5			56
#define KEY_6 			90
#define KEY_7 			66
#define KEY_8			74
#define KEY_9			82
************************************************/

#define KEY_menu		98
#define KEY_plus 		2
#define KEY_minus 		152
#define KEY_forward 	144
#define KEY_backward 	224
#define KEY_play 		168
#define KEY_mode 		34
#define KEY_mute 		226
#define KEY_power		162
#define KEY_return		194
#define KEY_ok			176
#define KEY_0 			104
#define KEY_1 			48
#define KEY_2 			24
#define KEY_3 			122
#define KEY_4 			16
#define KEY_5			56
#define KEY_6 			90
#define KEY_7 			66
#define KEY_8			74
#define KEY_9			82
#define KEY_NULL		0

#define Remote_NumKey_Act	case KEY_1:	remote_Num(1);	break;\
							case KEY_2:	remote_Num(2);	break;\
							case KEY_3:	remote_Num(3);	break;\
							case KEY_4:	remote_Num(4);	break;\
							case KEY_5:	remote_Num(5);	break;\
							case KEY_6:	remote_Num(6);	break;\
							case KEY_7:	remote_Num(7);	break;\
							case KEY_8:	remote_Num(8);	break;\
							case KEY_9:	remote_Num(9);	break;

#define REMOTE_ID 0

extern u8 key_Flag,cnt_Backup;
extern u16 Rmt_STA, Rmt_VAL;
void Remote_Init(void);//外部中断初始化		
u8 Remote_Scan(void);
u8 my_Remote_Scan(u8);
void IR_H_Init(void);
void IR_H_Det(void);
void ReSet_Check(void);
void Remote_Jump(void);

#endif

