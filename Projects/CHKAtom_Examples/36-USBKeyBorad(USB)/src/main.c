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
* 1- 同上一个实验 实现一个USB键盘 注意将usb.desc重名命名而得来. usb_desc有4个 对应4个实验
*/




int main(void)
{
	  //使用USB 必须CoreClock = 96M
    uint8_t FnKey = 0;
    uint8_t Keybuf[6] = {0,0,0,0,0,0};
    SystemClockSetup(ClockSource_EX50M,CoreClock_96M);
    DelayInit();
    LED_Init(LED_PinLookup_CHKATOM, kNumOfLED);
    KBI_Init(KBI_PinLookup_CHKATOM, kNumOfKEY);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
    DisplayCPUInfo();
	     
    UART_printf("Waitting for USB connect ...\r\n");
    USB_Init(); //初始化USB设备 
    USB_WaitDeviceEnumed(); //等待USB枚举成功
    UART_printf("USB connected!\r\n");
	
    while(1) 
    {
        KBI_Scan();
        if(KBI_GetKeyState(kKEY1) == kKBI_SINGLE)
        {
            Keybuf[0] = 4;  //A 的USBHIDKeyCode
            USB_HID_SetKeyBoard(FnKey,Keybuf);
        }
        HID_Proc(); //执行HID进程
        DelayMs(KBI_SCAN_PERIOD_IN_US/1000); //延时
    }
}

