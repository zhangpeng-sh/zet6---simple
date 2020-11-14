#include "lcd.h"
u8 FontBuf[130],FontBuf1[130];//字库缓存	
void ZK_command(u8 dat)		//向字库写入命令
{
	u8 i; 
	for(i=0;i<8;i++)
	{			  
		LCD_SCL_Clr();
		if(dat&0x80>>i)	LCD_SDA_Set();	else	LCD_SDA_Clr();
		LCD_SCL_Set();
	}	
 }
u8 get_data_from_ROM(void)		//从字库读取数据
{
	u8 i, ret_data=0;
	for(i=0;i<8;i++)
	{
		LCD_SCL_Clr();
		ret_data<<=1;
		if(LCD_FSO)		ret_data++;
		LCD_SCL_Set(); 
	}
	return ret_data;
} 
void get_n_bytes_data_from_ROM(u8 AddrHigh,u8 AddrMid,u8 AddrLow,u8 *pBuff,u8 DataLen)	//读取N个数据
{
 	u8 i;
	LCD_CS2_Clr(); //字库片选
	ZK_command(0x03);//写指令
	ZK_command(AddrHigh);	ZK_command(AddrMid);	ZK_command(AddrLow);//写地址高中低字节
	for(i=0;i<DataLen;i++)	   *(pBuff+i) =get_data_from_ROM();//读一个字节数据
	LCD_CS2_Set();//取消字库片选
}


void Display_GB2312(u16 x,u16 y,u8 zk_num,u16 fc,u16 bc)	//显示汉字
{
	u8 i,k;
	switch(zk_num)
	{
		case 1:
			LCD_Address_Set(x,y,x+15,y+11);
			for(i=0;i<24;i++)		 
				for(k=0;k<8;k++)
				{	if((FontBuf[i]&(0x80>>k))!=0)	LCD_WR_DATA(fc);	else	LCD_WR_DATA(bc);}
			break;  // 12*12 
		case 2: 
			LCD_Address_Set(x,y,x+15,y+15);
			for(i=0;i<32;i++)		 
				for(k=0;k<8;k++)
				{	if((FontBuf[i]&(0x80>>k))!=0)	LCD_WR_DATA(fc);	else	LCD_WR_DATA(bc);}
			break;     // 15*16 
	    case 3:  
			LCD_Address_Set(x,y,x+23,y+23);
			for(i=0;i<72;i++)		 
				for(k=0;k<8;k++)
				{	if((FontBuf[i]&(0x80>>k))!=0)	LCD_WR_DATA(fc);	else	LCD_WR_DATA(bc);}					
			break;     // 24*24  
	    case 4:  
			LCD_Address_Set(x,y,x+31,y+31);
			for(i=0;i<128;i++)		 
				for(k=0;k<8;k++)
				{	if((FontBuf[i]&(0x80>>k))!=0)	LCD_WR_DATA(fc);	else	LCD_WR_DATA(bc);}   
			break;    // 32*32  
	}
}
void Display_Asc(u16 x,u16 y,u8 zk_num,u16 fc,u16 bc)			//显示ASCII码 6种字体,从字库读出显示
{
	unsigned char i,k;
	switch(zk_num)
	{
		case 1:   
			LCD_Address_Set(x,y,x+7,y+7);
			for(i=0;i<7;i++)		 
				for(k=0;k<8;k++)
				{	if((FontBuf[i]&(0x80>>k))!=0)	LCD_WR_DATA(fc);	else	LCD_WR_DATA(bc);}
			break;//5x7 ASCII
	 	case 2:   	
			LCD_Address_Set(x,y,x+7,y+7);
			for(i=0;i<8;i++)		 
				for(k=0;k<8;k++)
				{ 	if((FontBuf[i]&(0x80>>k))!=0)	LCD_WR_DATA(fc);	else	LCD_WR_DATA(bc);}
			break;   //	  7x8 ASCII
	   case 3:   
			LCD_Address_Set(x,y,x+7,y+11);
			for(i=0;i<12;i++)		 
				for(k=0;k<8;k++)
				{	if((FontBuf[i]&(0x80>>k))!=0)	LCD_WR_DATA(fc);	else	LCD_WR_DATA(bc);}
			break;  //  6x12 ASCII
		 case 4:  
			LCD_Address_Set(x,y,x+7,y+15);
			for(i=0;i<16;i++)		 
				for(k=0;k<8;k++)
				{	if((FontBuf[i]&(0x80>>k))!=0)	LCD_WR_DATA(fc);	else	LCD_WR_DATA(bc);}
			break;     //  8x16 ASCII
	    case 5:  
			LCD_Address_Set(x,y,x+15,y+24);
			for(i=0;i<48;i++)		 
				for(k=0;k<8;k++)
				{	if((FontBuf[i]&(0x80>>k))!=0)	LCD_WR_DATA(fc);	else	LCD_WR_DATA(bc);}
			break;    //  12x24 ASCII
	    case 6:  	 
			LCD_Address_Set(x,y,x+15,y+31);
			for(i=0;i<64;i++)		 
				for(k=0;k<8;k++)
				{	if((FontBuf[i]&(0x80>>k))!=0)	LCD_WR_DATA(fc);	else	LCD_WR_DATA(bc);}
			break;   //  16x32 ASCII
	}
}

