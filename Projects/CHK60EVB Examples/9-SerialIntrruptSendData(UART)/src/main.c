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
* 1- 本实验采用中断方式发送串口数据
* 2- 所谓发送中断 就是 串口再每发送完成一个字节后进入中断服务程序 通知CPU CPU根据当前状态 送入Buffer下一个字符发送
* 3- 这种方式也叫做异步发送 或者 非阻塞式发送 即UART_SendDataInt 被调用后 开始发送串口数据 而并不是等待发送完成后才执行完
* 4- 中断发送相比轮询发送极大的节约了CPU资源
*/

uint8_t gTestBuffer[] = "UART Interrupt Send\r\n";

int main(void)
{
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
    //开启发送中断功能
    UART_ITConfig(UART4,UART_IT_TDRE,ENABLE);
    //接通NVIC上对应串口中断线
    NVIC_EnableIRQ(UART4_RX_TX_IRQn);
    //开始中断发送
    UART_SendDataInt(UART4,gTestBuffer,sizeof(gTestBuffer));
    //等待中断发送完成
    while(UART_TxIntStruct1.IsComplete == FALSE); 
    while(1)
		{
        LED_Toggle(kLED1);
        DelayMs(500);
		}
}

