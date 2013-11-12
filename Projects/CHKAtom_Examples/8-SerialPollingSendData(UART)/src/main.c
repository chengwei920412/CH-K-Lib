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
* 1- 这个实验室中不使用UartDebugPort 只是使用UART中标准函数UART_Init初始化串口 并且轮询发送数据
     这是串口的轮询发送标准用法
* 2- 当然 可以自己利用UART构件中发送一个字符的函数UART_SendData编写一个发送多个字节的函数 如本例的UART_SendBytes
*/

//发送多个字符串
static void UART_SendBytes(UART_Type* UARTx, uint8_t* pBuffer, uint32_t size)
{
    while(size--)
    {
        UART_SendData(UARTx, *(pBuffer++));
		}
}

int main(void)
{
    //定义串口初始化结构
    UART_InitTypeDef UART_InitStruct1;
    //初始化系统时钟 使用外部50M晶振 PLL倍频到100M
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    //初始化LED
    LED_Init(LED_PinLookup_CHKATOM, kNumOfLED);
    //KBI 初始化
   KBI_Init(KBI_PinLookup_CHKATOM, kNumOfKEY);
    
    UART_InitStruct1.UARTxMAP = UART4_RX_C14_TX_C15; //UART4模块 映射引脚:PTC14 PTC15
    UART_InitStruct1.UART_BaudRate = 115200;         //波特率 115200
    UART_Init(&UART_InitStruct1);                    //初始化串口
	
    UART_SendData(UART4, 'H');
    UART_SendData(UART4, 'e');
    UART_SendData(UART4, 'l');
    UART_SendData(UART4, 'l');
    UART_SendData(UART4, 'o');
    //一次发送多个字符串
    UART_SendBytes(UART4, "12345678", 8);
    while(1)
    {
			
    }
}

