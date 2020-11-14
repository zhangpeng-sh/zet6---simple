#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////////////////////////
//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 64 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1              //ʹ��FLASHд��(0��������;1��ʹ��)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
//FLASH������ֵ

#define FLASH_SAVE_ADDR  0X0800B000		//����FLASH �����ַ(����Ϊż��������ֵҪ���ڱ�������ռ��FLASH�Ĵ�С+0X08000000)
#define SAVED_NUM	*(vu16*)(FLASH_SAVE_ADDR+12)		//�����ʱ�����
#define SAVED_AUTO	*(vu16*)(FLASH_SAVE_ADDR+14)		//�����ģʽ
#define SAVED_MANU	*(vu16*)(FLASH_SAVE_ADDR+16)		//������ֶ�ģʽ
#define SAVED_MANU2	*(vu16*)(FLASH_SAVE_ADDR+18)		//����İ������ֶ�ģʽ

#define FLASH_MODE_ADDR	0X0800C000
#define SAVED_MODE	*(vu16*)(FLASH_MODE_ADDR)

#define Test_APP_Address 0x08007000

extern u8 	PARA_Buffer[20];
 

u16 STMFLASH_ReadHalfWord(u32 faddr);		  //��������  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//ָ����ַ��ʼ��ȡָ����������
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����

//����д��
void Test_Write(u32 WriteAddr,u16 WriteData);
void PARA_Read(void);
void PARA_Write(void);
void MODE_Write(void);
void Parameter_Load(void);
void timeSet_Flash_Write(void);

#endif

