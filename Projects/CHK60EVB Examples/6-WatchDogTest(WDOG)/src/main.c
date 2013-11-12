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
* 1- 看门狗 - 防止芯片程序意外跑飞
* 2- 本实验 - 上电后闪灯10次 制造可以看见的复位效果 如果1S内没有按键 则 没有喂狗 芯片复位继续闪灯 如果有按键按下 喂狗成功 则不复位
*/


int main(void)
{
    uint8_t i;
    //初始化系统时钟 使用外部50M晶振 PLL倍频到100M
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    //初始化LED
    LED_Init(LED_PinLookup_CHK60EVB, kNumOfLED);
	  //KBI 初始化
	  KBI_Init(KBI_PinLookup_CHK60EVB, kNumOfKEY);
    //闪灯 制造复位效果
    for(i = 0; i < 10; i++)
    {
        LED_Toggle(kLED1);
        DelayMs(50);
    }
	  //初始化看门狗 如果1000MS 内没有喂狗则 复位
	  WDOG_Init(1000);
    while(1)
    {   
        KBI_Scan();
        if((KBI_GetKeyState(kKEY1) == kKBI_SINGLE) || (KBI_GetKeyState(kKEY2) == kKBI_SINGLE))
        {  
					  //如果案件按下 喂狗:
            WDOG_Feed();
        }
				//扫描间隔延时
        DelayMs(KBI_SCAN_PERIOD_IN_US/1000);
    }
}

