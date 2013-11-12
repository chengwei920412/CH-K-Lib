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
* 1- 调试串口是本固件库中的一个特色 其本质是使用串口轮询发送
* 2- 轮询发送(Polling) 也叫同步发送 或阻塞式发送传输 每发送完一个字节后CPU会等待外设发送完成后再发送后面的字节
*    这种方式会大量浪费CPU资源 但是是最简单也最易学的一种发送方式 虽然效率低下但是应用最广
* 3- UART_printf使用轮询发送模式 使用串口输出信息 是除在线调试的重要手段
* 4- DisplayCPUInfo实在uart构件中的函数 他；利用UART_printf输出芯片信息 前提是 UART_DebugPortInit必须先被调用
*/


int main(void)
{
    uint8_t i = 0;
    //初始化系统时钟 使用外部50M晶振 PLL倍频到100M
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    //初始化LED
    LED_Init(LED_PinLookup_CHKATOM, kNumOfLED);
    //KBI 初始化
    KBI_Init(KBI_PinLookup_CHKATOM, kNumOfKEY);
    //初始化UART调试串口 使用 UART4 占用PC14 PC15 引脚 波特率115200
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
    //打印HelloWorld字符
    UART_printf("HelloWorld\r\n");
    //打印CPU信息
    DisplayCPUInfo();
    while(1)
    {
        UART_printf("i:%d\r\n", i++);
        DelayMs(500);
    }
}