void Display_Arial_TimesNewRoman(u16 x,u16 y,u8 zk_num,u16 fc,u16 bc)	//显示ASCII码(Arial&Times New Roman)	zk_num   1:6*12,  2:8*16,  3:12*24,  4:16*32
{
	unsigned char i,k;
	switch(zk_num)
	{
	   case 1:
			LCD_Address_Set(x,y,x+15,y+12);
			for(i=2;i<26;i++)		 
				for(k=0;k<8;k++)
				{	if((FontBuf[i]&(0x80>>k))!=0)	LCD_WR_DATA(fc);	else	LCD_WR_DATA(bc);}			 
			break;  //  6x12 ASCII
		case 2:  
			LCD_Address_Set(x,y,x+15,y+17);
			for(i=2;i<34;i++)		 
				for(k=0;k<8;k++)
				{	if((FontBuf[i]&(0x80>>k))!=0)	LCD_WR_DATA(fc);	else	LCD_WR_DATA(bc);}
			break;     //  8x16 ASCII
	    case 3:  
			LCD_Address_Set(x,y,x+23,y+23);
			for(i=2;i<74;i++)		 
				for(k=0;k<8;k++)
				{	if((FontBuf[i]&(0x80>>k))!=0)	LCD_WR_DATA(fc);	else	LCD_WR_DATA(bc);}
			break;    //  12x24 ASCII
	    case 4:  	 
			LCD_Address_Set(x,y,x+31,y+31);
			for(i=2;i<130;i++)		 
				for(k=0;k<8;k++)
				{	if((FontBuf[i]&(0x80>>k))!=0)	LCD_WR_DATA(fc);	else	LCD_WR_DATA(bc);}
			break;   //  16x32 ASCII
	}
}
void Display_GB2312_String(u16 x,u16 y,u8 zk_num,u8 text[],u16 fc,u16 bc)		//显示汉字
{
	u8 n,d,i= 0, AddrHigh,AddrMid,AddrLow ; //字高、中、低地址
	u32 FontAddr=0, BaseAdd=0; //字库基地址
	switch(zk_num)
	{		// n个数d：字间距
		case 1 :  BaseAdd=0x00;    n=24;  d=12; break;  // 12*12  
		case 2 :  BaseAdd=0x2C9D0; n=32;  d=16; break;   // 15*16  
		case 3 :  BaseAdd=0x68190; n=72;  d=24; break;   // 24*24  
		case 4 :  BaseAdd=0xEDF00; n=128; d=32; break;   // 32*32  
	}
	while((text[i]>0x00))
	{
		if(((text[i]>=0xA1)&&(text[i]<=0xA9))&&(text[i+1]>=0xA1))				//国标简体（GB2312）汉字在 字库IC中的地址由以下公式来计算：			
		{																		//Address = ((MSB - 0xA1) * 94 + (LSB - 0xA1))*n+ BaseAdd; 分三部取地址///
			FontAddr = (text[i]- 0xA1)*94; 
			FontAddr += (text[i+1]-0xA1);
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);			
			AddrHigh = (FontAddr&0xff0000)>>16;  								//地址的高8位,共24位//
			AddrMid = (FontAddr&0xff00)>>8;      								//地址的中8位,共24位//
			AddrLow = FontAddr&0xff;	     									//地址的低8位,共24位//
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );		//取一个汉字的数据，存到"FontBuf[]"
			Display_GB2312(x,y,zk_num,fc,bc);									//显示一个汉字到LCD上/ 
		}
		else if(((text[i]>=0xB0) &&(text[i]<=0xF7))&&(text[i+1]>=0xA1))			//国标简体（GB2312） 字库IC中的地址由以下公式来计算：
		{																		//Address = ((MSB - 0xB0) * 94 + (LSB - 0xA1)+846)*n+ BaseAdd; 分三部取地址	
			FontAddr = (text[i]- 0xB0)*94; 
			FontAddr += (text[i+1]-0xA1)+846;
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);			
			AddrHigh = (FontAddr&0xff0000)>>16;  								//地址的高8位,共24位//
			AddrMid = (FontAddr&0xff00)>>8;      								//地址的中8位,共24位//
			AddrLow = FontAddr&0xff;	    									//地址的低8位,共24位//
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );		//取一个汉字的数据，存到"FontBuf[ ]"
			Display_GB2312(x,y,zk_num,fc,bc);//显示一个汉字到LCD上/
		}
		x+=d; //下一个字坐标
		i+=2;  //下个字符
	}
}  
void Display_Asc_String(u16 x,u16 y,u16 zk_num,u8 text[],u16 fc,u16 bc)	//显示ASCII码	zk_num   1:5*7   2:5*8   3:6*12,  4:8*16,  5:12*24,  6:16*32
{
	u8 i= 0, n,d, AddrHigh,AddrMid,AddrLow ; //字高、中、低地址
	u32 FontAddr=0, BaseAdd=0; //字库基地址	
	switch(zk_num)
	{ 													//n个数，d:字间距
		case 1:  BaseAdd=0x1DDF80; n=8;  d=6;  break;	 //	  5x7 ASCII
		case 2:  BaseAdd=0x1DE280; n=8;  d=8;  break;	 //   7x8 ASCII
		case 3:  BaseAdd=0x1DBE00; n=12; d=6;  break;	 //  6x12 ASCII
		case 4:  BaseAdd=0x1DD780; n=16; d=8;  break;	 //  8x16 ASCII	
		case 5:  BaseAdd=0x1DFF00; n=48; d=12; break;	 //  12x24 ASCII
		case 6:  BaseAdd=0x1E5A50; n=64; d=16; break;	 //  16x32 ASCII
	}
	while((text[i]>0x00))
	{	
		if((text[i] >= 0x20) &&(text[i] <= 0x7E))
		{						
			FontAddr = 	text[i]-0x20;
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
			AddrHigh = (FontAddr&0xff0000)>>16;  /*地址的高8位,共24位*/
			AddrMid = (FontAddr&0xff00)>>8;      /*地址的中8位,共24位*/
			AddrLow = FontAddr&0xff;	     /*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*取一个汉字的数据，存到"FontBuf[]"*/
			Display_Asc(x,y,zk_num,fc,bc);/*显示一个ascii到LCD上 */
		}
		i++;  //下个数据
		x+=d;//下一个字坐标 
	}
}
void Display_Arial_String(u16 x,u16 y,u16 zk_num,u8 text[],u16 fc,u16 bc)			//显示ASCII(Arial类型)
{
	u8 i= 0, AddrHigh,AddrMid,AddrLow ; 				//字高、中、低地址
	u32 FontAddr=0, BaseAdd=0; 							//字地址、字库基地址	
	u8 n,d;												// 不同点阵字库的计算变量
	switch(zk_num)										//n:个数，d:字间距
	{
		case 1:  BaseAdd=0x1DC400; n=26;  d=8;  break;	 //  8x12 ASCII(Arial类型)
		case 2:  BaseAdd=0x1DE580; n=34;  d=12; break;	 //  12x16 ASCII(Arial类型)
		case 3:  BaseAdd=0x1E22D0; n=74;  d=16; break;	 //  16x24 ASCII(Arial类型)
		case 4:  BaseAdd=0x1E99D0; n=130; d=24; break;	 //  24x32 ASCII(Arial类型)
	}
	while((text[i]>0x00))
	{	
		if((text[i] >= 0x20) &&(text[i] <= 0x7E))
		{						
			FontAddr = 	text[i]-0x20;
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
			AddrHigh = (FontAddr&0xff0000)>>16;  			/*地址的高8位,共24位*/
			AddrMid = (FontAddr&0xff00)>>8;      			/*地址的中8位,共24位*/
			AddrLow = FontAddr&0xff;	     				/*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*取一个汉字的数据，存到"FontBuf[]"*/
			Display_Arial_TimesNewRoman(x,y,zk_num,fc,bc);/*显示一个ascii到LCD上 */
		}
		i++;  												//下个数据
		x+=d;												//下一个字坐标 
	}
}
void Display_TimesNewRoman_String(u16 x,u16 y,u16 zk_num,u8 text[],u16 fc,u16 bc)			//显示ASCII(Arial类型)
{
	u8 i= 0, AddrHigh,AddrMid,AddrLow ; 				//字高、中、低地址
	u32 FontAddr=0, BaseAdd=0; 							//字地址、字库基地址	
	u8 n,d;												// 不同点阵字库的计算变量
	switch(zk_num)
	{													//n:个数，d:字间距						
		case 1:  BaseAdd=0x1DCDC0; n=26;  d=8;  break;	 //  6x12 ASCII(TimesNewRoman类型)
		case 2:  BaseAdd=0x1DF240; n=34;  d=12;  break;	 //  12x16 ASCII	(TimesNewRoman类型)
		case 3:  BaseAdd=0x1E3E90; n=74;  d=16; break;	 //  12x24 ASCII(TimesNewRoman类型)
		case 4:  BaseAdd=0x1ECA90; n=130; d=24; break;	 //  16x32 ASCII(TimesNewRoman类型)
	}
	while((text[i]>0x00))
	{	
		if((text[i] >= 0x20) &&(text[i] <= 0x7E))
		{						
			FontAddr = 	text[i]-0x20;
			FontAddr = (unsigned long)((FontAddr*n)+BaseAdd);
			AddrHigh = (FontAddr&0xff0000)>>16;  		/*地址的高8位,共24位*/
			AddrMid = (FontAddr&0xff00)>>8;      		/*地址的中8位,共24位*/
			AddrLow = FontAddr&0xff;	     			/*地址的低8位,共24位*/
			get_n_bytes_data_from_ROM(AddrHigh,AddrMid,AddrLow,FontBuf,n );/*取一个汉字的数据，存到"FontBuf[]"*/
			Display_Arial_TimesNewRoman(x,y,zk_num,fc,bc);/*显示一个ascii到LCD上 */
		}
		i++;  //下个数据
		x+=d;//下一个字坐标 
	}
}
