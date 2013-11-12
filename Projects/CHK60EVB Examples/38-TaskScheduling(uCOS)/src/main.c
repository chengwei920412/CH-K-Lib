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
#include "led_chk60evb.h"
//KBI Devices
#include "kbi.h"
#include "kbi_chk60evb.h"
//SPI Flash Devices;
#include "spiflash.h"
//LCD Devices
#include "lcd_chk60evb.h"  
#include "ads7843.h"

//CHGUI 
#include "chgui.h"         
#include "chgui_char.h"    
#include "chgui_bmp.h"     
#include "chgui_touch.h"

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
* 1- uCOS操作系统的入门实验
* 本试验 让uCOS执行2个小任务 分别翻转2个小灯的电平 uCOS入门试验
*/

//任务堆栈大小
#define TASK_STK_SIZE              (128)
//定义任务优先级
#define APP_LED1_TASK_PRIO         (5)
#define APP_LED0_TASK_PRIO         (7)
//声明任务堆栈
OS_STK  APP_LED1_STK[TASK_STK_SIZE];
OS_STK  APP_LED0_STK[TASK_STK_SIZE];
//LED小灯任务
void AppLED1Task(void *pdata)
{
  pdata = pdata; //防止编译器出错 无实际意义
	while(1)
	{
		UART_printf("AppLED 1 Task:Process\r\n");
		LED_Toggle(kLED1);
		OSTimeDlyHMSM(0, 0, 0, 300);
	}
}

void AppLED0Task(void *pdata)
{
  pdata = pdata; //防止编译器出错 无实际意义
	while(1)
	{
		UART_printf("AppLED 2 Task:Process\r\n");
		LED_Toggle(kLED2);
		OSTimeDlyHMSM(0, 0, 0, 300);
	}
}

void uCOS_SysTickInit(void)
{
    SysTick->CTRL|=1<<1;   						//开启SYSTICK中断
    SysTick->LOAD=CPUInfo.CoreClock/OS_TICKS_PER_SEC; 	//每1/OS_TICKS_PER_SEC秒中断一次	
    SysTick->CTRL|=1<<0;   														//开启SYSTICK 
}

int main(void)
{
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHK60EVB, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
	  DisplayCPUInfo();
	  uCOS_SysTickInit();
    OSInit();  //OS初始化
    OSTaskCreate(AppLED1Task,(void *)0,
                &APP_LED1_STK[TASK_STK_SIZE-1],
                APP_LED1_TASK_PRIO); //建立LED1任务
    OSTaskCreate(AppLED0Task,(void *)0,
                &APP_LED0_STK[TASK_STK_SIZE-1],
                APP_LED0_TASK_PRIO); //建立LED0任务
    OSStart(); //控制权交给操作系统
	//程序永远不会运行到这
  while(1);
}

void SysTick_Handler (void)
{
	OSIntEnter();		//进入中断
	OSTimeTick();       //调用ucos的时钟服务程序               
	OSIntExit();        //触发任务切换软中断
}

