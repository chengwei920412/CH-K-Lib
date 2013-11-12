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

#include "bmp.h"


//超核 飞思卡尔 Kinetis固件库例程
//固件库版本 V2.4
//startup:        启动文件
//devices:        硬件板子资源支持库(部分实验用到)
//utilities:      软件模块支持库(部分实验用到)
//drivers:        固件库源码
//use:            用户代码 包含mian函数和 中断服务程序 

/*
* 入门知识:
* 1- CHGUI支持图片解码及显示
* 2- 制作图片 将图片用画图打开 调整大小 控制在 LCD_W*LCD_H 内 另存为BMP位图
*    使用 BINtoC 工具将图片转换为C数组 然后调用GUI_BMP_Draw 填入位图数组指针即可
*/


CHGUI_CtrlOperation_TypeDef gILI9320_OperationStrcut1 = 
{
    SPILCD_Init,        
    NULL,
    SPILCD_DrawPoint,
    NULL,
    NULL,
    NULL,
    NULL,
    SPILCD_Fill,
    NULL,
};


//CHGUI初始化结构
CHGUI_InitTypeDef CHGUI_InitStruct1 = 
{
    "CHK60", 
    0,      
    SPILCD_W, 
    SPILCD_H, 
    &gILI9320_OperationStrcut1,   //LCD物理层操作函数接口链接器
};


int main(void)
{
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHKATOM, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
    DisplayCPUInfo();
    GUI_Init(&CHGUI_InitStruct1);
    while(1) 
    {
        GUI_BMP_Draw(0,0,BMP_DATA_TABLE);
    }
}

