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


//超核 飞思卡尔 Kinetis固件库例程
//固件库版本 V2.4
//startup:        启动文件
//devices:        硬件板子资源支持库(部分实验用到)
//utilities:      软件模块支持库(部分实验用到)
//drivers:        固件库源码
//use:            用户代码 包含mian函数和 中断服务程序 

/*
* 入门知识:
* 1- LPTMR 低功耗定时器 可以用来做普通内部计数或者外部脉冲计数
* 2- 本实验利用FTM模块在PC4上产生1KHZ方波 使用PC5进行脉冲技术 并串口打印显示
*    实验室请拔掉LCD 短接C5C4
*/


int main(void)
{
    uint32_t LPTM_Value = 0;
    LPTM_InitTypeDef LPTM_InitStruct1;
    FTM_InitTypeDef FTM_InitStruct1;
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHK60EVB, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
	  DisplayCPUInfo();
	  //在PC3上产生 1KHz 占空比50%的 方波
    FTM_InitStruct1.Frequency = 1000;
    FTM_InitStruct1.FTMxMAP = FTM0_CH3_PC4;
    FTM_InitStruct1.FTM_Mode = FTM_Mode_EdgeAligned;
    FTM_InitStruct1.InitalDuty = 5000;
    FTM_Init(&FTM_InitStruct1);
    //开启PTC5上的脉冲计数引脚
    LPTM_InitStruct1.LPTMxMap = LPTM_CH2_PC5;
    LPTM_InitStruct1.LPTM_InitCompareValue = 200;          //在脉冲计数模式下无意义
    LPTM_InitStruct1.LPTM_Mode = LPTM_Mode_PC_FALLING;     //下降沿触发计数
    LPTM_Init(&LPTM_InitStruct1);
    while(1) 
    {
        //读取脉冲技术值
        LPTM_Value = LPTM_GetTimerCounterValue(LPTMR0);
        //清空技术值
        LPTM_ResetTimeCounter(LPTMR0);
        UART_printf("LPTMR:%dHz\r\n", LPTM_Value);
        DelayMs(1000);
    }
}

