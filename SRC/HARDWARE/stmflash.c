#include "stmflash.h"
#include "delay.h"
#include "timer.h"
#include "switch.h"
#include "ds1302.h"
#include "d5026.h"
   
u8 PARA_Buffer[20]={0x25,0x00,0x25,0x00,0x25,0x00,0x25,0x00,0x25,0x00,0x25,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03};
//PARA_Buffer[12], ʱ�������	PARA_Buffer[14], ռ�ձ�/���ʣ�	PARA_Buffer[15], ����ʱ�䣻	PARA_Buffer[16], �ֶ�ռ�ձ�/���ʣ�PARA_Buffer[17], �ֶ�����ʱ��

void STMFLASH_Unlock(void)
{
	FLASH->KEYR=0X45670123;//д���������.
	FLASH->KEYR=0XCDEF89AB;
}
//flash����
void STMFLASH_Lock(void)
{
	FLASH->CR|=1<<7;//����
}
u8 STMFLASH_GetStatus(void)
{	
	u32 res;		
	res=FLASH->SR; 
	if(res&(1<<0))return 1;		    //æ
	else if(res&(1<<2))return 2;	//��̴���
	else if(res&(1<<4))return 3;	//д��������
	return 0;						//�������
}
u8 STMFLASH_WaitDone(u16 time)
{
	u8 res;
	do
	{
		res=STMFLASH_GetStatus();
		if(res!=1)break;//��æ,����ȴ���,ֱ���˳�.
		delay_us(1);
		time--;
	 }while(time);
	 if(time==0)res=0xff;//TIMEOUT
	 return res;
} 	
	
u8 STMFLASH_ErasePage(u32 paddr)
{
	u8 res=0;
	res=STMFLASH_WaitDone(0X5FFF);//�ȴ��ϴβ�������,>20ms    
	if(res==0)
	{ 
		FLASH->CR|=1<<1;//ҳ����
		FLASH->AR=paddr;//����ҳ��ַ 
		FLASH->CR|=1<<6;//��ʼ����		  
		res=STMFLASH_WaitDone(0X5FFF);//�ȴ���������,>20ms  
		if(res!=1)//��æ
			FLASH->CR&=~(1<<1);//���ҳ������־.
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
		FLASH->CR|=1<<0;			//���ʹ��
		*(vu16*)faddr=dat;			//д������
		res=STMFLASH_WaitDone(0XFF);//�ȴ��������
		if(res!=1)//�����ɹ�
			FLASH->CR&=~(1<<0);	//���PGλ.
	} 
	return res;
} 
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		STMFLASH_WriteHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//��ַ����2.
	}  
} 

#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //�ֽ�
#else 
#define STM_SECTOR_SIZE	2048
#endif		 
u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//�����2K�ֽ�
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //������ַ
	u16 secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
	u16 secremain; //������ʣ���ַ(16λ�ּ���)	   
 	u16 i;    
	u32 offaddr;   //ȥ��0X08000000��ĵ�ַ
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//�Ƿ���ַ
	STMFLASH_Unlock();						//����
	offaddr=WriteAddr-STM32_FLASH_BASE;		//ʵ��ƫ�Ƶ�ַ.
	secpos=offaddr/STM_SECTOR_SIZE;			//������ַ  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//����ʣ��ռ��С   
	if(NumToWrite<=secremain)secremain=NumToWrite;//�����ڸ�������Χ
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			STMFLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//�����������
			for(i=0;i<secremain;i++)//����
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//д����������  
		}else STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;				//������ַ��1
			secoff=0;				//ƫ��λ��Ϊ0 	 
		   	pBuffer+=secremain;  	//ָ��ƫ��
			WriteAddr+=secremain;	//д��ַƫ��	   
		   	NumToWrite-=secremain;	//�ֽ�(16λ)���ݼ�
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//��һ����������д����
			else secremain=NumToWrite;//��һ����������д����
		}	 
	};	
	STMFLASH_Lock();//����
}
#endif

void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
		ReadAddr+=2;//ƫ��2���ֽ�.	
	}
}

void PARA_Write(void)   							//ģʽ1-9����
{	
	STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)PARA_Buffer,10);//д��10������ 
}

void MODE_Write(void)   							//������ʽ����
{	
	STMFLASH_Write(FLASH_MODE_ADDR, (u16*)(&TR_Mode),1);//д��1������ 
}

void Parameter_Load(void)							//�������أ������ֶ�ģʽ1���ֶ�ģʽ2���Զ�ģʽ���Լ���ʱʱ��
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

void timeSet_Flash_Write()					//���涨ʱ
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
