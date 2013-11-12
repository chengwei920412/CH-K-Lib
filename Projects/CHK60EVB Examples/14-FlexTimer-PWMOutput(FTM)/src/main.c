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


//超核 飞思卡尔 Kinetis固件库例程
//固件库版本 V2.4
//startup:        启动文件
//devices:        硬件板子资源支持库(部分实验用到)
//utilities:      软件模块支持库(部分实验用到)
//drivers:        固件库源码
//use:            用户代码 包含mian函数和 中断服务程序 

/*
* 入门知识:
*/


int main(void)
{
    uint32_t FTMDuty = 0;
    FTM_InitTypeDef FTM_InitStruct1;
    //初始化系统时钟 使用外部50M晶振 PLL倍频到100M
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHK60EVB, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
    UART_printf("PWM Test\r\n");
	
    //初始化FTM
    FTM_InitStruct1.Frequency = 1000;                // 1KHZ
    FTM_InitStruct1.FTMxMAP = FTM0_CH0_PC1;          //FTM0_CH0 PC1引脚
    FTM_InitStruct1.FTM_Mode = FTM_Mode_EdgeAligned; //边沿对齐模式
    FTM_InitStruct1.InitalDuty = 4000;               //初始化后产生40%的占空比
    FTM_Init(&FTM_InitStruct1);
	
    FTM_InitStruct1.InitalDuty = 7000;               //初始化后产生70%的占空比
    FTM_InitStruct1.FTMxMAP = FTM0_CH1_PC2;          //FTM0 CH1 PC2 引脚
    FTM_Init(&FTM_InitStruct1);
    while(1)
		{
        FTMDuty += 1000;
	    	FTMDuty %= 10000; //FTM占空比输入范围 0-10000 对应 %0- 100%
	    	FTM_PWM_ChangeDuty(FTM0_CH0_PC1, FTMDuty); //FTMDUTY变化
	    	DelayMs(300);
		}
 }

