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
* 1- GPIO 模式选择: 详见gpio.h
*   1: GPIO_Mode_IN_FLOATING  浮空输入
*   2: GPIO_Mode_IPD          下拉输入
*   3: GPIO_Mode_IPU          上拉输入
*   4: GPIO_Mode_OOD          开漏输出 只能输出低电平
*   5: GPIO_Mode_OPP          推挽输出
* 2- GPIO_ReadInputDataBit() 获得某个IO口的电平状态 前途是该IO已经被设置为输入并且正确的初始化
*/

int main(void)
{
    uint8_t PinState = 0;
    //定义GPIO初始化结构
    GPIO_InitTypeDef GPIO_InitStruct1;
    //初始化系统时钟 使用外部50M晶振 PLL倍频到100M
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    //初始化GPIO输入引脚 (KEY1)
    GPIO_InitStruct1.GPIOx = PTC;              
    GPIO_InitStruct1.GPIO_InitState = Bit_RESET;   
    GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DISABLE;
    GPIO_InitStruct1.GPIO_Pin = GPIO_Pin_17;
    GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_IPU;         //上拉输入
    GPIO_Init(&GPIO_InitStruct1);  
    //初始化GPIO输出引脚(LED1)
	  GPIO_InitStruct1.GPIOx = PTD;  
	  GPIO_InitStruct1.GPIO_Pin = GPIO_Pin_7;
	  GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_OPP;         //推挽输出
    GPIO_Init(&GPIO_InitStruct1);    
	
    while(1)
    {
        //将KEY1的电平状态反映到LED1上 
        PinState = GPIO_ReadInputDataBit(PTC, GPIO_Pin_17);
        GPIO_WriteBit(PTD, GPIO_Pin_7, (BitAction)PinState); 
    }
}

