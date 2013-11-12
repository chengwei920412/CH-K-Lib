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
* 1- 内部Flash控制器 可以控制写入芯片内部Flash 但是要小心写入 写入时不能被中断打断或写到 程序区
* 2- 写入操作以Sector 为单位 写入一次会写入整个Sector 需要需要FLASH_WriteByte函数来缓冲
*/

static uint8_t flash_buffer[2048];	 //数据缓冲区
/***********************************************************************************************
 功能：写入Flash数据
 形参：FlashStartAdd:起始地址
    NumByteToWrite  :需要写入的数据长度
	        u8 *buffer: 参数指针
 返回：0
 详解：用户接口函数
************************************************************************************************/
void FLASH_WriteByte(uint32_t FlashStartAdd,uint32_t NumByteToWrite,uint8_t *buffer)
{
	uint32_t i=0;
	uint32_t sectorNo = FlashStartAdd/(1<<11);        //该地址所在的扇区
	uint32_t offset =   FlashStartAdd%(1<<11);        //该地址所在扇区的偏移
	uint32_t remain =   (1<<11)-offset;               //扇区剩余大小
	if(NumByteToWrite<=remain)remain = NumByteToWrite;//不大于4096个字节
	while(1)
	{
		FLASH_ReadByte(sectorNo*(1<<11),(1<<11),flash_buffer);  //取出该扇区所有数据
		FLASH_EraseSector(sectorNo);             //擦除扇区
		for(i=0;i<remain;i++)
		{
			flash_buffer[offset+i]=buffer[i];	   //修改数据
		}
		FLASH_WriteSector(sectorNo,(1<<11),flash_buffer);	//写数据
		if(NumByteToWrite == remain) break;//写入结束了
		else
		{
			sectorNo++;              //扇区地址增1
			offset=0;                //偏移位置为0 	
		  buffer+=remain;       //指针偏移
			FlashStartAdd+=remain;    //写地址偏移	  
			NumByteToWrite-=remain;				//字节数递减
			if(NumByteToWrite > (1<<11)) remain = (1<<11);	//下一个扇区还是写不完
			else remain=NumByteToWrite;			//下一个扇区可以写完了
		}
	}
}

//测试参数
#define FLASH_TEST_BUFFER_SIZE   (64)
#define FLASH_TEST_WRITE_ADDRESS (64)
//TEST函数
void flashTest(void)
{
    uint32_t i;
    uint8_t test_buffer[FLASH_TEST_BUFFER_SIZE];
    FLASH_Init();
    memset(test_buffer, 'A', FLASH_TEST_BUFFER_SIZE);
    FLASH_WriteByte(CPUInfo.PFlashSize-FLASH_TEST_WRITE_ADDRESS, FLASH_TEST_BUFFER_SIZE, test_buffer);  //写入100扇区 0偏移 写入256字节
    memset(test_buffer, 0, FLASH_TEST_BUFFER_SIZE);      //清空数据
    FLASH_ReadByte(CPUInfo.PFlashSize-FLASH_TEST_WRITE_ADDRESS, FLASH_TEST_BUFFER_SIZE, test_buffer);  //读取数据
    for(i = 0; i < FLASH_TEST_BUFFER_SIZE; i++)
    {
		    if(test_buffer[i] != 'A') //数据出错
        {
            UART_printf("FlashTestErr:[%d]:%d It should be A\r\n", i, test_buffer[i]);
		    }
    }
		UART_printf("Flash Test Completed\r\n");
}

int main(void)
{
	
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHK60EVB, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
	  DisplayCPUInfo();
	  //初始化内部Flash控制器
    flashTest();
    
    while(1) 
    {
			  DelayMs(500);
    }
}

