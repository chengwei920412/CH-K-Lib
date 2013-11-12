/***********************************************************************************************
//CH_Kinetis������  V2.3
//����    :YANDLD 
//E-MAIL  :yandld@126.com
//�޸�����:2013/2/14
//�汾��V2.3
//�Ա���http://upcmcu.taobao.com
//QQ    1453363089
//Copyright(C) YANDLD 2012-2022
//All rights reserved
************************************************************************************************/
#include "wdog.h"
#include "sys.h"
/***********************************************************************************************
 ���ܣ����Ź���ʼ��
 �βΣ�ms: ���Ź���λ���ʱ�� ��λΪms  
 ���أ�0
 ��⣺���Ź���ʼ��
************************************************************************************************/
void WDOG_Init(u16 ms)
{
	u32 Value;
	WDOG->UNLOCK=0xC520u;
	WDOG->UNLOCK=0xD928u;  //���Ź�Կ�ף��ο�k10�ֲ�504ҳ��
	//������󲿷�ѡ��Ĭ������,ʱ����ԴΪbus clock��48mhz
	WDOG->PRESC=WDOG_PRESC_PRESCVAL(7);//���÷�Ƶ��������ֵΪ8��������Ƶ���μ��ֲ�506ҳ
	Value=CPUInfo.BusClock/8;            //BusClock8��Ƶ
  Value=((Value/1000)*ms);
	WDOG->TOVALH=(Value&0xFFFF0000)>>16;
	WDOG->TOVALL=(Value&0x0000FFFF)>>0;
	WDOG->STCTRLH|=0x0001u;//�������Ź����ο�k10�ֲ�501ҳ��
}
/***********************************************************************************************
 ���ܣ��رտ��Ź� 
 �βΣ�0
 ���أ�0
 ��⣺�رտ��Ź� 
************************************************************************************************/
void WDOG_Close(void)
{
	WDOG->UNLOCK=0xC520u;
	WDOG->UNLOCK=0xD928u;  //���Ź�Կ�ף��ο�k10�ֲ�504ҳ��
	WDOG->STCTRLH&=~0x0001u;//�رտ��Ź����ο�k10�ֲ�501ҳ��
}
/***********************************************************************************************
 ���ܣ��������Ź�
 �βΣ�0
 ���أ�0
 ��⣺�������Ź�
************************************************************************************************/
void WDOG_Open(void)
{
	WDOG->UNLOCK=0xC520u;
	WDOG->UNLOCK=0xD928u;  //���Ź�Կ�ף��ο�k10�ֲ�504ҳ��
	WDOG->STCTRLH|=0x0001u;//�򿪿��Ź����ο�k10�ֲ�501ҳ��
}
/***********************************************************************************************
 ���ܣ����Ź�ι��
 �βΣ�0
 ���أ�0
 ��⣺���Ź�ι��
************************************************************************************************/
void WDOG_Feed(void)
{
	WDOG->REFRESH=0xA602u; 
	WDOG->REFRESH=0xB480u; //ι���Ź����ο�k10�ֲ�504ҳ��
}