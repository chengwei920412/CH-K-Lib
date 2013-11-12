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

//LED Components
#include "led.h"
#include "led_chk60evb.h"


//超核 飞思卡尔 Kinetis固件库例程
//固件库版本 V2.4
//startup:        启动文件
//devices:        硬件板子资源支持库(部分实验用到)
//utilities:      软件模块支持库(部分实验用到)
//drivers:        固件库源码
//use:            用户代码 包含mian函数和 中断服务程序 

/*
* 入门知识:
* 1- 封装式构件化设计思想 LED虽小简单 但是要将其封装成构件
*   1: led.c led.h 构件架构 不需要用户修改代码
*   2: led_<boradname>.c  led_<boradname>.h 和板子有关的配置信息 根据硬件板子配置
*   3: kLED1,kLED2是在led_chk60evb.h中定义的枚举量 前面加k 只是为了表示他们是枚举常量
*   4: LED_PinLookup_CHK60EVB[] 是一个保存在 led_chk60evb.c中的 结构体数组 存放板子的LED配置信息 CHK60EVB 是超核K60开发板的简写
*   5: kNumOfLED 是存放在 led_chk60evb.h中的枚举常量 表示板子LED的总数 在CHK60开发板中 它就是2
*   6: LED_Init 第一个参数传入 LED_PinLookup_CHK60EVB 这个结构体指针 第二个参数传入 结构体成员数量 也就是板子上LED的数目
*   7: LED构件具体API函数参见 led.c 和 led.h
*/


int main(void)
{
    //初始化系统时钟 使用外部50M晶振 PLL倍频到100M
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    //初始化LED
    LED_Init(LED_PinLookup_CHK60EVB, kNumOfLED);
	  //将LED1设置为高电平
    LED_Set(kLED1);
    while(1)
    {   
			  //2LED闪烁
			  LED_Toggle(kLED1);
        LED_Toggle(kLED2);
        DelayMs(500);
    }
}

