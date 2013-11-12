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
*/

#define US_TRIG_GPIO_PORT  PTD
#define US_TRIG_GPIO_PIN   GPIO_Pin_0

#define US_ECHO_GPIO_PORT  PTD
#define US_ECHO_GPIO_PIN   GPIO_Pin_1

int main(void)
{
    uint32_t counter = 0;
    GPIO_InitTypeDef GPIO_InitStruct1;
    //初始化系统时钟 使用外部50M晶振 PLL倍频到100M
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHKATOM, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
    UART_printf("Ultrasonic Test\r\n");
    //Init US_TRIG_Pin
    GPIO_InitStruct1.GPIOx = US_TRIG_GPIO_PORT;
    GPIO_InitStruct1.GPIO_InitState = Bit_RESET;
    GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DISABLE;
    GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_OPP;
    GPIO_InitStruct1.GPIO_Pin = US_TRIG_GPIO_PIN;
    GPIO_Init(&GPIO_InitStruct1);
    //Init US_ECHO_PIN
    GPIO_InitStruct1.GPIOx = US_ECHO_GPIO_PORT;
    GPIO_InitStruct1.GPIO_InitState = Bit_RESET;
    GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DISABLE;
    GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStruct1.GPIO_Pin = US_ECHO_GPIO_PIN;
    GPIO_Init(&GPIO_InitStruct1);
		
		LED_Init(LED_PinLookup_CHKATOM, kNumOfLED);
    while(1)
    {
			  //发送一个 >10US高电平启动 TRIG
        GPIO_SetBits(US_TRIG_GPIO_PORT, US_TRIG_GPIO_PIN);
        DelayUs(30);
        GPIO_ResetBits(US_TRIG_GPIO_PORT, US_TRIG_GPIO_PIN);
        counter = 0;
			  //掠过低电平时间
        while(GPIO_ReadInputDataBit(US_ECHO_GPIO_PORT, US_ECHO_GPIO_PIN) == Bit_RESET) {};
			  //读取高电平时间
        while(GPIO_ReadInputDataBit(US_ECHO_GPIO_PORT, US_ECHO_GPIO_PIN) == Bit_SET)
        {
          counter++;
					DelayUs(1);
        }
        UART_printf("Times %dUS\r\n", counter);
				//延时时间与距离成正比
        DelayUs(counter*100);
				LED_Toggle(kLED1);
    }
 }

