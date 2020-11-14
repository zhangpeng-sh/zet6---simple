#ifndef __DB5026_H
#define __DB5026_H	 
#include "sys.h"

#define LED1 1
#define LED2 2

#define b_ENABLE 0xFF
#define b_DISABLE 0

//»­±ÊÑÕÉ« 
#define FC	0xFFFF
#define BC	0x001F

#define LED_RED           	0x01
#define LED_GREEN         	0x02
#define LED_BLUE         	0x04
#define	LED_PURPLE			0x05
#define	LED_WHITE			0x07
#define	OFF				0x00

#define MIN				0x01
#define HOUR			0x02

#define NBLK			0x00
#define BLK				0x01
							
extern u8 Disp_Buffer[6];

void d5026_init(void); 
void write_5026(u8*);
void LCD_Disp(u8 ,u8 ,u8 ,u8);
void cLED_Cmd(u8 , u8 );
void BEEP_Cmd(u8);
void Quit(void);
void Quit_Sec(u8,u8);
void blinkNWait(u8 );
void remote_Plus_Act(u8);
void remote_Minus_Act(u8);
void remote_Num(u8 num);
void KB_Delay_Adj(void);
void KB_Mode_Adj(void);
extern u8 key_Flag,cnt_Backup;
#endif

