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
* 1- 本实验采用轮询方式接收串口数据
* 2- 进入while循环后 不断扫描 串口有没有接收到数据 一旦接收到数据则echo回去
* 3- 这种方式 需要 CPU不断的进行扫描 导致资源浪费 并且如果while中有其他任务 则 可能导致串口数据相应不及时
*/


int main(void)
{
    uint8_t ch;
    //定义串口初始化结构
    UART_InitTypeDef UART_InitStruct1;
    //初始化系统时钟 使用外部50M晶振 PLL倍频到100M
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    //初始化LED
    LED_Init(LED_PinLookup_CHK60EVB, kNumOfLED);
	  //KBI 初始化
	  KBI_Init(KBI_PinLookup_CHK60EVB, kNumOfKEY);
	
    UART_InitStruct1.UARTxMAP = UART4_RX_C14_TX_C15; //UART4模块 映射引脚:PTC14 PTC15
    UART_InitStruct1.UART_BaudRate = 115200;         //波特率 115200
    UART_Init(&UART_InitStruct1);                    //初始化串口

    while(1)
		{
        //接收成功
        if(UART_ReceiveData(UART4, &ch) == TRUE)
				{
				    //echo
            UART_SendData(UART4, ch);
				}
		}
}

