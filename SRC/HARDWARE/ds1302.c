#include "sys.h"	
u8 RTC_Buffer[3],g[3],sh[3],RTC_Set_Buffer[3],g_Set[3],sh_Set[3],sh1[3]={0xFF,0xFF,0xFF},g1[3]={0xFF,0xFF,0xFF};
void write_1302byte(u8 dat)                	//дһ���ֽڵ�����sck������д����                
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
void write_1302(u8 add,u8 dat) 		//��DS1302ָ���Ĵ���д��һ���ֽڵ����� 
{      
	PAout(15)=0;    				//RST		��Ϊռ������
	PBout(4)=0;		delay_us(50);   //ʱ���Լ50us,ԭ��Ϊ2us
	PAout(15)=1;	delay_us(50);    
    write_1302byte(add);	write_1302byte(dat);             //�ȷ���ַ  Ȼ������    
	PBout(4)=0;		PAout(15)=0;    delay_us(50);    
}   
u8 read_1302(u8 add)                //��DS1302ָ���Ĵ���������  
{    
    u8 i=0,  Return_dat = 0x00;  
	PAout(15)=0; 	PBout(4)=0;    delay_us(50);			//��΢��ʱ50us
	PAout(15)=1;    delay_us(50);							//ʱ��Ҫ��Լ50us 
    write_1302byte(add);									//��д�Ĵ����ĵ�ַ 
	GPIOB->CRL&=0XFF0FFFFF;	GPIOB->CRL|=0x00800000; 		//B5 in
    for(i=0;i<8;i++)    
    {    
		PBout(4)=1; 
        Return_dat >>= 1;  
        delay_us(50);    
		PBout(4)=0;//����ʱ���ߣ��Ա������ݵĶ���    
        if(PBin(5)==1)    //�����ߴ�ʱΪ�ߵ�ƽ 
        {Return_dat = Return_dat|0x80;}  
        delay_us(50); 		
    }    
	GPIOB->CRL&=0XFF0FFFFF;GPIOB->CRL|=0x00300000; GPIOB->ODR&=0XFFDF;	//B5 out
	delay_us(50);  	PAout(15)=0;	//�ͷ�����   
	return Return_dat;    
}   
void ds1302_data()          	//��DS1302�ж�ȡ��ʱ�丳��myTime��Ӳ��ϵͳʱ�䣩 
{    
	u8 i=0;
	for(i=0;i<3;i++)    
    {    
        RTC_Buffer[i]=read_1302(0x81+2*i);          //�������Ѿ����    
        g[i]=RTC_Buffer[i]%16;	sh[i]=RTC_Buffer[i]/16; 
		delay_us(500);
    }    
}
void ds1302_Set(void)
{
    write_1302(0x8e,0x00);                  //�ر�д����
	delay_ms(5); 
    write_1302(0x80,RTC_Set_Buffer[0]);	write_1302(0x82,RTC_Set_Buffer[1]);	write_1302(0x84,RTC_Set_Buffer[2]);
    write_1302(0x8e,0x80);
}
