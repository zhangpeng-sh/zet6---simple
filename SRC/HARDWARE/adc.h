#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"

#define OC_Compare	OC_Volt*4096/3.3
#define SC_Compare	SC_Volt*4096/3.3

void Adc_Init(void);
void OC_Detec(void);
void MOS_Detec(void);
 
#endif 
