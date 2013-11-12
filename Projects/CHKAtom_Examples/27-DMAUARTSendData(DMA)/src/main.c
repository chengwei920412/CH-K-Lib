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
//SPI Flash Devices;
#include "spiflash.h"
//LCD Devices
#include "spilcd.h"  

//CHGUI 
#include "chgui.h"         
#include "chgui_char.h"    
#include "chgui_bmp.h"     

//nrf24xx
#include "nrf2401.h"

//超核 飞思卡尔 Kinetis固件库例程
//固件库版本 V2.4
//startup:        启动文件
//devices:        硬件板子资源支持库(部分实验用到)
//utilities:      软件模块支持库(部分实验用到)
//drivers:        固件库源码
//use:            用户代码 包含mian函数和 中断服务程序 

/*
* 入门知识:
* 1- DMA 就是搬运工 可以在CPU不干预的情况下搬运海量数据 特别适用于大数据量传输
* 2- 在本例程中 将DMA用作内串口发送数据传输 即 将内存数据传送到 串口发送寄存器 实现传输功能
*/

uint8_t UARTSendBuffer[] = "This string is send through DMA\r\n";

int main(void)
{
    DMA_InitTypeDef DMA_InitStruct1;
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHKATOM, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
	  DisplayCPUInfo();
    //开启UART 发送中断/DMA功能
    UART_ITConfig(UART4,UART_IT_TDRE,ENABLE);
    //使能UART4 TX DMA发送
    UART_DMACmd(UART4,UART_DMAReq_Tx,ENABLE);
    DMA_InitStruct1.Channelx = DMA_CH0;                         //使用DMA0通道  (0-15)
    DMA_InitStruct1.DMAAutoClose = ENABLE;                      //传输完毕后自动关闭
    DMA_InitStruct1.EnableState = ENABLE;                       //初始化后立即开始传输
    DMA_InitStruct1.MinorLoopLength = sizeof(UARTSendBuffer);   //传输次数
    DMA_InitStruct1.PeripheralDMAReq  = UART4_TRAN_DMAREQ;      //UART 传输完成触发
    DMA_InitStruct1.TransferBytes = 1;                          //每次传输一个字节
    //配置目的地址传输参数
    DMA_InitStruct1.DestBaseAddr = (uint32_t)&(UART4->D);       //指向目的地址
    DMA_InitStruct1.DestDataSize = DMA_DST_8BIT;                //数组为1Byte
    DMA_InitStruct1.DestMajorInc = 0;                           //执行一次大循环后 地址不增加
    DMA_InitStruct1.DestMinorInc = 0;                           //每次传输完地址不增加
 
    //配置源地址传输参数
    DMA_InitStruct1.SourceBaseAddr = (uint32_t)UARTSendBuffer;
    DMA_InitStruct1.SourceDataSize = DMA_SRC_8BIT;
    DMA_InitStruct1.SourceMajorInc = 0;
    DMA_InitStruct1.SourceMinorInc = 1;                         //每次传输1个字节后地址自动加1 将后面的数据继续发送
    DMA_Init(&DMA_InitStruct1);
    //等待传输完成
    while(DMA_IsComplete(DMA_CH0) == FALSE);
    while(1)
    {
		
    }
}

