#include "lcd.h"
#include "delay.h"
void LCD_Writ_Bus(u8 dat) 			//LCD串行数据写入函数
{	
	u8 i;
	LCD_CS_Clr();
	for(i=0;i<8;i++)
	{			  
		LCD_SCL_Clr();
		if(dat&0x80)	LCD_SDA_Set();	else	LCD_SDA_Clr();
		LCD_SCL_Set();		dat<<=1;
	}	
	LCD_CS_Set();	
}
void LCD_WR_REG(u8 dat)				//LCD写入命令
{
	LCD_DC_Clr();	LCD_Writ_Bus(dat);	LCD_DC_Set();
}
void LCD_WR_DATA8(u8 dat)			//LCD写入8位数据
{
	LCD_Writ_Bus(dat);
}
void LCD_WR_DATA(u16 dat)			//LCD写入16位数据
{
	LCD_Writ_Bus(dat>>8);	LCD_Writ_Bus(dat);
}
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)		//设置起始和结束地址
{
		LCD_WR_REG(0x2a);		LCD_WR_DATA(x1);		LCD_WR_DATA(x2);	//列地址设置
		LCD_WR_REG(0x2b);		LCD_WR_DATA(y1+1);		LCD_WR_DATA(y2+1);	//行地址设置
		LCD_WR_REG(0x2c);													//储存器写
}
void LCD_Init(void)					//显示屏的初始化
{
	RCC->APB2ENR |= 3<<2;
	LCD_RES_Clr();	delay_ms(100);	LCD_RES_Set();	delay_ms(100);							//复位
	LCD_BL_Set();	delay_ms(100);															//打开背光
	//************* Start Initial Sequence **********//
	LCD_WR_REG(0x11); 	delay_ms(120);              //Sleep out  
	//------------------------------------ST7735S Frame Rate-----------------------------------------// 
	LCD_WR_REG(0xB1); 	LCD_WR_DATA8(0x05); 	LCD_WR_DATA8(0x3C); 	LCD_WR_DATA8(0x3C); 
	LCD_WR_REG(0xB2); 	LCD_WR_DATA8(0x05);		LCD_WR_DATA8(0x3C); 	LCD_WR_DATA8(0x3C); 
	LCD_WR_REG(0xB3); 	LCD_WR_DATA8(0x05); 	LCD_WR_DATA8(0x3C); 	LCD_WR_DATA8(0x3C); 	LCD_WR_DATA8(0x05); 	LCD_WR_DATA8(0x3C); 	LCD_WR_DATA8(0x3C); 
	LCD_WR_REG(0xB4);	LCD_WR_DATA8(0x03); 		 //Dot inversion 	
	//------------------------------------ST7735S Power Sequence---------------------------------// 
	LCD_WR_REG(0xC0); 	LCD_WR_DATA8(0x28); 	LCD_WR_DATA8(0x08); 	LCD_WR_DATA8(0x04); 
	LCD_WR_REG(0xC1); 	LCD_WR_DATA8(0XC0); 
	LCD_WR_REG(0xC2); 	LCD_WR_DATA8(0x0D); 	LCD_WR_DATA8(0x00); 
	LCD_WR_REG(0xC3); 	LCD_WR_DATA8(0x8D); 	LCD_WR_DATA8(0x2A); 
	LCD_WR_REG(0xC4); 	LCD_WR_DATA8(0x8D); 	LCD_WR_DATA8(0xEE); 
	LCD_WR_REG(0xC5); 	LCD_WR_DATA8(0x1A); 						//VCOM 
	LCD_WR_REG(0x36);	LCD_WR_DATA8(0xA0);  						//MX, MY, RGB mode 		00	70	C0	A0
	//------------------------------------ST7735S Gamma Sequence---------------------------------// 
	LCD_WR_REG(0xE0); 
	LCD_WR_DATA8(0x04); 	LCD_WR_DATA8(0x22); 	LCD_WR_DATA8(0x07); 	LCD_WR_DATA8(0x0A); 	LCD_WR_DATA8(0x2E); 	LCD_WR_DATA8(0x30); 	LCD_WR_DATA8(0x25); 	LCD_WR_DATA8(0x2A); 
	LCD_WR_DATA8(0x28); 	LCD_WR_DATA8(0x26); 	LCD_WR_DATA8(0x2E); 	LCD_WR_DATA8(0x3A); 	LCD_WR_DATA8(0x00); 	LCD_WR_DATA8(0x01); 	LCD_WR_DATA8(0x03); 	LCD_WR_DATA8(0x13);
 	LCD_WR_REG(0xE1); 
	LCD_WR_DATA8(0x04);		LCD_WR_DATA8(0x16); 	LCD_WR_DATA8(0x06); 	LCD_WR_DATA8(0x0D); 	LCD_WR_DATA8(0x2D);		LCD_WR_DATA8(0x26); 	LCD_WR_DATA8(0x23); 	LCD_WR_DATA8(0x27); 
	LCD_WR_DATA8(0x27); 	LCD_WR_DATA8(0x25); 	LCD_WR_DATA8(0x2D); 	LCD_WR_DATA8(0x3B); 	LCD_WR_DATA8(0x00); 	LCD_WR_DATA8(0x01); 	LCD_WR_DATA8(0x04); 	LCD_WR_DATA8(0x13); 
	//------------------------------------End ST7735S Gamma Sequence-----------------------------// 
	LCD_WR_REG(0x3A);		LCD_WR_DATA8(0x05);  //65k mode 	
	LCD_WR_REG(0x29); //Display on 
} 

void LCD_DrawPoint(u16 x,u16 y,u16 color)					//在指定位置画点
{
	LCD_Address_Set(x,y,x,y);	LCD_WR_DATA(color);
} 
void Bresenham_Line(u16 x0, u16 y0, u16 x1, u16 y1, u16 color)
{
	int dx, dy, x, y, e, i;
	x=x0; y=y0;
	dx = x1 - x0;	dy = y1 - y0;	e = 2*dy - dx;
	for(i=1;i<=dx;i++)
	{
		LCD_DrawPoint(x,y,color);
		if(e>=0)
		{
			y++;
			e += 2*dy-2*dx;
		}
		else
			e += 2*dy;
		x++;
	}
}
void LCD_Fill(u16 x0, u16 y0, u16 x1, u16 y1, u16 color)
{
	u16 i,j;
	LCD_Address_Set(x0, y0, x1, y1);
	for(i=x0;i<=x1;i++)
		for(j=y0;j<=y1;j++)
			LCD_WR_DATA(color);
}
