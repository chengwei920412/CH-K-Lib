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

#include "chgui.h"

//超核 飞思卡尔 Kinetis固件库例程
//固件库版本 V2.4
//startup:        启动文件
//devices:        硬件板子资源支持库(部分实验用到)
//utilities:      软件模块支持库(部分实验用到)
//drivers:        固件库源码
//use:            用户代码 包含mian函数和 中断服务程序 
//LCD Devices
#include "spilcd.h"  //物理层 LCD驱动
#include "chgui.h"         //CHGUI基本组件
#include "chgui_char.h"    //CHGUI字符显示组件
#include "chgui_bmp.h"     //CHGUI BMP图片解码组件


#include "ov7620.h"
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
void I2C_ConfigOV7620(void);
void DisplayImage_WithSPILCD(uint16_t x,uint16_t y,uint8_t mode);
uint8_t StrBuffer[30];
int main(void)
{
    uint8_t i = 0;
    I2C_InitTypeDef I2C_InitStruct1;
    I2C_InitStruct1.I2CxMAP = I2C1_SCL_PC10_SDA_PC11;  //Use I2C1,PC10,PC11 to config SCCB
    I2C_InitStruct1.I2C_ClockSpeed = I2C_CLOCK_SPEED_200KHZ;
    //初始化系统时钟 使用外部50M晶振 PLL倍频到100M
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
	   //初始化GUI
    GUI_Init(&CHGUI_InitStruct1);
    //设置背景色为黑色
    GUI_SetBkColor(BLACK);
    //普通字符显示模式
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
    //设置前景色为红色
    GUI_SetColor(LGRAY);
    //LED_Init(LED_PinLookup_CHKATOM, kNumOfLED);
    //KBI_Init(KBI_PinLookup_CHKATOM, kNumOfKEY);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
    UART_printf("HelloWorld\r\n");
    DisplayCPUInfo();
    I2C_Init(&I2C_InitStruct1);
    I2C_ConfigOV7620();
    OV7620_Init();
    UART_printf("OV7620 TEST");
    EnableInterrupts();
    OV7620_StartTransfer();
    while(1)
    {
        if(OV7620Dev.TransferCompleteFlag == TRUE)
        {
            DisplayImage_WithSPILCD(5,15,1);        //显示图像
            OV7620_StartTransfer();                 //开始一场传输
            OV7620Dev.TransferCompleteFlag = FALSE; //不要忘记用完一场数据后将标志位请0
        }
    }
}



//一下是将图像显示在LCD上的函数

//RGB 混色
uint16_t RGB2COLOR(uint8_t RR,uint8_t GG,uint8_t BB)
{
  return (((RR/8)<<11)+((GG/8)<<6)+BB/8); 
}

//显示图像 通过SPILCD
void DisplayImage_WithSPILCD(uint16_t x,uint16_t y,uint8_t mode)
{
	uint16_t i,j; 
	//LCD采用窗口形式打点，以追求最大速度
	SPILCD_SetWindow(x,x+CCD_IMAGE_W-1,y,y+CCD_IMAGE_H+1);									
	SPILCD_WriteRAM_Prepare();        //开始写入GRAM
		for(i=0;i<CCD_IMAGE_H;i++)
		{
		 	for(j=0;j<CCD_IMAGE_W;j++)
			{		
				//二值化
				if(mode == 0)	
				{
				   if(CCD_Image[i][j] > 128) 
					 {
						 SPILCD_WriteRAM(WHITE);
					 }
					 else
					 {
						 SPILCD_WriteRAM(BLACK);
					 }
				} 
				//灰度
				else   	SPILCD_WriteRAM(RGB2COLOR(CCD_Image[i][j],CCD_Image[i][j],CCD_Image[i][j]));
			}
		}
}
 void I2C_ConfigOV7620(void)
{
	//发送地址
	I2C_GenerateSTART(I2C1);
	I2C_Send7bitAddress(I2C1,0x42,I2C_MASTER_WRITE);
	I2C_WaitAck(I2C1);
	//发送需要写入的寄存器地址
	I2C_SendData(I2C1,0x11);
	I2C_WaitAck(I2C1);
	//发送数据
	I2C_SendData(I2C1,0x03);
	I2C_WaitAck(I2C1);
	//结束
	I2C_GenerateSTOP(I2C1);
	while(I2C_IsLineBusy(I2C1) == TRUE);
	I2C_GenerateSTART(I2C1);
	I2C_Send7bitAddress(I2C1,0x42,I2C_MASTER_WRITE);
	I2C_WaitAck(I2C1);
	//发送需要写入的寄存器地址
	I2C_SendData(I2C1,0x14);
	I2C_WaitAck(I2C1);
	//发送数据
	I2C_SendData(I2C1,0x24);
	I2C_WaitAck(I2C1);
	//结束
	I2C_GenerateSTOP(I2C1);
	while(I2C_IsLineBusy(I2C1) == TRUE);
	I2C_GenerateSTART(I2C1);
	I2C_Send7bitAddress(I2C1,0x42,I2C_MASTER_WRITE);
	I2C_WaitAck(I2C1);
	//发送需要写入的寄存器地址
	I2C_SendData(I2C1,0x28);
	I2C_WaitAck(I2C1);
	//发送数据
	I2C_SendData(I2C1,0x20);
	I2C_WaitAck(I2C1);
	//结束
	I2C_GenerateSTOP(I2C1);
	while(I2C_IsLineBusy(I2C1) == TRUE);
}

