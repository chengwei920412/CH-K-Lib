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


//超核 飞思卡尔 Kinetis固件库例程
//固件库版本 V2.4
//startup:        启动文件
//devices:        硬件板子资源支持库(部分实验用到)
//utilities:      软件模块支持库(部分实验用到)
//drivers:        固件库源码
//use:            用户代码 包含mian函数和 中断服务程序 

/*
* 入门知识:
* 1- 封装式构件化设计思想 KBI: KeyBorad Input 即按键输入 
*   1: kbi.c kbi.h 架构 不需要修改
*   2: kbi<boradname>.c  led_<boradname>.h 和板子有关的配置信息 根据硬件板子配置
*   3: KBI_Init 第一个参数传入 KBI_PinLookup_CHK60EVB 这个结构体指针 第二个参数传入 结构体成员数量 也就是板子上KEY的数目
*   4: KBI构件具体API函数参见 kbi.c 和 kbi.h
*   5: KBI_Scan() 是按键定期扫描函数 扫描周期必须是 KBI_SCAN_PERIOD_IN_US 单位为US。
*/

//如果使用超核K60开发板，请将kbi.h中 #define	KBI_STATE_DOWN 设置为 Bit_SET
//如果使用超核K60核心板，请将kbi.h中 #define    KBI_STATE_DOWN 设置为 Bit_Reset

int main(void)
{
    //初始化系统时钟 使用外部50M晶振 PLL倍频到100M
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    //初始化LED
    LED_Init(LED_PinLookup_CHKATOM, kNumOfLED);
    //KBI 初始化
    KBI_Init(KBI_PinLookup_CHKATOM, kNumOfKEY);
    while(1)
    {   
			  //扫描按键
        KBI_Scan();
        if(KBI_GetKeyState(kKEY1) == kKBI_SINGLE)
        {
            LED_Toggle(kLED1);
        }
        if(KBI_GetKeyState(kKEY1) == kKBI_LONG)
        {
            LED_Toggle(kLED2);
        }
				//扫描间隔延时
        DelayMs(KBI_SCAN_PERIOD_IN_US/1000);
    }
}

