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
#include "ads7843.h"

//CHGUI 
#include "chgui.h"         
#include "chgui_char.h"    
#include "chgui_bmp.h"     
#include "chgui_touch.h"

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
* 1
*/

uint8_t NRF2401RXBuffer[32];

int main(void)
{
    uint32_t counter;
    NRF2401_InitTypeDef NRF2401_InitStruct1;
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHKATOM, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
    DisplayCPUInfo();
    //配置2401引脚
    NRF2401_InitStruct1.CE_GPIO_Instance  = PTA_BASE;    //CE
    NRF2401_InitStruct1.CE_GPIO_Pin       = GPIO_Pin_13;
    NRF2401_InitStruct1.CS_GPIO_Instance  = PTA_BASE;    //CS
    NRF2401_InitStruct1.CS_GPIO_Pin       = GPIO_Pin_14;
    NRF2401_InitStruct1.IRQ_GPIO_Instance = PTA_BASE;    //IRQ
    NRF2401_InitStruct1.IRQ_GPIO_Pin      = GPIO_Pin_12;
    NRF2401_InitStruct1.DATA_SPI_CSMap = SPI0_PCS0_PA14; //SPI
    NRF2401_InitStruct1.DATA_SPI_DataMap = SPI0_SCK_PA15_SOUT_PA16_SIN_PA17;
    NRF2401_Init(&NRF2401_InitStruct1);
    if(NRF24L01_Check() != NRF_OK)
    {
        UART_printf("NRF2401 init failed\r\n");
        return 1;
    }
		
    //先设置为接收模式
    NRF2401_SetRXMode(); 
    while(1)
    {
        if(counter > 20000)
        {
            NRF2401_SetTXMode(); //设置为发送模式
            if(NRF2401_SendData("NRF2401 Test CHK60EVB!") == TX_OK)
            {
                UART_printf("Send Succ.\r\n");
            }
            else
            {
                UART_printf("Send Failed\r\n");
            }
            NRF2401_SetRXMode(); //设置为接收模式
            counter = 0;
        }
        counter++;
        if(NRF2401_RecData(NRF2401RXBuffer) == NRF_OK) //接收到了数据
        {
            UART_printf("DataRec:%s\r\n",NRF2401RXBuffer); //打印数据
        }
    }
}

