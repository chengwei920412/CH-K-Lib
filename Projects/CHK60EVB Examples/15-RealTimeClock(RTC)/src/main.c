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
*/


int main(void)
{
    uint8_t last_sec;
    RTC_CalanderTypeDef RTC_Calander1;
    //初始化系统时钟 使用外部50M晶振 PLL倍频到100M
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHK60EVB, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
    UART_printf("RTC TEST\r\n");
    RTC_Init();
	
	  //可以设置时间
    RTC_Calander1.Hour = 10;
    RTC_Calander1.Minute = 57;
    RTC_Calander1.Second = 58;
    RTC_Calander1.Month = 10;
    RTC_Calander1.Date = 10;
    RTC_Calander1.Year = 2013;
    //RTC_SetCalander(&RTC_Calander1);
    NVIC_EnableIRQ(RTC_IRQn);
    while(1) 
		{
        RTC_GetCalander(&RTC_Calander1); //读取时间
        if(last_sec != RTC_Calander1.Second)
				{
            UART_printf("%d-%d-%d %d:%d:%d\r\n", RTC_Calander1.Year, RTC_Calander1.Month, RTC_Calander1.Date, RTC_Calander1.Hour, RTC_Calander1.Minute, RTC_Calander1.Second);
            last_sec = RTC_Calander1.Second;
				}	
		}
 }

