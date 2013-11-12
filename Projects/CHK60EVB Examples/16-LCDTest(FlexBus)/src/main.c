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
//LCD Devices
#include "lcd_chk60evb.h"  //物理层 LCD驱动
#include "chgui.h"         //CHGUI基本组件
#include "chgui_char.h"    //CHGUI字符显示组件
#include "chgui_bmp.h"     //CHGUI BMP图片解码组件


//超核 飞思卡尔 Kinetis固件库例程
//固件库版本 V2.4
//startup:        启动文件
//devices:        硬件板子资源支持库(部分实验用到)
//utilities:      软件模块支持库(部分实验用到)
//drivers:        固件库源码
//use:            用户代码 包含mian函数和 中断服务程序 

/*
* 入门知识:
* 1-LCD和GUI LCD:TFT彩色液晶 GUI:图形用户接口 在这里表示一套软件组件
* 2-CHGUI: 超核开发的CHGUI组件 目前可实现 基本2D库 字符的显示以及BMP图片解码
* 3-CHGUI:用法 : CHGUI 通过一组回调操作指针 完成初始化
* 首先实体化一个 操作结构CHGUI_CtrlOperation_TypeDef 然后填入底层写好的LCD操作函数 最重要的是LCD_DrawPoint
* 只要保证底层接口函数正确 CHGUI就可以运行在任意平台上 与硬件无关
* 完成操作接口函数连接后 在定义CHGUI_InitTypeDef 填入
* LCD设备名 - 目前任意
* LCD设备号 - 目前任意
* LCDX方向最大尺寸
* LCDY方向最大尺寸
* 底层函数连接操作结构的指针
* 然后调用CHGUI_Init(&CHGUI_InitStruct1);即可
* CHGUI具体的函数接口在ch_gui.h chgui_char.h 和  ucgui_bmp.h中有介绍 当然看这个历程也可以
* 如果想切换横屏竖屏 在lcd_chk60evb.h中 定义宏 #define LCD_USE_HORIZONTAL   即可
*/

//底层操作连接结构 目前的版本只需实现LCD_DrawPoint和 LCD_Init就可以工作，其他填NULL就可以
CHGUI_CtrlOperation_TypeDef gILI9320_OperationStrcut1 = 
{
	LCD_Init,        //底层函数与CHGUI无关 LCD_Init的实现在 LCD_CHK60EVB.c中 下同
	NULL,
	LCD_DrawPoint,
	NULL,
	LCD_SetCursor,
	LCD_DrawHLine,
	LCD_DrawVLine,
	LCD_Fill,
	LCD_GetDeivceID,
};

//CHGUI初始化结构
CHGUI_InitTypeDef CHGUI_InitStruct1 = 
{
    "CHK60", //设备名称
	   0,      //设备号 如不明确填0
	   LCD_X_MAX, //LCD X方向最大值 像素
	   LCD_Y_MAX, //LCD Y方向最大值 像素
    &gILI9320_OperationStrcut1,
};


int main(void)
{
    //初始化系统时钟 使用外部50M晶振 PLL倍频到100M
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHK60EVB, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
	  DisplayCPUInfo();
    //初始化GUI
    GUI_Init(&CHGUI_InitStruct1);
	  //设置背景色为黑色
    GUI_SetBkColor(BLACK);
	  //普通字符显示模式
	  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
	  //设置前景色为红色
    GUI_SetColor(LGRAY);
	  //将当前字体设置为CourierNew
    GUI_SetFontFormName("FONT_CourierNew");
	  //打印字符
		GUI_printf("HelloWorld\r\n");
	  //打印CHGUI版本号
		GUI_printf("CHGUI_Version:%0.2f\r\n", (float)(GUI_VERSION/100));
	  //打印LCDID
		GUI_printf("ID:%X\r\n", GUI_GetDeivceID());
    while(1) 
    {

    }
}

