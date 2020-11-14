#include "stmflash.h"
#include "delay.h"
#include "timer.h"
#include "switch.h"
#include "ds1302.h"
#include "d5026.h"
   
u8 PARA_Buffer[20]={0x25,0x00,0x25,0x00,0x25,0x00,0x25,0x00,0x25,0x00,0x25,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03};
//PARA_Buffer[12], 时间段数；	PARA_Buffer[14], 占空比/功率；	PARA_Buffer[15], 工作时间；	PARA_Buffer[16], 手动占空比/功率；PARA_Buffer[17], 手动工作时间

void STMFLASH_Unlock(void)
{
	FLASH->KEYR=0X45670123;//写入解锁序列.
	FLASH->KEYR=0XCDEF89AB;
}
//flash上锁
void STMFLASH_Lock(void)
{
	FLASH->CR|=1<<7;//上锁
}
u8 STMFLASH_GetStatus(void)
{	
	u32 res;		
	res=FLASH->SR; 
	if(res&(1<<0))return 1;		    //忙
	else if(res&(1<<2))return 2;	//编程错误
	else if(res&(1<<4))return 3;	//写保护错误
	return 0;						//操作完成
}
u8 STMFLASH_WaitDone(u16 time)
{
	u8 res;
	do
	{
		res=STMFLASH_GetStatus();
		if(res!=1)break;//非忙,无需等待了,直接退出.
		delay_us(1);
		time--;
	 }while(time);
	 if(time==0)res=0xff;//TIMEOUT
	 return res;
} 	
	
u8 STMFLASH_ErasePage(u32 paddr)
{
	u8 res=0;
	res=STMFLASH_WaitDone(0X5FFF);//等待上次操作结束,>20ms    
	if(res==0)
	{ 
		FLASH->CR|=1<<1;//页擦除
		FLASH->AR=paddr;//设置页地址 
		FLASH->CR|=1<<6;//开始擦除		  
		res=STMFLASH_WaitDone(0X5FFF);//等待操作结束,>20ms  
		if(res!=1)//非忙
			FLASH->CR&=~(1<<1);//清除页擦除标志.
	}
	return res;
} 
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}
#if STM32_FLASH_WREN

u8 STMFLASH_WriteHalfWord(u32 faddr, u16 dat)
{
	u8 res;	   	    
	res=STMFLASH_WaitDone(0XFF);	 
	if(res==0)//OK
	{
		FLASH->CR|=1<<0;			//编程使能
		*(vu16*)faddr=dat;			//写入数据
		res=STMFLASH_WaitDone(0XFF);//等待操作完成
		if(res!=1)//操作成功
			FLASH->CR&=~(1<<0);	//清除PG位.
	} 
	return res;
} 
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		STMFLASH_WriteHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//地址增加2.
	}  
} 

#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else 
#define STM_SECTOR_SIZE	2048
#endif		 
u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//最多是2K字节
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //扇区地址
	u16 secoff;	   //扇区内偏移地址(16位字计算)
	u16 secremain; //扇区内剩余地址(16位字计算)	   
 	u16 i;    
	u32 offaddr;   //去掉0X08000000后的地址
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//非法地址
	STMFLASH_Unlock();						//解锁
	offaddr=WriteAddr-STM32_FLASH_BASE;		//实际偏移地址.
	secpos=offaddr/STM_SECTOR_SIZE;			//扇区地址  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//在扇区内的偏移(2个字节为基本单位.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//扇区剩余空间大小   
	if(NumToWrite<=secremain)secremain=NumToWrite;//不大于该扇区范围
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			STMFLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//擦除这个扇区
			for(i=0;i<secremain;i++)//复制
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//写入整个扇区  
		}else STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;				//扇区地址增1
			secoff=0;				//偏移位置为0 	 
		   	pBuffer+=secremain;  	//指针偏移
			WriteAddr+=secremain;	//写地址偏移	   
		   	NumToWrite-=secremain;	//字节(16位)数递减
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//下一个扇区还是写不完
			else secremain=NumToWrite;//下一个扇区可以写完了
		}	 
	};	
	STMFLASH_Lock();//上锁
}
#endif

void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}

void PARA_Write(void)   							//模式1-9保存
{	
	STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)PARA_Buffer,10);//写入10个半字 
}

void MODE_Write(void)   							//启动方式保存
{	
	STMFLASH_Write(FLASH_MODE_ADDR, (u16*)(&TR_Mode),1);//写入1个半字 
}

void Parameter_Load(void)							//参数加载：包括手动模式1和手动模式2，自动模式，以及定时时间
{
	if(SAVED_NUM == 0xFFFF)
		PARA_Buffer[12] = set_Num;
	else
	{		
		set_Num=PARA_Buffer[12];		STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)PARA_Buffer,10);
	}
	if(SAVED_AUTO == 0xFFFF)
	{
		PARA_Buffer[14] = pwr_Mod[1];	PARA_Buffer[15] = tim_Mod[1];
	}
	else			
	{
		pwr_Mod[1]=PARA_Buffer[14];		tim_Mod[1]=PARA_Buffer[15];
	}
	if(SAVED_MANU == 0xFFFF)
	{
		PARA_Buffer[16] = pwr_Mod[2];	PARA_Buffer[17] = tim_Mod[2];
	}
	else			
	{
		pwr_Mod[2]=PARA_Buffer[16];		tim_Mod[2]=PARA_Buffer[17];
	}
	if(SAVED_MANU2 == 0xFFFF)
	{
		PARA_Buffer[18] = pwr_Mod[3];	PARA_Buffer[19] = tim_Mod[3];
	}
	else			
	{
		pwr_Mod[3]=PARA_Buffer[18];		tim_Mod[3]=PARA_Buffer[19];
	}
}

void timeSet_Flash_Write()					//保存定时
{
	u8 j;
	Delay_STA &= ~(0x3F<<17);	Delay_STA &= ~(1<<16);
	cLED_Cmd(LED2,LED_BLUE);	LCD_Disp(17,17,17,17);		write_5026(Disp_Buffer);	delay_ms(40);
	for(j=0;j<4;j++)
	{
		PARA_Buffer[2*j]=hour_Set_Buffer[j];	PARA_Buffer[2*j+1]=minute_Set_Buffer[j];
	}
	PARA_Write();
	sh_Set[2]=18,g_Set[2]=1,sh_Set[1]=18,g_Set[1]=set_Num;	LCD_Disp(sh_Set[2],g_Set[2],sh_Set[1],g_Set[1]);
}
