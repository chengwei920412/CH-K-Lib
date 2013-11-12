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
* 1- CAN总线实验 需要一个USB-CAN转换器
* 2- 发送ID 10  接收ID 7
* 3- 本实验采用轮询发送接收模式 每隔一段时间发送一帧数据 同事将接收到的信息串口打印 需要USB-CAN上位机配合
*/

CAN_TxMsgTypeDef CAN_TxMsg1;
CAN_RxMsgTypeDef CAN_RxMsg1;

int main(void)
{
	  uint8_t i;
	  uint32_t cnt = 0;
    CAN_InitTypeDef CAN_InitStruct1;
    //使用CAN时 CoreClock 必须是96M
    SystemClockSetup(ClockSource_EX50M,CoreClock_96M);
    DelayInit();
    LED_Init(LED_PinLookup_CHK60EVB, kNumOfLED);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
	  DisplayCPUInfo();
	  //配置CAN初始化信息
    CAN_InitStruct1.CANxMap = CAN1_TX_PE24_RX_PE25;      //PE24 PE25引脚
    CAN_InitStruct1.CAN_BaudRateSelect = CAN_SPEED_125K; //125K 波特率
	  CAN_InitStruct1.FilterEnable = ENABLE;              //不使用过滤ID功能
	  CAN_Init(&CAN_InitStruct1);
	  
	  //配置接收信息
	  CAN_RxMsg1.MBIndex = CAN_MB1;                  //接收邮箱使用CAN_MB1
	  CAN_RxMsg1.DLC = 8;                            //最多接收8个字节 一帧
    CAN_RxMsg1.Id = 7;                             //接收ID
	  CAN_EnableReceiveMB(CAN1, &CAN_RxMsg1);        //使能接收邮箱
	  //配置发送数据
    CAN_TxMsg1.Id = 10;                          //发送ID 10
    CAN_TxMsg1.DLC = 8;                          //每帧8字节
    CAN_TxMsg1.IDE = CAN_IDE_Standard;           //标准数据帧
    CAN_TxMsg1.RTR = CAN_RTR_Data;               //数据帧
    CAN_TxMsg1.MBIndex = CAN_MB2;                //使用邮箱0
		
	  //写入测试数据  
    memcpy(CAN_TxMsg1.Data, "12345678", CAN_TxMsg1.DLC);
    while(1) 
    { 
        if (CAN_Receive(CAN1, &CAN_RxMsg1) == TRUE)
        {
					  //如果接收成功 打印接收数据
            UART_printf("ID:0x%x\r\n",CAN_RxMsg1.Id);
            UART_printf("Data: ");
            for(i = 0; i < CAN_RxMsg1.DLC; i++)
            {
						    UART_printf("0x%x ", CAN_RxMsg1.Data[i]);
            }
						UART_printf("\r\n");
        }
        cnt++; 
				//发送
        if(cnt == 1000000)
        {
            CAN_Transmit(CAN1, &CAN_TxMsg1);
            cnt = 0;
        }				
    }
}

