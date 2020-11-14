#include "sys.h"

void Draw_RoundRect()							//画圆角矩形
{
	int a,b,i;
	a=0;b=10;	  
	while(a<=b)
 	{
		LCD_Address_Set(10-a,10-b,150+a,10-b);
		for(i=10-a;i<=150+a;i++)
			LCD_WR_DATA(0xFF00);
		LCD_Address_Set(10-a,118+b,150+a,118+b);
		for(i=10-a;i<=150+a;i++)
			LCD_WR_DATA(0xFF00);
		LCD_Address_Set(10-b,10-a,150+b,10-a);
		for(i=10-b;i<=150+b;i++)
			LCD_WR_DATA(0xFF00);
		LCD_Address_Set(10-b,118+a,150+b,118+a);
		for(i=10-b;i<=150+b;i++)
			LCD_WR_DATA(0xFF00);	
		a++;
		if((a*a+b*b)>100)//判断要画的点是否过远
			b--;
	}	
}

void Bresenham_Line(u16,u16,u16,u16,u16);

int main(void)
{
	Periphery_Init();							//外设初始化
	LCD_Fill(1,1,160,128,0xFF00);
	Bresenham_Line(50,25,100,50,0);
	while(1)
	{	
	}
		
}		//main
