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


//超核 飞思卡尔 Kinetis固件库例程
//固件库版本 V2.4
//startup:        启动文件
//devices:        硬件板子资源支持库(部分实验用到)
//utilities:      软件模块支持库(部分实验用到)
//drivers:        固件库源码
//use:            用户代码 包含mian函数和 中断服务程序 

/*
* 入门知识:
* 1- SPI Flash 串行SPIFLASH 使用SPI接口通讯 适合存储一些字库文件等等 
* 2- spiflash.c spiflash.h 利用固件库的SPI驱动提供了基本FLASH操作函数 包括 读写 初始化 等等 
*/

//SPIFLASH 测试函数
#define SPIFLASH_TEST_ADDR   (0x1000)
#define SPIFLASH_TEST_SIZE   (64)

//SPIFLASH 质量测试函数
static void SPIFLASH_Test(void)
{
    uint32_t i;
    uint8_t test_buffer[SPIFLASH_TEST_SIZE];
    SPI_FLASH_Read(test_buffer, SPIFLASH_TEST_ADDR, sizeof(test_buffer));
    UART_printf("Read data form 0x%x:\r\n", SPIFLASH_TEST_ADDR);
    for(i = 0; i < SPIFLASH_TEST_SIZE; i++)
    {
        UART_printf("[%x]:0x%x ", i, test_buffer[i]);
    }
		//写入测试数据
		for(i = 0; i < SPIFLASH_TEST_SIZE; i++)
		{
        test_buffer[i] = i;
		}
		UART_printf("\r\n");
		//写入数据
		SPI_FLASH_Write(test_buffer, SPIFLASH_TEST_ADDR, sizeof(test_buffer));
		UART_printf("Write data completed\r\n");
		memset(test_buffer, 0, sizeof(test_buffer));
		SPI_FLASH_Read(test_buffer, SPIFLASH_TEST_ADDR, sizeof(test_buffer));
    UART_printf("Read data form 0x%x:\r\n", SPIFLASH_TEST_ADDR);
		//打印数据
    for(i = 0; i < SPIFLASH_TEST_SIZE; i++)
    {
        UART_printf("[%x]:0x%x ", i, test_buffer[i]);
    }
}

int main(void)
{
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHK60EVB, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
	  DisplayCPUInfo();
	  //初始化SPIFlash
    SPI_FLASH_Init();
	  UART_printf("SPI Flash Size:%dKB\r\n", SPI_FLASH_GetCapacity()/1024);
    //测试SPIFLASH
	  SPIFLASH_Test();
    while(1) 
    {		
    }
}

