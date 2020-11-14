#ifndef __DELAY_H
#define __DELAY_H 

#include "stm32F10x.h"

void Stm32_Clock_Init(void);
void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus); 
void IWDG_Init(u8 prer,u16 rlr);

#endif





























