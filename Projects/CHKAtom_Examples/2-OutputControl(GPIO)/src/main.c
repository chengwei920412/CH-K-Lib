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


//超核 飞思卡尔 Kinetis固件库例程
//固件库版本 V2.4
//startup:        启动文件
//devices:        硬件板子资源支持库(部分实验用到)
//utilities:      软件模块支持库(部分实验用到)
//drivers:        固件库源码
//use:            用户代码 包含mian函数和 中断服务程序 

/*
* 入门知识:
* 1-SystemClockSetup: 使用片内FLL 或 PLL倍频内核时钟: 
*   1:ClockSource_EX50M:外部50M晶振  ClockSource_IRC:内部RC振荡器
*   2:CoreClock_100M: 倍频到 内核频率 = 100M
* 2-Delay: 延时构件 利用SysTick 定时器实现 首先初始化DelayInit(); 然后就可以使用DelayMs 和  DelayUs了
* 3-GPIO_ToggleBit() 翻转一个IO口的电平 类似的还有
*   1:GPIO_SetBits   将一个IO口设置为高电平
*   2:GPIO_ResetBits 将一个IO口设置为低电平 
*   3:等多函数参见gpio.h 和 gpio.c
*/

int main(void)
{
	//定义GPIO初始化结构
	GPIO_InitTypeDef GPIO_InitStruct1;
	//初始化系统时钟 使用内部RC振荡器 FLL倍频到96M
	SystemClockSetup(ClockSource_IRC,CoreClock_96M);
        DelayInit();
	GPIO_InitStruct1.GPIOx = PTC;                       //PTC(PORTC) 端口
	GPIO_InitStruct1.GPIO_InitState = Bit_SET;          //如果设置为输出 则输出高电平 如果设置为输入 此项无效
	GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DISABLE;    //禁止此引脚的中断请求
	GPIO_InitStruct1.GPIO_Pin = GPIO_Pin_16;            //C16引脚
	GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_OPP;         //设置为推挽输出模式
	//执行GPIO初始化 点亮LED
	GPIO_Init(&GPIO_InitStruct1);                             
	
    while(1)
    {
        GPIO_ToggleBit(PTC, GPIO_Pin_16);
        DelayMs(500);
    }
}

