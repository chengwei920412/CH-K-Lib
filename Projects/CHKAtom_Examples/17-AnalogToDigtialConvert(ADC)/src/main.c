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
* 
*/



int main(void)
{
    uint32_t ADC_Value = 0;
    ADC_InitTypeDef ADC_InitStruct1;
    //初始化系统时钟 使用外部50M晶振 PLL倍频到100M
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHKATOM, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
	  DisplayCPUInfo();
    //初始化ADC
    ADC_InitStruct1.ADCxMap = ADC0_SE13_PB3;              //PC0 引脚作为ADC0 14通道
    ADC_InitStruct1.ADC_Precision = ADC_PRECISION_10BIT;  //10位精度
    ADC_InitStruct1.ADC_TriggerSelect = ADC_TRIGGER_SW;   //软件触发(A 通道可使用软/硬件触发 B 通道只能使用硬件触发)
    ADC_Init(&ADC_InitStruct1);
    while(1) 
    {
        ADC_Value = ADC_GetConversionValue(ADC0_SE13_PB3); //获得AD转换值
        UART_printf("ADC0_SE13_PB3:%d\r\n",ADC_Value);
        DelayMs(300);
    }
}

