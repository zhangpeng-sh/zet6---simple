#ifndef __DS1302_H
#define __DS1302_H	 
#include "sys.h"

extern u8 RTC_Buffer[3],g[3],sh[3],RTC_Set_Buffer[3],g_Set[3],sh_Set[3];
	
void ds1302_data(void);
void ds1302_Set(void);

#endif

