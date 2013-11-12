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
* 1- CHGUI支持触摸屏 由chgui_touch实现
* 2- ADS7843是触摸AD芯片 ADS7843.c,.h实现物理层 驱动 然后使用CHGUI_TouchCtrlOperation_TypeDef
*    连接物理层与CHGUI接口
*/
CHGUI_PID_TypeDef State;
//LCD操作连接器
CHGUI_CtrlOperation_TypeDef gILI9320_OperationStrcut1 = 
{
	LCD_Init,        
	NULL,
	LCD_DrawPoint,
	NULL,
	LCD_SetCursor,
	LCD_DrawHLine,
	LCD_DrawVLine,
	LCD_Fill,
	LCD_GetDeivceID,
};

//触摸屏操作连接器
CHGUI_TouchCtrlOperation_TypeDef gADS7843_OperationStruct1 = 
{
    ADS7843_Init,
    ADS7843_Get_X_Value,
    ADS7843_Get_Y_Value,
};

//CHGUI初始化结构
CHGUI_InitTypeDef CHGUI_InitStruct1 = 
{
    "CHK60", 
	   0,      
	   LCD_X_MAX, 
	   LCD_Y_MAX, 
    &gILI9320_OperationStrcut1,   //LCD物理层操作函数接口
    &gADS7843_OperationStruct1,   //触摸屏操作物理层函数接口
};


int main(void)
{
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHK60EVB, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
	  DisplayCPUInfo();
    GUI_Init(&CHGUI_InitStruct1);
	  GUI_SetFontFormName("FONT_CourierNew");
    while(1) 
    {
		    GUI_GotoXY(0, 0);
			  GUI_TOUCH_GetState(&State);
			  //打印物理AD坐标
			  GUI_printf("Phy:X:%04d Y:%04d\r\n", GUI_TOUCH_GetxPhys(), GUI_TOUCH_GetyPhys());
			  //打印逻辑AD坐标
        GUI_printf("Log:X:%04d Y:%04d\r\n", State.x, State.y);	
        GUI_printf("State:%01d\r\n", State.Pressed);
			  //LCD 画笔跟踪
        GUI_DrawPoint(State.x, State.y);			
			  DelayMs(10);
			  //GUI 触摸屏处理函数 每10MS调用一次
			  GUI_TOUCH_Exec();
    }
}

