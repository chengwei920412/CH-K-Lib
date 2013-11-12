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
* 1-K60有A B C D E 5个PORT口每个PORT口有0-31个引脚(根据型号不同会有删减)
* 2-每个GPIO引脚 可设置为 输出 输入
* 3-输入又可以设置为普通输入 和 外部中断触发输入
* 4-Bit_RESET:低电平  Bit_SET:高电平
* 5-初始化一个GPIO:
*   1:先声明结构体GPIO_InitStruct1
*   2:给结构体传递相应的配置参数
*   3:调用GPIO_Init() 来真正初始化这个GPIO
* 6-如果要初始化多个GPIO 只能重新配置初始化结构体 然后依次调用GPIO_Init
*/

int main(void)
{
    //定义GPIO初始化结构
    GPIO_InitTypeDef GPIO_InitStruct1;
    //初始化系统时钟 使用内部RC振荡器 FLL倍频到96M
    SystemClockSetup(ClockSource_IRC,CoreClock_96M);
    //初始化延时
	  DelayInit();
    GPIO_InitStruct1.GPIOx = PTD;                       //PTD(PORTD) 端口
    GPIO_InitStruct1.GPIO_InitState = Bit_RESET;        //如果设置为输出 则输出低电平 如果设置为输入 此项无效
    GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DISABLE;    //禁止此引脚的中断请求
    GPIO_InitStruct1.GPIO_Pin = GPIO_Pin_7;             //D7引脚
    GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_OPP;         //设置为推挽输出模式
    //执行GPIO初始化 点亮LED
    GPIO_Init(&GPIO_InitStruct1);                             
	
    while(1)
    {
        GPIO_ToggleBit(PTD, GPIO_Pin_7); //翻转电平
        DelayMs(500); //延时500MS
    }
}

