/**
  ******************************************************************************
  * @file    spilcd.c
  * @author  YANDLD
  * @version V2.4
  * @date    2013.5.23
  * @brief   超核原子核核心板 BSP构件 SPILCD驱动
  ******************************************************************************
  */
#include "spilcd.h"
#include "spi.h"


//函数名：SPILCD_WR_REG
//参  数：无
//        实现写入一字节命令
void SPILCD_WR_REG(uint8_t com)
{
	SPILCD_RS=0;
	SPI_ReadWriteByte(SPILCD_PORT_CS,com,SPI_PCS_Inactive);
}
//函数名：LCD_WR_REG
//参  数：无
//        实现写入一字节数据
void SPILCD_WR_DATA(uint8_t dat)
{
	SPILCD_RS=1;
	SPI_ReadWriteByte(SPILCD_PORT_CS,dat,SPI_PCS_Inactive);
}
//函数名：LCD_WR_REG
//参  数：无
//        准备开始写入GRAM
 void SPILCD_WriteRAM_Prepare(void)
{
	SPILCD_WR_REG(0x2c);   //写RAM
}	 

//函数名：LCD_WR_REG
//参  数：无
//        写GRAM数据
void SPILCD_WriteRAM(uint16_t RGB_Code)
{							    
    SPILCD_WR_DATA(RGB_Code>>8);
    SPILCD_WR_DATA(RGB_Code); 
}
//函数名：LCD_DisplayOn
//参  数：无
//        开启显示
void LCD_DisplayOn(void)
{					   
    SPILCD_WR_REG(0x29);   //
}	 
//函数名：LCD_DisplayOff
//参  数：无
//        关闭显示
void LCD_DisplayOff(void)
{	   
    SPILCD_WR_REG(0x28);   //
}   
//函数名：LCD_SetWindow
//参  数：Xpos:横坐标
//				Ypos:纵坐标
 void SPILCD_SetWindow(uint16_t xstat,uint16_t xend,uint16_t ystat,uint16_t yend)
{
	 SPILCD_WR_REG(0x2A);
   SPILCD_WR_DATA(xstat>>8);
   SPILCD_WR_DATA(xstat);
   SPILCD_WR_DATA(xend>>8);
   SPILCD_WR_DATA(xend);
  
   SPILCD_WR_REG(0x2B);
   SPILCD_WR_DATA(ystat>>8);
   SPILCD_WR_DATA(ystat);
   SPILCD_WR_DATA(yend>>8);
	SPILCD_WR_DATA(yend);
}
//开启反色显示
void LCD_InvDisplayOn()
{
  SPILCD_WR_REG(0x21);  
}
//关闭反色显示
void LCD_InvDisplayOff()
{
  SPILCD_WR_REG(0x20); 
}   

