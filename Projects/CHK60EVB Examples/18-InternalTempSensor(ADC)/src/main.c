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
* 1- 内部温度计实际上就是 芯片内部的一个 二极管连接到ADC上。实际还是使用ADC来测量
*/


char strBuffer[30];
int main(void)
{
    float temp;
    float vtemp;
    uint32_t ADC_Value = 0;
    ADC_InitTypeDef ADC_InitStruct1;
    //初始化系统时钟 使用外部50M晶振 PLL倍频到100M
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHK60EVB, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
	  DisplayCPUInfo();
    //初始化ADC
    ADC_InitStruct1.ADCxMap = ADC0_TEMP_SENSOR_DIFF;      //内部温度计
    ADC_InitStruct1.ADC_Precision = ADC_PRECISION_12BIT;  //12位精度
    ADC_InitStruct1.ADC_TriggerSelect = ADC_TRIGGER_SW;   //软件触发(A 通道可使用软/硬件触发 B 通道只能使用硬件触发)
    ADC_Init(&ADC_InitStruct1);
    while(1) 
    {		
        ADC_Value = ADC_GetConversionValue(ADC0_TEMP_SENSOR_DIFF) & 0x0FFF; 
        //将AD值转换为 摄氏度  参考官方例程 最后结果不是很准确
        vtemp=((float)ADC_Value/4096)*3.3;
        if(vtemp>=.7012)
        {
            temp=(vtemp-.7012)/.001646;
        }
        else
        {
            temp=(vtemp-.7012)/.001769;
        }
        temp=25-temp;
        sprintf((char *)strBuffer,"Tempature:%0.3f C\r\n",temp);	
        UART_printf(strBuffer);
        DelayMs(300);
    }
}

