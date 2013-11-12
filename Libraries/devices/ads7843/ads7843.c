#include "ads7843.h"
#include "spi.h"
#include "gpio.h"
#include "delay.h"
#include "stdlib.h"
/***********************************************************************************************
 功能：TP 触摸芯片数据写入函数
 形参：num:写入的数据
 返回：0
 详解：0
************************************************************************************************/
uint8_t ADS7843_ReadWrite_Byte(uint8_t num)    
{
    return (SPI_ReadWriteByte(SPI_ADS7843_CS_PORT, num, SPI_PCS_Inactive) & 0xFF);		    
}


#define ADS7843_CMD_READ_X   (0xD0)
#define ADS7843_CMD_READ_Y   (0X90)


uint16_t TP_Read_AD(uint8_t CMD)	  
{ 	   
	uint16_t Num=0; 
	ADS7843_CS_LOW;
	ADS7843_ReadWrite_Byte(CMD);
	DelayUs(6);//ADS7846的转换时间最长为6us
	Num  = ADS7843_ReadWrite_Byte(0xFF)<<8;
	Num += ADS7843_ReadWrite_Byte(0xFF);
	Num >>= 4;
	ADS7843_CS_HIGH;
	return(Num);   
}

#define ADS7843_ERR_RANGE   30
#define ADS7843_FILTER_DEEP 4
uint32_t ADS7843_Get_X_Value(void)
{
    int x1 = 0, x2 = 0,i;
	  for(i=0;i<ADS7843_FILTER_DEEP;i++)
	  {
	  	  x1 += TP_Read_AD(ADS7843_CMD_READ_X);
	  }
		x1/=ADS7843_FILTER_DEEP;
	  for(i=0;i<ADS7843_FILTER_DEEP;i++)
	  {
	  	  x2 += TP_Read_AD(ADS7843_CMD_READ_X);
	  }
		x2/=ADS7843_FILTER_DEEP;

    if(abs(x1-x2) < ADS7843_ERR_RANGE)
		{
			return (x1+x2)/2;
		}
		return 0;
}


uint32_t ADS7843_Get_Y_Value(void)
{
    int x1 = 0, x2 = 0,i;
	  for(i=0;i<ADS7843_FILTER_DEEP;i++)
	  {
	  	  x1 += TP_Read_AD(ADS7843_CMD_READ_Y);
	  }
		x1/=ADS7843_FILTER_DEEP;
	  for(i=0;i<ADS7843_FILTER_DEEP;i++)
	  {
	  	  x2 += TP_Read_AD(ADS7843_CMD_READ_Y);
	  }
		x2/=ADS7843_FILTER_DEEP;
		
    if(abs(x1-x2) < ADS7843_ERR_RANGE)
		{
			return (x1+x2)/2;
		}
		return 0;
}

void ADS7843_Init(void)
{
    SPI_InitTypeDef SPI_InitStruct1;
    GPIO_InitTypeDef GPIO_InitStruct1;
    //初始化SPI串行接口引擎
    SPI_InitStruct1.SPIxDataMap = SPI_ADS7843_PORT;
    SPI_InitStruct1.SPIxPCSMap = SPI_ADS7843_CS_PORT;
    SPI_InitStruct1.SPI_DataSize = 8;
    SPI_InitStruct1.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStruct1.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct1.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct1.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct1.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(&SPI_InitStruct1);
	
    GPIO_InitStruct1.GPIOx = GPIO_ADS7843_CS_PORT;
    GPIO_InitStruct1.GPIO_InitState = Bit_SET;
    GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DISABLE;
    GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_OPP;
    GPIO_InitStruct1.GPIO_Pin = GPIO_ADS7843_CS_PIN;
    GPIO_Init(&GPIO_InitStruct1);
}





































