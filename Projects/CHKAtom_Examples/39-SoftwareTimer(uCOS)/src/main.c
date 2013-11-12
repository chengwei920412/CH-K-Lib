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
* 1- 本实验使用uCOS软件定时器 定时刷新LCD颜色  展现软件定时器功能
*/


#define TASK_STK_SIZE              (128)  //所有任务统一使用128字节堆栈地址
//定义任务优先级
#define APP_START_TASK_PRIO        (4)  //开始任务
#define APP_LED_TASK_PRIO          (6)  //LED闪烁任务
#define APP_WDOG_TASK_PRIO         (20) //看门狗 任务
//声明任务堆栈
OS_STK  APP_START_STK[TASK_STK_SIZE];
OS_STK  APP_LED_STK[TASK_STK_SIZE];
OS_STK  APP_WDOG_STK[TASK_STK_SIZE];
//////////////////////////////////////////////////////////////////////////////
OS_TMR   * tmr1;			//软件定时器1
//看门狗任务
void AppWDOGTask(void *pdata)
{
	pdata=pdata; //防止编译器出错 无实际意义
	WDOG_Init(100); //开启看门狗 100MS超时复位
	while(1)
	{
		WDOG_Feed();
		DelayMs(50);
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
//软件定时器1的回调函数				  	   
void tmr1_callback(OS_TMR *ptmr,void *p_arg) 
{	
    static uint32_t counter;
    UART_printf("Enter tmr1_callback:%d times\r\n", counter);
    counter++;	
}
//开始任务
void AppStartTask(void *pdata)
{
	uint8_t err;
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 		  
 	tmr1=OSTmrCreate(100,20,OS_TMR_OPT_PERIODIC,(OS_TMR_CALLBACK)tmr1_callback,0,"tmr1",&err);		//1S后开始执行，每200ms执行一次 根据 OS_TMR_CFG_TICKS_PER_SEC 值决定
	OSStatInit();					//初始化统计任务.这里会延时1秒钟左右	
 	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
    OSTaskCreate(AppLEDTask,(void *)0,
                &APP_LED_STK[TASK_STK_SIZE-1],
                APP_LED_TASK_PRIO); //建立LED1任务
    OSTaskCreate(AppWDOGTask,(void *)0,
                &APP_WDOG_STK[TASK_STK_SIZE-1],
                APP_WDOG_TASK_PRIO); //建立看门狗任务
  UART_printf("uCOSII Software Timer\r\n");
	OSTmrStart(tmr1,&err);//启动软件定时器1				
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

static void uCOS_SysTickInit(void)
{
    SysTick->CTRL|=1<<1;   						//开启SYSTICK中断
    SysTick->LOAD=CPUInfo.CoreClock/OS_TICKS_PER_SEC; 	//每1/OS_TICKS_PER_SEC秒中断一次	
    SysTick->CTRL|=1<<0;   														//开启SYSTICK 
}

void SysTick_Handler (void)
{
	OSIntEnter();		//进入中断
	OSTimeTick();       //调用ucos的时钟服务程序               
	OSIntExit();        //触发任务切换软中断
}

