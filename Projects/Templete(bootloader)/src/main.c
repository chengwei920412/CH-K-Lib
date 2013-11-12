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
//user:           用户代码 包含mian函数和 中断服务程序 
 
/*
* 入门知识:
* 1-CH-Kinetis固件库 模板程序(Bootloader版本)
* 2-如果使用Bootloader 才用次模板 否则不要用这个模板
*/

int main(void)
{
    //定义GPIO初始化结构
    GPIO_InitTypeDef GPIO_InitStruct1;
    SetVectorTable(0x05000UL); //为了配合Bootlaoder程序，更改中断向量表起始地址	
    //初始化系统时钟 使用内部RC振荡器 FLL倍频到96M
    SystemClockSetup(ClockSource_EX50M,CoreClock_96M);
    //初始化延时
    DelayInit();
    GPIO_InitStruct1.GPIOx = PTD;                       //PTD(PORTD) 端口
    GPIO_InitStruct1.GPIO_InitState = Bit_RESET;        //如果设置为输出 输出为低电平  设置为输入时此项无效
    GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DISABLE;    //禁止此引脚的中断请求
    GPIO_InitStruct1.GPIO_Pin = GPIO_Pin_7;             //D7引脚
    GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_OPP;         //设置为推挽输出模式
    //执行GPIO初始化 点亮LED
    GPIO_Init(&GPIO_InitStruct1);                             
	
    while(1)
    {
        GPIO_ToggleBit(PTD, GPIO_Pin_7);//翻转电平
        DelayMs(500);
    }
}

