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
* 1- FAT32文件系统 本例程使用国产的Znfat文件系统 
* 2- Znfat的源代码在znfat目录下 无需用户更改
* 3- 更多关于FAT文件系统的信息 请参阅 Znfat使用说明及软件资料中文件系统一节
* 4- 本例实现一个最最简单的 数码相框 他会轮播SD卡中 PIC文件夹内所有的BMP图片
* 5- 使用时 请将本例程所在文件夹内的PIC文件夹拷贝到SD卡根目录下
*/

//底层操作连接结构 目前的版本只需实现LCD_DrawPoint和 LCD_Init就可以工作，其他填NULL就可以
CHGUI_CtrlOperation_TypeDef gILI9320_OperationStrcut1 = 
{
	SPILCD_Init,        //底层函数与CHGUI无关 LCD_Init的实现在 LCD_CHK60EVB.c中 下同
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
    "CHK60", //设备名称
    0,      //设备号 如不明确填0
    SPILCD_W, //LCD X方向最大值 像素
    SPILCD_H, //LCD Y方向最大值 像素
    &gILI9320_OperationStrcut1,
};

struct znFAT_Init_Args initArgSD; //初始化参数集合
struct znFAT_Init_Args initArgFlash; //初始化参数集合
struct FileInfo fileInfo1,fileInfo2; //文件信息集合
struct DateTime dtInfo1; //日期时间结构体变量

static uint8_t gBMPBuffer[512];
//BMP 显示函数 用户回调函数 用于读取数据
//实际上就是读SD卡中的数据
static uint32_t AppGetData(uint32_t ReqestedByte, uint32_t Offset, uint8_t **ppData)
{
    //返回实际可以读取的数据长度
    if(ReqestedByte > sizeof(gBMPBuffer))
		{
			ReqestedByte = sizeof(gBMPBuffer);
		}
		//读取数据
    znFAT_ReadData(&fileInfo1 ,Offset, ReqestedByte, gBMPBuffer);
		//给出数据指针位置
    *ppData = gBMPBuffer;
		//返回实际读到的长度
    return ReqestedByte;
}


int main(void)
{
    uint8_t bmp_pic_cnt = 0;
    uint32_t i = 0;
    uint8_t res;
    SD_InitTypeDef SD_InitStruct1;
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHKATOM, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
    DisplayCPUInfo();
	
    GUI_Init(&CHGUI_InitStruct1);
    GUI_SetBkColor(BLACK);
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
    GUI_SetColor(LGRAY);
    GUI_printf("HelloWorld\r\n");
    GUI_printf("CHGUI_Version:%0.2f\r\n", (float)(GUI_VERSION/100));
    GUI_printf("ID:%X\r\n", GUI_GetDeivceID());
    GUI_printf("Please Insert Card...\r\n");
    DelayMs(200);
    SD_InitStruct1.SD_BaudRate = 2000000;
    //等待SD卡初始化成功
    while(SD_Init(&SD_InitStruct1) != ESDHC_OK);
    //初始化FAT32
    znFAT_Device_Init(); //设备初始化
    znFAT_Select_Device(0,&initArgSD); //选择SD卡设备
    res = znFAT_Init();
    if(res == ERR_SUCC)
    {
        GUI_printf("Suc. to init FS\r\n");
        GUI_printf("BPB_Sector_No:%d\r\n",initArgSD.BPB_Sector_No);   
        GUI_printf("Total_SizeKB:%d\r\n",initArgSD.Total_SizeKB); 
        GUI_printf("BytesPerSector:%d\r\n",initArgSD.BytesPerSector); 
        GUI_printf("FATsectors:%d\r\n",initArgSD.FATsectors);  
        GUI_printf("SectorsPerClust:%d\r\n",initArgSD.SectorsPerClust); 
        GUI_printf("FirstFATSector:%d\r\n",initArgSD.FirstFATSector); 
        GUI_printf("FirstDirSector:%d\r\n",initArgSD.FirstDirSector); 
        GUI_printf("FSsec:%d\r\n",initArgSD.FSINFO_Sec);
        GUI_printf("Next_Free_Cluster:%d\r\n",initArgSD.Next_Free_Cluster);
        GUI_printf("FreenCluster:%d\r\n",initArgSD.Free_nCluster); 
    }
    else
    {
        GUI_printf("FAT32 Init failed CODE:%d",res);
        while(1);
    }
    //确定有多少BMP
    while(znFAT_Open_File(&fileInfo1, "/PIC/*.bmp", bmp_pic_cnt, 1) == ERR_SUCC)
    {
        bmp_pic_cnt++;
    }
    while(1)
    {
				//打开SD卡中的BMP文件
        znFAT_Open_File(&fileInfo1, "/PIC/*.bmp", i, 1);
			  //画图
        GUI_BMP_DrawEx(0,0, AppGetData);
			  //关闭文件
        znFAT_Close_File(&fileInfo1);
			  //停顿一秒
        DelayMs(1000);
        i++;
			  if(i == bmp_pic_cnt) i = 0;
        LED_Toggle(kLED1);
    }
}

