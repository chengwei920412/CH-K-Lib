#ifndef _LCD_CHK60EVB_H_
#define _LCD_CHK60EVB_H_

#include "sys.h"
#include "gpio.h"


#define LCD_DelayMs(x)   DelayMs(x)

#define LCD_USE_HORIZONTAL  


#ifdef LCD_USE_HORIZONTAL

#define LCD_X_MAX   (320)
#define LCD_Y_MAX   (240)

#else

#define LCD_X_MAX   (240)
#define LCD_Y_MAX   (320)

#endif

//RST引脚定义
#define LCD_RST_PORT   PTC
#define LCD_RST_PIN    3

//背光引脚定义
#define LCD_BK_PORT    PTC
#define LCD_BK_PIN     11

//FlexBus总线定义
#define FLEXBUS_BASE_ADDRESS  0x70000000
#define LCD_COMMAND_ADDRESS		*(unsigned short *)0x70000000
#define LCD_DATA_ADDRESS		  *(unsigned short *)0x78000000

//写数据，写命令
#define WMLCDCOM(cmd)	   {LCD_COMMAND_ADDRESS = cmd;}
#define WMLCDDATA(data)	   {LCD_DATA_ADDRESS = data;}





void MLCD_DrawHLine(uint16_t YPos, uint16_t XPos0, uint16_t XPos1, uint16_t Color);
void MLCD_DrawVLine(uint16_t XPos, uint16_t YPos0, uint16_t YPos1, uint16_t Color);
void MLCD_Init(void);
void LCD_DrawPoint(uint16_t XPos, uint16_t YPos, uint16_t Color);
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color);
uint16_t LCD_ReadPoint(uint16_t x,uint16_t y);
uint32_t LCD_GetDeivceID(void);
#endif 


