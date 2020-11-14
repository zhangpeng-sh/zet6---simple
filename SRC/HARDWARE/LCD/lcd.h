#ifndef __LCD_H
#define __LCD_H		
#include "stm32F10x.h"
#include "sys.h"

#define USE_HORIZONTAL 3 //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1 
#define LCD_W 10
#define LCD_H 162

#else
#define LCD_W 160
#define LCD_H 128
#endif

//-----------------LCD端口定义---------------- 

#define LCD_SCL_Clr() PFout(0) = 0					//SCL=SCLK	PAout(0)
#define LCD_SCL_Set() PFout(0) = 1

#define LCD_SDA_Clr() PFout(1) = 0					//SDA=MOSI	PAout(1)
#define LCD_SDA_Set() PFout(1) = 1

#define LCD_RES_Clr()   PFout(2) = 0					//RES	PAout(2)
#define LCD_RES_Set()   PFout(2) = 1

#define LCD_DC_Clr()   PFout(3) = 0					//DC	PAout(3)
#define LCD_DC_Set()   PFout(3) = 1
 		     
#define LCD_CS_Clr()  PFout(4) = 0 				//CS1	PAout(4)
#define LCD_CS_Set()  PFout(4) = 1  

#define LCD_BL_Clr()  PFout(5)	= 0					//BLK
#define LCD_BL_Set()  PFout(5)	= 1

#define LCD_FSO        PFin(6)						//MISO  读取字库数据引脚	PAin(6)

#define LCD_CS2_Clr()    PFout(7)	= 0					//CS2 字库片选
#define LCD_CS2_Set()    PFout(7) = 1 	

void LCD_Writ_Bus(u8 dat);//模拟SPI时序
void LCD_WR_DATA8(u8 dat);//写入一个字节
void LCD_WR_DATA(u16 dat);//写入两个字节
void LCD_WR_REG(u8 dat);//写入一个指令
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//设置坐标函数
void LCD_Init(void);//LCD初始化

void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);//指定区域填充颜色
void LCD_DrawPoint(u16 x,u16 y,u16 color);//在指定位置画一个点
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color);//在指定位置画一条线
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);//在指定位置画一个矩形
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color);//在指定位置画一个圆

void LCD_ShowChinese(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示汉字串
void LCD_ShowChinese16x16(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示单个16x16汉字
void LCD_ShowChinese24x24(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示单个24x24汉字
void LCD_ShowChinese32x32(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode);//显示单个32x32汉字

void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey,u8 mode);//显示一个字符
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey,u8 mode);//显示字符串
u32 mypow(u8 m,u8 n);//求幂
void LCD_ShowIntNum(u16 x,u16 y,u16 num,u8 len,u16 fc,u16 bc,u8 sizey);//显示整数变量
void LCD_ShowFloatNum1(u16 x,u16 y,float num,u8 len,u16 fc,u16 bc,u8 sizey);//显示两位小数变量

void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width,const u8 pic[]);//显示图片

//字库操作函数
void ZK_command(u8 dat);
u8 get_data_from_ROM(void);
void get_n_bytes_data_from_ROM(u8 AddrHigh,u8 AddrMid,u8 AddrLow,u8 *pBuff,u8 DataLen);
void Display_GB2312(u16 x,u16 y,u8 zk_num,u16 fc,u16 bc);
void Display_GB2312_String(u16 x,u16 y,u8 zk_num,u8 text[],u16 fc,u16 bc);
void Display_Asc(u16 x,u16 y,u8 zk_num,u16 fc,u16 bc);
void Display_Asc_String(u16 x,u16 y,u16 zk_num,u8 text[],u16 fc,u16 bc);
void Display_Arial_TimesNewRoman(u16 x,u16 y,u8 zk_num,u16 fc,u16 bc);
void Display_Arial_String(u16 x,u16 y,u16 zk_num,u8 text[],u16 fc,u16 bc);
void Display_TimesNewRoman_String(u16 x,u16 y,u16 zk_num,u8 text[],u16 fc,u16 bc);

void Gui_DrawChar_GBK48(u16 x, u16 y, u16 fc, u16 bc, u8 k);
void Gui_DrawFont_GBK48(u16 x, u16 y, u16 fc, u16 bc, u8 *s);

//画笔颜色
#define WHITE         	 0xFFFF //11111 111111 11111
#define BLACK         	 0x0000	//00000 000000 00000 
#define BLUE           	 0x001F //00000 000000 11111
#define BRED             0XF81F	//11111 000000 11111
#define GRED 			 0XFFE0	//11111 111111 00000
#define GBLUE			 0X07FF //00000 111111 11111
#define RED           	 0xF800 //11111 000000 00000
#define MAGENTA       	 0xF81F //11111 000000 11111
#define GREEN         	 0x07E0 //00000 111111 00000
#define CYAN          	 0x7FFF //01111 111111 11111
#define YELLOW        	 0xFFE0 //11111 111111 00000
#define BROWN 			 0XBC40 //10111 100010 00000棕色
#define BRRED 			 0XFC07 //11111 100000 00111棕红色
#define GRAY  			 0X8430 //10000 100001 10000灰色
#define DARKBLUE      	 0x001F //0X01CF	//00000 001110 01111深蓝色
#define LIGHTBLUE      	 0X7D7C	//01111 101011 11100浅蓝色  
#define GRAYBLUE       	 0X5458 //01010 100010 11000灰蓝色
#define LIGHTGREEN     	 0X841F //10000 100000 11111浅绿色
#define LGRAY 			 0XC618 //11000 110000 11000浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 //10100 110010 10001浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //00101 011000 10010浅棕蓝色(选择条目的反色)

#endif





