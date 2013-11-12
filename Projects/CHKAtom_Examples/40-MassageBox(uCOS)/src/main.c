#include "sys.h"
#include "uart.h"
#include "gpio.h"
#include "adc.h"
#include "can.h"
#include "dac.h"
#include "delay.h"
#include "dma.h"
#include "enet.h"
#include "flash.h"
#include "ftm.h"
#include "i2c.h"
#include "lptm.h"
#include "pdb.h"
#include "pit.h"
#include "rtc.h"
#include "sd.h"
#include "spi.h"
#include "tsi.h"
#include "wdog.h"

//LED Devices
#include "led.h"
#include "led_chkatom.h"
//KBI Devices
#include "kbi.h"
#include "kbi_chkatom.h"
//SPI Flash Devices;
#include "spiflash.h"
//LCD Devices
#include "spilcd.h"  

//CHGUI 
#include "chgui.h"         
#include "chgui_char.h"    
#include "chgui_bmp.h"     

//MiniShell
#include "minishell.h"
//uCOS
#include "includes.h"


//超核 飞思卡尔 Kinetis固件库例程
//固件库版本 V2.4
//startup:        启动文件
//devices:        外设芯片支持库(部分实验用到)
//utilities:      软件模块支持库(部分实验用到)
//drivers:        固件库源码

/*
* 入门知识:
* 1- 开启uCOS 3个任务 一个发送信号量和邮箱  另外一个接收信号量并显示 另外一个接收消息邮箱并显示相关信息 
*/




#define TASK_STK_SIZE              (128)  //所有任务统一使用128字节堆栈地址
//定义任务优先级
#define APP_START_TASK_PRIO        (4)  //开始任务
#define APP_LED_TASK_PRIO          (6)  //LED闪烁任务
#define APP_MBOX_TASK_PRIO         (8)  //邮箱接收显示任务
#define APP_SEM_TASK_PRIO          (9)  //信号量接收显示任务
#define APP_POST_TASK_PRIO         (10) //邮箱 信号量投递任务
#define APP_WDOG_TASK_PRIO         (20) //看门狗 任务
//声明任务堆栈
OS_STK  APP_START_STK[TASK_STK_SIZE];
OS_STK  APP_LED_STK[TASK_STK_SIZE];
OS_STK  APP_MBOX_STK[TASK_STK_SIZE];
OS_STK  APP_SEM_STK[TASK_STK_SIZE];
OS_STK  APP_POST_STK[TASK_STK_SIZE];
OS_STK  APP_WDOG_STK[TASK_STK_SIZE];
//////////////////////////////////////////////////////////////////////////////
OS_EVENT * msg_test;		//按键邮箱事件块指针
OS_EVENT * sem_test;		//蜂鸣器信号量指针	 
//看门狗任务
void AppWDOGTask(void *pdata)
{
	pdata=pdata; //防止编译器出错 无实际意义
	WDOG_Init(100); //开启看门狗 100MS超时复位
	while(1)
	{
		WDOG_Feed();
		OSTimeDlyHMSM(0, 0, 0, 50);
	}
}
//邮箱投递，信号量投递任务
void AppPostTask(void *pdata)
{
	uint8_t key='A';
	uint16_t TaskCtr=0;
	while(1)
	{
		key++;
		TaskCtr++;
		if(key > 'Z') key='A';            //改变邮箱投递的数据
		OSMboxPost(msg_test,(void*)key);  //发送消息
		OSSemPost(sem_test);              //发送信号量

		UART_printf("App Post Message&Sem:%d times\r\n", TaskCtr);
		OSTimeDlyHMSM(0, 0, 0, 300);
	}
}
//LED小灯任务
void AppLEDTask(void *pdata)
{
  pdata=pdata; //防止编译器出错 无实际意义
	while(1)
	{
		LED_Toggle(kLED1);
		OSTimeDlyHMSM(0, 0, 0, 300);
	}
}
//邮箱接收函数任务
void AppMBOXTask(void *pdata)
{
	uint8_t key;
	uint8_t err;
	uint16_t TaskCtr=0;
  pdata=pdata; //防止编译器出错 无实际意义
	while(1)
	{
		key=(uint32_t)OSMboxPend(msg_test,0,&err);  //等待消息邮箱
		TaskCtr++;
      UART_printf("Received MBox:%d \r\n", key);
	}
}
//信号量测试，显示函数
void AppSEMTask(void *pdata)
{
	uint8_t err;
	uint16_t TaskCtr=0;
  pdata=pdata; //防止编译器出错 无实际意义
	while(1)
	{
		OSSemPend(sem_test,0,&err); //等待信号量
		TaskCtr++;
		UART_printf("Received Sem:%d  \r\n", TaskCtr);
	}
}

//开始任务
void AppStartTask(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 		  
	msg_test=OSMboxCreate((void*)0);	//创建消息邮箱
	sem_test=OSSemCreate(0);		//创建信号量	
	OSStatInit();					//初始化统计任务.这里会延时1秒钟左右	
 	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
    OSTaskCreate(AppLEDTask,(void *)0,
                &APP_LED_STK[TASK_STK_SIZE-1],
                APP_LED_TASK_PRIO); //建立LED1任务
    OSTaskCreate(AppMBOXTask,(void *)0,
                &APP_MBOX_STK[TASK_STK_SIZE-1],
                APP_MBOX_TASK_PRIO); //建立邮箱接收显示任务
    OSTaskCreate(AppSEMTask,(void *)0,
                &APP_SEM_STK[TASK_STK_SIZE-1],
                APP_SEM_TASK_PRIO); //建立信号量接收显示任务
    OSTaskCreate(AppWDOGTask,(void *)0,
                &APP_WDOG_STK[TASK_STK_SIZE-1],
                APP_WDOG_TASK_PRIO); //建立看门狗任务
    OSTaskCreate(AppPostTask,(void *)0,
                &APP_POST_STK[TASK_STK_SIZE-1],
                APP_POST_TASK_PRIO); //建立邮箱，信号量投递任务
  UART_printf("uCOSII MBox&Sem DemoTest\r\n");
 	OSTaskSuspend(APP_START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}

static void uCOS_SysTickInit(void);

int main(void)
{
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHKATOM, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
	  DisplayCPUInfo();
	  uCOS_SysTickInit();
    OSInit();  //OS初始化
OSTaskCreate(AppStartTask,(void *)0,
							&APP_START_STK[TASK_STK_SIZE-1],
							APP_START_TASK_PRIO); //建立LED1任务
	OSStart(); //控制权交给操作系统
	//程序永远不会运行到这
}

void SysTick_Handler (void)
{
	OSIntEnter();		//进入中断
	OSTimeTick();       //调用ucos的时钟服务程序               
	OSIntExit();        //触发任务切换软中断
}

void uCOS_SysTickInit(void)
{
    SysTick->CTRL|=1<<1;   						//开启SYSTICK中断
    SysTick->LOAD=CPUInfo.CoreClock/OS_TICKS_PER_SEC; 	//每1/OS_TICKS_PER_SEC秒中断一次	
    SysTick->CTRL|=1<<0;   														//开启SYSTICK 
}
