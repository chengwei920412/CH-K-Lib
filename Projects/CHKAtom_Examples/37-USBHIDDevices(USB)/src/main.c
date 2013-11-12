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
* 1- 实验同上 但是使用USB协议传输HID的自定义数据 下载完成后需要配合例程文件夹中的上位机程序测试
*/




int main(void)
{
	  //使用USB 必须CoreClock = 96M
    uint8_t usb_hid_send_buf[8] ={0,0,0,0,0,0};
    uint8_t usb_hid_rec_buf[8];
    uint8_t usb_hid_rec_cnt = 0; //接收帧计数
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
        usb_hid_send_buf[0] = KBI_GetKeyValue(kKEY1);
        usb_hid_send_buf[1] = KBI_GetKeyValue(kKEY1);	
        USB_HID_SendData(usb_hid_send_buf,8);  //发送数据
        if(USB_HID_RecData(usb_hid_rec_buf) != 0)  //接收到了数据
        {
            LED_Ctrl(kLED1, (usb_hid_rec_buf[0]&0x01)>>0);
            LED_Ctrl(kLED2, (usb_hid_rec_buf[0]&0x02)>>1);		
            usb_hid_rec_cnt++;
					  UART_printf("USB HID Data Received\r\n");
        }
        HID_Proc(); //执行HID进程
			  DelayMs(KBI_SCAN_PERIOD_IN_US/1000); //延时
    }
}
