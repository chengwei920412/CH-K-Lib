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



//MiniShell
#include "minishell.h"

//超核 飞思卡尔 Kinetis固件库例程
//固件库版本 V2.4
//startup:        启动文件
//devices:        外设芯片支持库(部分实验用到)
//utilities:      软件模块支持库(部分实验用到)
//drivers:        固件库源码

/*
* 入门知识:
* 1- MiniShell 超核开发的简易Shell系统
* 2- MiniShell的 使用 首先实现GetChar 和 PutChar函数然后定义安装结构体 传入MINISHELL_Install即可
* 3- 用户函数可由SHELL_Register 注册完成 可以注册最大64个用户函数
* 4- 注册完成后调用MINISHELL_Init()初始化
* 5- 出事后话调用LOOP函数开始SHELL循环
* 6- 打开上位机终端软件 输入命令即可
*/
extern volatile int OS_TimeMS;
int main(void)
{
    OS_TimeMS=0; 
    PIT_InitTypeDef PIT_InitStruct1;
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHK60EVB, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
	  DisplayCPUInfo();
	  //
    PIT_InitStruct1.PITx = PIT0;
    PIT_InitStruct1.PIT_Interval = 1;
	  PIT_Init(&PIT_InitStruct1);
	  PIT_ITConfig(PIT0, PIT_IT_TIF, ENABLE);
	  NVIC_EnableIRQ(PIT0_IRQn);
	  //
    PIT_InitStruct1.PITx = PIT1;
    PIT_InitStruct1.PIT_Interval = 10;
	  PIT_Init(&PIT_InitStruct1);
	  PIT_ITConfig(PIT1, PIT_IT_TIF, ENABLE);
	  NVIC_EnableIRQ(PIT1_IRQn);
	  ADS7843_Init();
		MainTask();			 //运行UCGUIdemo
    while(1) 
    {
			
    }
}

