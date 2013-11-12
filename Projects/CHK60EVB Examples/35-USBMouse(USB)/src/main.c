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

//usb
#include "usb.h"
#include "usb_hid.h"

//超核 飞思卡尔 Kinetis固件库例程
//固件库版本 V2.4
//startup:        启动文件
//devices:        外设芯片支持库(部分实验用到)
//utilities:      软件模块支持库(部分实验用到)
//drivers:        固件库源码

/*
* 入门知识:
* 1- 使用USB中的HID协议 人体学输入设备 模拟一个USB鼠标，在SHELL中输入mouse r 鼠标就像右移动 其他类似
* 2- 注意使用USB BusCLock必须是48*nM  本例程中 就是48M
*/

//实现MINISHELL所需要的PutChar函数
static void Putc(uint8_t data)
{
	UART_SendData(UART4, data);
}
//实现MINISHELL所需要的GetChar函数
static uint8_t Getc(void)
{
	uint8_t ch;
  while(UART_ReceiveData(UART4, &ch) == FALSE);
	return ch;
}

//用户函数 LED控制
int MouseControl(int argc, char *argv[])
{
    //输入内容只有2段(空格为分隔符)   
    if(argc == 2)
		{
        if(!strcmp(argv[1], "r"))
				{
					USB_HID_SetMouse(20,0,0,0);
				}
        if(!strcmp(argv[1], "l"))
				{
					USB_HID_SetMouse(-20,0,0,0);
				}	  
        if(!strcmp(argv[1], "u"))
				{
					USB_HID_SetMouse(0,-20,0,0);
				}
        if(!strcmp(argv[1], "d"))
				{
					USB_HID_SetMouse(0,20,0,0);
				}	  
		}
		else
		{
        MINISHELL_printf("mouse r(l)(u)(d)\r\n");
        MINISHELL_printf("Ex:mouse r (Move Mouse Left)\r\n");
		}
		return 0;
}

//注册 MiniShell函数安装结构
MINISHELL_InstallTypeDef MiniShell_InstallStruct1 = 
{
    .ctrl_putchar = Putc,
    .ctrl_getchar = Getc,
};

//用户函数结构
MINISHELL_CommandTableTypeDef MyCommand1 =
{
    .name = "mouse",
    .maxargs = 2,
    .cmd = MouseControl,
    .usage = "mouse r(l)(u)(d)",
};



int main(void)
{
	  //使用USB 必须CoreClock = 96M
    SystemClockSetup(ClockSource_EX50M,CoreClock_96M);
    PIT_InitTypeDef PIT_InitStruct1;
    DelayInit();
    LED_Init(LED_PinLookup_CHK60EVB, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
	  DisplayCPUInfo();
	     
	  UART_printf("Waitting for USB connect ...\r\n");
    USB_Init(); //初始化USB设备 
    USB_WaitDeviceEnumed(); //等待USB枚举成功
	  UART_printf("USB connected!\r\n");
	
	  //启动定时器
	  PIT_InitStruct1.PITx = PIT0;
	  PIT_InitStruct1.PIT_Interval = 10;
	  PIT_Init(&PIT_InitStruct1);
	  NVIC_EnableIRQ(PIT0_IRQn);
	  PIT_ITConfig(PIT0, PIT_IT_TIF, ENABLE);
	  //启动SHELL
    MINISHELL_Install(&MiniShell_InstallStruct1);
		MINISHELL_Register(&MyCommand1, 1);
	  MINISHELL_Init();
	  //启动SHELL
	  MINISHELL_CmdHandleLoop("SHELL>>");
    while(1) 
    {
    }
}

