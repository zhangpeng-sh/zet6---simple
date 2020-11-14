#include "sys.h"	
u8 RTC_Buffer[3],g[3],sh[3],RTC_Set_Buffer[3],g_Set[3],sh_Set[3],sh1[3]={0xFF,0xFF,0xFF},g1[3]={0xFF,0xFF,0xFF};
void write_1302byte(u8 dat)                	//写一个字节的数据sck上升沿写数据                
{    
    u8 i = 0;    
    PBout(4)=0;  delay_us(50);  			//CLK
	for(i = 0;i < 8;i ++)  
	{    
		PBout(4)=0; 
		if(dat&0x01)  PBout(5) = 1;   else   PBout(5) = 0;
		delay_us(50);  PBout(4) = 1;   dat >>= 1;   delay_us(50);  
	}    
}   
void write_1302(u8 add,u8 dat) 		//向DS1302指定寄存器写入一个字节的数据 
{      
	PAout(15)=0;    				//RST		高为占用总线
	PBout(4)=0;		delay_us(50);   //时间大约50us,原先为2us
	PAout(15)=1;	delay_us(50);    
    write_1302byte(add);	write_1302byte(dat);             //先发地址  然后发数据    
	PBout(4)=0;		PAout(15)=0;    delay_us(50);    
}   
u8 read_1302(u8 add)                //从DS1302指定寄存器读数据  
{    
    u8 i=0,  Return_dat = 0x00;  
	PAout(15)=0; 	PBout(4)=0;    delay_us(50);			//略微延时50us
	PAout(15)=1;    delay_us(50);							//时间要大约50us 
    write_1302byte(add);									//先写寄存器的地址 
	GPIOB->CRL&=0XFF0FFFFF;	GPIOB->CRL|=0x00800000; 		//B5 in
    for(i=0;i<8;i++)    
    {    
		PBout(4)=1; 
        Return_dat >>= 1;  
        delay_us(50);    
		PBout(4)=0;//拉低时钟线，以便于数据的读入    
        if(PBin(5)==1)    //数据线此时为高电平 
        {Return_dat = Return_dat|0x80;}  
        delay_us(50); 		
    }    
	GPIOB->CRL&=0XFF0FFFFF;GPIOB->CRL|=0x00300000; GPIOB->ODR&=0XFFDF;	//B5 out
	delay_us(50);  	PAout(15)=0;	//释放总线   
	return Return_dat;    
}   
void ds1302_data()          	//从DS1302中读取出时间赋给myTime（硬件系统时间） 
{    
	u8 i=0;
	for(i=0;i<3;i++)    
    {    
        RTC_Buffer[i]=read_1302(0x81+2*i);          //读数据已经完成    
        g[i]=RTC_Buffer[i]%16;	sh[i]=RTC_Buffer[i]/16; 
		delay_us(500);
    }    
}
void ds1302_Set(void)
{
    write_1302(0x8e,0x00);                  //关闭写保护
	delay_ms(5); 
    write_1302(0x80,RTC_Set_Buffer[0]);	write_1302(0x82,RTC_Set_Buffer[1]);	write_1302(0x84,RTC_Set_Buffer[2]);
    write_1302(0x8e,0x80);
}