//函数名：SPILCD_Init
//参  数：无
//        LCD 硬件初始化
void SPILCD_Init()
{
	uint32_t delay_cnt = 0;
	SPI_InitTypeDef SPI_InitStruct1;
	GPIO_InitTypeDef GPIO_InitStruct1;
  //初始化RST引脚
	GPIO_InitStruct1.GPIO_Pin = SPILCD_RST_PIN;
	GPIO_InitStruct1.GPIO_InitState = Bit_RESET;
	GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DISABLE;
	GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_OPP;
	GPIO_InitStruct1.GPIOx = SPILCD_RST_PORT;
	GPIO_Init(&GPIO_InitStruct1);
	//初始化RS引脚
	GPIO_InitStruct1.GPIO_Pin = SPILCD_RS_PIN;
	GPIO_InitStruct1.GPIO_InitState = Bit_RESET;
	GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DISABLE;
	GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_OPP;
	GPIO_InitStruct1.GPIOx = SPILCD_RS_PORT;
	GPIO_Init(&GPIO_InitStruct1);
	//初始化SPI串行接口引擎
	SPI_InitStruct1.SPIxDataMap = SPILCD_PORT_DATA;
	SPI_InitStruct1.SPIxPCSMap = SPILCD_PORT_CS;
	SPI_InitStruct1.SPI_DataSize = 8;
	SPI_InitStruct1.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStruct1.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct1.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct1.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct1.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Init(&SPI_InitStruct1);
	
	SPILCD_RST=0;    
	for(delay_cnt=0;delay_cnt<60000;delay_cnt++);
	SPILCD_RST=1;    
	for(delay_cnt=0;delay_cnt<60000;delay_cnt++);	
	SPILCD_WR_REG(0x11);       		  	//关闭睡眠，振荡器工作
	for(delay_cnt=0;delay_cnt<60000;delay_cnt++);	
	
	SPILCD_WR_REG(0x3a);       		  	//每次传送16位数据(VIPF3-0=0101)，每个像素16位(IFPF2-0=101)
	SPILCD_WR_DATA(0x55);						
 
	SPILCD_WR_REG(0x26);       		  	
	SPILCD_WR_DATA(0x04);

	SPILCD_WR_REG(0xf2);              		//Driver Output Control(1)
	SPILCD_WR_DATA(0x01);
	
	SPILCD_WR_REG(0xe0);              		//Driver Output Control(1)
	SPILCD_WR_DATA(0x3f);
	SPILCD_WR_DATA(0x25);
	SPILCD_WR_DATA(0x1c);
	SPILCD_WR_DATA(0x1e);
	SPILCD_WR_DATA(0x20);
	SPILCD_WR_DATA(0x12);
	SPILCD_WR_DATA(0x2a);
	SPILCD_WR_DATA(0x90);
	SPILCD_WR_DATA(0x24);
	SPILCD_WR_DATA(0x11);
	SPILCD_WR_DATA(0x00);
	SPILCD_WR_DATA(0x00);
	SPILCD_WR_DATA(0x00);
	SPILCD_WR_DATA(0x00);
	SPILCD_WR_DATA(0x00);
	 
	SPILCD_WR_REG(0xe1);              //Driver Output Control(1)
	SPILCD_WR_DATA(0x20);
	SPILCD_WR_DATA(0x20);
	SPILCD_WR_DATA(0x20);
	SPILCD_WR_DATA(0x20);
	SPILCD_WR_DATA(0x05);
	SPILCD_WR_DATA(0x00);
	SPILCD_WR_DATA(0x15);
	SPILCD_WR_DATA(0xa7);
	SPILCD_WR_DATA(0x3d);
	SPILCD_WR_DATA(0x18);
	SPILCD_WR_DATA(0x25);
	SPILCD_WR_DATA(0x2a);
	SPILCD_WR_DATA(0x2b);
	SPILCD_WR_DATA(0x2b);  
	SPILCD_WR_DATA(0x3a);  
	
	SPILCD_WR_REG(0xb1);              	//设置屏幕刷新频率
	SPILCD_WR_DATA(0x08);				   	//DIVA=8
	SPILCD_WR_DATA(0x08);				   	//VPA =8，约90Hz
						 
	SPILCD_WR_REG(0xb4);              	//LCD Driveing control
	SPILCD_WR_DATA(0x07);				  	//NLA=1,NLB=1,NLC=1
 
 
	SPILCD_WR_REG(0xc0);              //LCD Driveing control
	SPILCD_WR_DATA(0x0a);
	SPILCD_WR_DATA(0x02);
		
	SPILCD_WR_REG(0xc1);              //LCD Driveing control
	SPILCD_WR_DATA(0x02);

	SPILCD_WR_REG(0xc5);              //LCD Driveing control
	SPILCD_WR_DATA(0x4f);
	SPILCD_WR_DATA(0x5a);

	SPILCD_WR_REG(0xc7);              //LCD Driveing control
	SPILCD_WR_DATA(0x40);
	
	SPILCD_WR_REG(0x2a);              	//配置MCU可操作的LCD内部RAM横坐标起始、结束参数
	SPILCD_WR_DATA(0x00);				   	//横坐标起始地址0x0000
	SPILCD_WR_DATA(0x00);					
	SPILCD_WR_DATA(0x00);				   	//横坐标结束地址0x007f(127)
	SPILCD_WR_DATA(0x7f);
 
	SPILCD_WR_REG(0x2b);              	//配置MCU可操作的LCD内部RAM纵坐标起始结束参数
	SPILCD_WR_DATA(0x00);				   	//纵坐标起始地址0x0000
	SPILCD_WR_DATA(0x00);
	SPILCD_WR_DATA(0x00);				  	//纵坐标结束地址0x009f(159)
	SPILCD_WR_DATA(0x9f);

	SPILCD_WR_REG(0x36);              	//配置MPU和DDRAM对应关系
	SPILCD_WR_DATA(0xc0);					//MX=1,MY=1

	SPILCD_WR_REG(0xb7);              	//LCD Driveing control
	SPILCD_WR_DATA(0x00);				   	//CRL=0
	 
	SPILCD_WR_REG(0x29);   		  	//开启屏幕显示
	SPILCD_WR_REG(0x2c);   			//设置为LCD接收数据/命令模式
	SPILCD_Clear(0x0000);
}


//函数名：SPILCD_Init
//参  数：X Y 坐标 
//        在X Y上打点
void SPILCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
  SPILCD_SetWindow(x,x+1,y,y+1);//设置光标位置 
	SPILCD_WriteRAM_Prepare();     //开始写入GRAM	 
	SPILCD_WriteRAM(color);
} 	 

//函数名：SPILCD_Clear
//参  数：Color 颜色      
void SPILCD_Clear(uint16_t Color)
{
	uint32_t index=0;      
	SPILCD_SetWindow(0,SPILCD_W-1,0,SPILCD_H-1);	 
	SPILCD_WriteRAM_Prepare();     //开始写入GRAM	 	  
	for(index=0;index<SPILCD_W*SPILCD_H;index++)
	{
		SPILCD_WriteRAM(Color);//显示所填充的颜色. 
	}
}  
//函数名：SPILCD_Fill
//参  数：起始终点坐标
void SPILCD_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color)
{                    
	uint32_t n;
	//设置窗口										
  SPILCD_SetWindow(xsta,xend,ysta,yend);
	SPILCD_WriteRAM_Prepare();  //开始写入GRAM	 	   	   
	n=(uint32_t)(yend-ysta+1)*(xend-xsta+1);    
	while(n--){SPILCD_WriteRAM(color);}//显示所填充的颜色. 
	//恢复设置
	SPILCD_SetWindow(0,SPILCD_W-1,0,SPILCD_H-1);	    
}



