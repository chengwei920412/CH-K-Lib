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
//SPI Flash Devices;
#include "spiflash.h"
//LCD Devices
#include "lcd_chk60evb.h"  
#include "ads7843.h"

//CHGUI 
#include "chgui.h"         
#include "chgui_char.h"    
#include "chgui_bmp.h"     
#include "chgui_touch.h"

//MiniShell
#include "minishell.h"

//超核 飞思卡尔 Kinetis固件库例程
//固件库版本 V2.4
//startup:        启动文件
//devices:        外设芯片支持库(部分实验用到)
//utilities:      软件模块支持库(部分实验用到)
//drivers:        固件库源码

/*
* 入门知识:
* 1- ENET 以太网模块 Internet的物理基础 传输的单位为以太帧
* 2- 本实验主要测试以太网性能  可以将网线连接到PC上 PC会发出以太帧 而后在串口上会打印收到的帧数和信息
*/

//本地MAC物理地址定义
//报文类型定义
//本机地址
uint8_t  gCfgLoca_MAC[] = {0x22, 0x22, 0x22, 0x00, 0x00, 0x01};

//以太帧发送缓冲区
uint8_t  gBuffer[1520]; 

extern uint8_t gEnetFlag ;

int main(void)
{
    uint32_t len;
    uint32_t i;
    ENET_InitTypeDef ENET_InitStruct1;
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHK60EVB, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
	  DisplayCPUInfo();

	  ENET_InitStruct1.pMacAddress = gCfgLoca_MAC;
	  ENET_Init(&ENET_InitStruct1);
	  if(ENET_MiiLinkState() == TRUE)
		{
        UART_printf("ENET Phy Connected succ.\r\n");
		}
		else
		{
			  UART_printf("ENET Phy Connected failed.\r\n");
		}
    while(1) 
    {
				//等待其他设备(PC机 发来的消息)
				if(gEnetFlag == 1)
				{
						len = ENET_MacRecData(gBuffer);
						UART_printf("Enet frame received, len:%d\r\n", len);
						//打印接收到的数据
						for(i = 0; i < len; i++)
						{
								UART_printf("0x%x ", gBuffer[i]);
						}
						UART_printf("\r\n");
				 gEnetFlag = 0;
			 }
    }
}

