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

//fat32
#include "znfat.h"

//超核 飞思卡尔 Kinetis固件库例程
//固件库版本 V2.4
//startup:        启动文件
//devices:        外设芯片支持库(部分实验用到)
//utilities:      软件模块支持库(部分实验用到)
//drivers:        固件库源码

/*
* 入门知识:
* 1- FAT32文件系统 本例程使用国产的Znfat文件系统 如果初始化成功 会打印FAT表信息
* 2- Znfat的源代码在znfat目录下 无需用户更改
* 3- 更多关于FAT文件系统的信息 请参阅 Znfat使用说明及软件资料中文件系统一节
*/

struct znFAT_Init_Args initArgSD; //初始化参数集合
struct znFAT_Init_Args initArgFlash; //初始化参数集合
struct FileInfo fileInfo1,fileInfo2; //文件信息集合
struct DateTime dtInfo1; //日期时间结构体变量

int main(void)
{
    uint8_t res;
    SD_InitTypeDef SD_InitStruct1;
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHKATOM, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
	  DisplayCPUInfo();
	  UART_printf("Please Insert Card\r\n");
    SD_InitStruct1.SD_BaudRate = 2000000; //SD卡波特率
    //等待SD卡初始化成功
    while(SD_Init(&SD_InitStruct1) != ESDHC_OK);
    //初始化FAT32
    znFAT_Device_Init(); //设备初始化
    znFAT_Select_Device(0,&initArgSD); //选择SD卡设备
    res = znFAT_Init(); //初始化Znfat
    if(res == ERR_SUCC)
    {
        UART_printf("Suc. to init FS\r\n");
        UART_printf("BPB_Sector_No:%d\r\n",initArgSD.BPB_Sector_No);   
        UART_printf("Total_SizeKB:%d\r\n",initArgSD.Total_SizeKB); 
        UART_printf("BytesPerSector:%d\r\n",initArgSD.BytesPerSector); 
        UART_printf("FATsectors:%d\r\n",initArgSD.FATsectors);  
        UART_printf("SectorsPerClust:%d\r\n",initArgSD.SectorsPerClust); 
        UART_printf("FirstFATSector:%d\r\n",initArgSD.FirstFATSector); 
        UART_printf("FirstDirSector:%d\r\n",initArgSD.FirstDirSector); 
        UART_printf("FSsec:%d\r\n",initArgSD.FSINFO_Sec);
        UART_printf("Next_Free_Cluster:%d\r\n",initArgSD.Next_Free_Cluster);
        UART_printf("FreenCluster:%d\r\n",initArgSD.Free_nCluster); 
    }
    else
    {
        UART_printf("FAT32初始化失败 代码:%d",res);
        while(1);
    }
    while(1) {};
}

