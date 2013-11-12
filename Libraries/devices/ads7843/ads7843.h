#ifndef __ADS7843_H__
#define __ADS7843_H__
#include "sys.h"
#include "spi.h"



//与触摸屏芯片连接引脚	   
#define GPIO_ADS7843_CS_PORT   PTB
#define GPIO_ADS7843_CS_PIN    GPIO_Pin_20
//定义硬件SPI端口
#define SPI_ADS7843_PORT      SPI2_SCK_PB21_SOUT_PB22_SIN_PB23
#define SPI_ADS7843_CS_PORT   SPI2_PCS0_PB20

#define ADS7843_CS_HIGH        GPIO_WriteBit(GPIO_ADS7843_CS_PORT, GPIO_ADS7843_CS_PIN, Bit_SET)
#define ADS7843_CS_LOW         GPIO_WriteBit(GPIO_ADS7843_CS_PORT, GPIO_ADS7843_CS_PIN, Bit_RESET)


//API funtctions
uint32_t ADS7843_Get_X_Value(void);
uint32_t ADS7843_Get_Y_Value(void);
void ADS7843_Init(void);

#endif

