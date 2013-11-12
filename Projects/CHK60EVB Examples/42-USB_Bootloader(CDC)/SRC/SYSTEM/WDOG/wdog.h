/***********************************************************************************************
//CH_Kinetis驱动库  V2.3
//作者    :YANDLD 
//E-MAIL  :yandld@126.com
//修改日期:2013/2/14
//版本：V2.3
//淘宝：http://upcmcu.taobao.com
//QQ    1453363089
//Copyright(C) YANDLD 2012-2022
//All rights reserved
************************************************************************************************/
#ifndef __WDOG_H__
#define __WDOG_H__
#include "sys.h"

//看门狗驱动头文件
typedef unsigned short u16;
void WDOG_Init(u16 ms); //看门狗初始化 ，并开启看门狗
void WDOG_Open(void);  //开启看门狗
void WDOG_Close(void);  //关闭看门狗
void WDOG_Feed(void);   //喂狗,防止系统重新启动
#endif
