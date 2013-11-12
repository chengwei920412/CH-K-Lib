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

//nrf24xx
#include "nrf2401.h"

//fat32
#include "znfat.h"

#include <math.h>


//超核 飞思卡尔 Kinetis固件库例程
//固件库版本 V2.4
//startup:        启动文件
//devices:        外设芯片支持库(部分实验用到)
//utilities:      软件模块支持库(部分实验用到)
//drivers:        固件库源码

/*
* 入门知识:
* 1- FAT32文件系统 本例程使用国产的Znfat文件系统 如果初始化成功 会创建一个record.csv的文件记录样本数据 读取完成后用读卡器在PC上查看
* 2- Znfat的源代码在znfat目录下 无需用户更改
* 3- 更多关于FAT文件系统的信息 请参阅 Znfat使用说明及软件资料中文件系统一节
* 4- CSV文件请用Execl打开
*/

struct znFAT_Init_Args initArgSD; //初始化参数集合
struct znFAT_Init_Args initArgFlash; //初始化参数集合
struct FileInfo fileInfo1,fileInfo2; //文件信息集合
struct DateTime dtInfo1; //日期时间结构体变量
//本实验宏定义
#define PI  (3.1415)
//本实验ROM常量区
const uint8_t TextTitle[] = "序号,采样数据1,采样数据2\r\n";
uint8_t recordBuffer[20];
int main(void)
{
    float cnt = 0;
    uint32_t i;
    uint8_t res;
    SD_InitTypeDef SD_InitStruct1;
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHKATOM, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
	  DisplayCPUInfo();
	  UART_printf("Please Insert Card\r\n");
    SD_InitStruct1.SD_BaudRate = 2000000;
    //等待SD卡初始化成功
    while(SD_Init(&SD_InitStruct1) != ESDHC_OK);
    //初始化FAT32
    znFAT_Device_Init(); //设备初始化
    znFAT_Select_Device(0,&initArgSD); //选择SD卡设备
    res = znFAT_Init();
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
        UART_printf("FAT32 Init failed CODE:%d",res);
        while(1);
    }
    res = znFAT_Delete_File("/RECORD/record.csv");
    if(res != ERR_SUCC) 
    {
        UART_printf("File delate failed!, CODE:%d\r\n",res);
    }
    else
    {
        UART_printf("/RECORD/record.csv deleted succ.\r\n");
    }
    UART_printf("Creating Dir...");
    res = znFAT_Create_Dir("/RECORD/",&dtInfo1); //创建目录 
    if((res != ERR_SUCC) && (res != ERR_DIR_ALREADY_EXISTING)) 
    {
        UART_printf("Fail CODE:%d\r\n",res);
        while(1);
    }
    UART_printf("Creating Files...");
    res = znFAT_Create_File(&fileInfo1,"/RECORD/record.csv",&dtInfo1); //创建文件 文件名尽量使用8.3短文件名
    if(res != ERR_SUCC) 
    {
        UART_printf("Failed CODE:%d\r\n",res);
        while(1);
    }
    UART_printf("Writing data...\r\n");
    znFAT_WriteData(&fileInfo1,sizeof(TextTitle),(unsigned char *)TextTitle); //打印表头
	
    //写入数据
    for(cnt=0;cnt<(2*PI);cnt+=0.05)
    {
        i++;
        sprintf((char *)recordBuffer,"%d,%1.3f,%1.3f\r\n",i,sin(cnt),cos(cnt)); //写入sin cos
        UART_printf((char *)recordBuffer);
        znFAT_WriteData(&fileInfo1,strlen((char *)recordBuffer),(unsigned char *)recordBuffer);
    }
    UART_printf("Writing Completed please see CSV file on PC!\r\n");
    znFAT_Close_File(&fileInfo1); //打开一个文件后，必须关闭
    znFAT_Flush_FS();  //必须刷新系统使文件生效
    while(1)
    {
		
    }
}

