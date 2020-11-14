#ifndef __CONFIG_H
#define __CONFIG_H

#define WAIT_TIME	4					//�˳��ȴ���λʱ��
#define PLAY_SEC 	2					//��һ�εȴ�ʱ��

#define DEFUALT_DELAY_TIME 10			//�ֶ�������ʱʱ��

#define D_TIME 1		//�̵�����ʱ����
		
#define PMOS_ON 	0	// 0�������ư�(��)  1 ֱ�����ư壨�£�
#define PMOS_OFF 	1

#define KB_PRESS	0 	// 1����ʾ�壬0����ʾ��
#define KB_UP		1

#define RL_ON 		1	//	0���ռ̵������ϣ� 1�����̵������£�	
#define RL_OFF		0

#define OC_Volt		2.5		//������ѹ
#define SC_Volt		0.8		//��·��ѹ
#define SC_Time		1.5		//��·�ӳ�ʱ��

#define VERSERL		{19,25,2,4 }		//V 3. 2 4			�汾��
#define CONTAB		{22,23,23,17}	//L X X				Ӳ��
#define SERLNUM		{0,0,0,1}		//0001				���к�

#define DUTY_DEFAULT 100				//����ռ�ձ�

void GPIO_Config(void); 

#endif
