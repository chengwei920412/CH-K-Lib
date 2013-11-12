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
#ifndef __SYS_H__
#define __SYS_H__
#include <MK60DZ10.h>
#include "wdog.h"
//频率设置
#define CLOCK_SETUP     (0)	 
/* 预定义的时钟设置
   0 ... 内部晶振
	       Core/System Clock: 96M 
				 Bus         Clock: 48M
				 FlexBus     Clock: 48M
				 Flash       Clock: 24M
   1 ... 外部8M晶振 
	       Core/System Clock: 96M 
				 Bus         Clock: 48M
				 FlexBus     Clock: 48M
				 Flash       Clock: 24M
   2 ... 外部8M晶振 超频
	       Core/System Clock: 200M 
				 Bus         Clock: 100M
				 FlexBus     Clock: 100M
				 Flash       Clock: 25M
   3 ... 外部50M有源晶振
	       Core/System Clock: 96M 
				 Bus         Clock: 48M
				 FlexBus     Clock: 48M
				 Flash       Clock: 24M
   4 ... 外部50M有源晶振 超频
	       Core/System Clock: 200M 
				 Bus         Clock: 100M
				 FlexBus     Clock: 100M
				 Flash       Clock: 25M			 
*/
//是否支持uCOSII

#define SYSTEM_SUPPORT_OS		(0)		

/*
定义系统文件夹是否支持UCOS
   0 ... 不支持uCOS
	 1 ... 支持  uCOS
*/

//是否用串口打印调试信息
#define DEBUG_PRINT         (0)
//定义printf打印口
#define DEBUG_UART_PORT     UART4       //指定那个串口作为printf输出口
#define DEBUG_UART_BAUD     (115200)    //printf 波特率
//重定义常用数据变量类型
typedef signed long  s32;
typedef signed short s16;
typedef signed char  s8;
typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;
//常用常量及宏定义
#ifndef TRUE
	#define TRUE		(1)
#endif
#ifndef FLASE
	#define FALSE		(0)
#endif
#ifndef NULL
	#define NULL 0
#endif
//位  设置 清除 查询
#define SET_BIT(BitNumber, Register)        (Register |=(1<<BitNumber))
#define CLR_BIT(BitNumber, Register)        (Register &=~(1<<BitNumber))
#define CHK_BIT(BitNumber, Register)        (Register & (1<<BitNumber))
//求最大值和最小值 
#define  MAX( x, y ) ( ((x) > (y)) ? (x) : (y) ) 
#define  MIN( x, y ) ( ((x) < (y)) ? (x) : (y) ) 
//得到一个结构体中field所占用的字节数 
#define FSIZ( type, field ) sizeof( ((type *) 0)->field ) 
//得到一个变量的地址（word宽度） 
#define  B_PTR( var )  ( (byte *) (void *) &(var) ) 
#define  W_PTR( var )  ( (word *) (void *) &(var) ) 
//返回数组元素的个数 
#define  ARR_SIZE( a )  ( sizeof( (a) ) / sizeof( (a[0]) ) ) 

#if (CLOCK_SETUP == 0)
    #define CPU_XTAL_CLK_HZ                 4000000u  //时钟发生器频率Hz 
    #define CPU_XTAL32k_CLK_HZ              32768u    //时钟发生器频率Hz 
    #define CPU_INT_SLOW_CLK_HZ             32768u    //慢速内部时钟振荡器频率 Hz 
    #define CPU_INT_FAST_CLK_HZ             4000000u  //快速内部时钟振荡器频率 Hz
    #define DEFAULT_SYSTEM_CLOCK            96000000u //默认系统时钟频率
#elif (CLOCK_SETUP == 1)
    #define CPU_XTAL_CLK_HZ                 8000000u  //时钟发生器频率Hz 
    #define CPU_XTAL32k_CLK_HZ              32768u    //时钟发生器频率Hz /
    #define CPU_INT_SLOW_CLK_HZ             32768u    //慢速内部时钟振荡器频率 Hz 
    #define CPU_INT_FAST_CLK_HZ             4000000u  //快速内部时钟振荡器频率 Hz
    #define DEFAULT_SYSTEM_CLOCK            96000000u //默认系统时钟频率
#elif (CLOCK_SETUP == 2)
    #define CPU_XTAL_CLK_HZ                 8000000u  //时钟发生器频率Hz 
    #define CPU_XTAL32k_CLK_HZ              32768u    //时钟发生器频率Hz /
    #define CPU_INT_SLOW_CLK_HZ             32768u    //慢速内部时钟振荡器频率 Hz 
    #define CPU_INT_FAST_CLK_HZ             4000000u  //快速内部时钟振荡器频率 Hz
    #define DEFAULT_SYSTEM_CLOCK            200000000u //默认系统时钟频率
#elif (CLOCK_SETUP == 3)
    #define CPU_XTAL_CLK_HZ                 50000000u  //时钟发生器频率Hz 
    #define CPU_XTAL32k_CLK_HZ              32768u    //时钟发生器频率Hz /
    #define CPU_INT_SLOW_CLK_HZ             32768u    //慢速内部时钟振荡器频率 Hz 
    #define CPU_INT_FAST_CLK_HZ             4000000u  //快速内部时钟振荡器频率 Hz
    #define DEFAULT_SYSTEM_CLOCK            96000000u //默认系统时钟频率
#elif (CLOCK_SETUP == 4)
    #define CPU_XTAL_CLK_HZ                 50000000u  //时钟发生器频率Hz 
    #define CPU_XTAL32k_CLK_HZ              32768u    //时钟发生器频率Hz /
    #define CPU_INT_SLOW_CLK_HZ             32768u    //慢速内部时钟振荡器频率 Hz 
    #define CPU_INT_FAST_CLK_HZ             4000000u  //快速内部时钟振荡器频率 Hz
    #define DEFAULT_SYSTEM_CLOCK            200000000u //默认系统时钟频率
#endif
//处理器信息及主频信息结构
typedef struct
{
	u8 FamilyType;    //Kinetis系列家族类型号 
	u16 PinCnt;       //引脚数量
	u8 ResetState;    //复位原因
	u8 SiliconRev;    //SiliconRev
	u32 PFlashSize;   //PFlash大小
	u32 FlexNVMSize;  //FlexNVM大小
	u32 RAMSize;      //RAM大小
	u8  ClkOpt;       //主频选项
	u32 CoreClock;    //内核时钟
	u32 BusClock;     //总线时钟
	u32 FlexBusClock; //FlexBus时钟
	u32 FlashClock;   //Flash时钟
} CPUInfoType_t;
extern CPUInfoType_t CPUInfo;
//本构件实现的接口函数列表
void SystemSoftReset(void);                                     //软复位
void GetCPUInfo(void);                                          //获得处理器信息
void SystemInit (void);                                         //系统初始化
void SystemCoreClockUpdate (void);                              //重新计算内核频率
void NVIC_Pgc (u32 PriorityGroup);                              //设置NVIC中断组
void NVIC_Setp (IRQn_Type IRQn, u32 priority);                  //设置NVIC中断优先级
void EnableInterrupts(void);                                    //时能总中断
void DisableInterrupts(void);                                   //关闭总中断
void SetVectorTableAdress(u32 offset);                          //设置中断向量起始位置
#endif
