/**
  ******************************************************************************
  * @file    spilcd.h
  * @author  YANDLD
  * @version V2.4
  * @date    2013.5.23
  * @brief   超核原子核核心板 BSP构件 SPILCD驱动
  ******************************************************************************
  */
#ifndef __SPILCD_H__
#define __SPILCD_H__
#include "gpio.h"
#include "spi.h"

//SPILCD驱动头文件
//端口宏定义
#define SPILCD_RS_PORT    PTA
#define SPILCD_RST_PORT   PTA

#define SPILCD_RS_PIN    12
#define SPILCD_RST_PIN   13

#define SPILCD_RS     PAout(SPILCD_RS_PIN)
#define SPILCD_RST    PAout(SPILCD_RST_PIN)

#define SPILCD_PORT_DATA     (SPI0_SCK_PA15_SOUT_PA16_SIN_PA17)
#define SPILCD_PORT_CS       (SPI0_PCS0_PA14)

//液晶屏尺寸定义
#define SPILCD_W 128
#define SPILCD_H 160

//本构件实现的接口函数
void LCD_InvDisplayOn(void);
void LCD_InvDisplayOff(void);
void SPILCD_Init(void);
void SPILCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
void SPILCD_Clear(uint16_t Color);
void SPILCD_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color);
#endif
