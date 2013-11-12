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

//超核 飞思卡尔 Kinetis固件库例程
//固件库版本 V2.4
//startup:        启动文件
//devices:        外设芯片支持库(部分实验用到)
//utilities:      软件模块支持库(部分实验用到)
//drivers:        固件库源码

/*
* 入门知识:
* 1- MiniShell 超核开发的简易Shell系统
* 2- MiniShell的 使用 首先实现GetChar 和 PutChar函数然后定义安装结构体 传入MINISHELL_Install即可
* 3- 用户函数可由SHELL_Register 注册完成 可以注册最大64个用户函数
* 4- 注册完成后调用MINISHELL_Init()初始化
* 5- 出事后话调用LOOP函数开始SHELL循环
* 6- 打开上位机终端软件 输入命令即可
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

//用户函数 LED控制 这只是一个例子
int LED_Control(int argc, char *argv[])
{
    //输入内容只有2段(空格为分隔符)   
    if(argc == 2)
    {
        if(!strcmp(argv[1], "ON"))
        {
        LED_Ctrl(kLED1, Bit_SET);
        }
        if(!strcmp(argv[1], "OFF"))
        {
            LED_Ctrl(kLED1, Bit_RESET);
        }
    }
    else
    {
        MINISHELL_printf("Unknown Command!\r\n");
    }
    return 0;
}

//注册 MiniShell函数安装结构
MINISHELL_InstallTypeDef MiniShell_InstallStruct1 = 
{
    .ctrl_putchar = Putc,
    .ctrl_getchar = Getc,
};

//用户函数注册结构
MINISHELL_CommandTableTypeDef MyCommand1 =
{
    .name = "LED",            //命令名字
    .maxargs = 2,             //包含的最大参数
    .cmd = LED_Control,       //实现函数接口
    .usage = "LED ON(OFF)",   //用途说明
};


int main(void)
{
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHKATOM, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
    DisplayCPUInfo();
    //安装连接器
    MINISHELL_Install(&MiniShell_InstallStruct1);
    //注册接口函数
    MINISHELL_Register(&MyCommand1, 1);
    //初始化SHELL
    MINISHELL_Init();
    //启动SHELL
    MINISHELL_CmdHandleLoop("SHELL>>");
    while(1) {};

}

