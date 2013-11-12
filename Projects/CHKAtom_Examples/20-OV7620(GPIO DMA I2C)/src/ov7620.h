
#ifndef __OV7620_H__
#define __OV7620_H__

#include "gpio.h"
#include "dma.h"

#define OV7620_STATE_IDLE          (0)
#define OV7620_STATE_START         (1)
#define OV7620_STATE_COMPLETE      (2)

//摄像头物理分辨率
#define OV7620_W  320
#define OV7620_H  240


// 处理后图像的在内存中的大小  //这个值最好别动
#define CCD_IMAGE_W  111	
#define CCD_IMAGE_H  120      


//引脚定义 数据 PD0-7
#define OV7620_DATAPORT  PTD

//行中断
#define OV7620_HREF_PORT  PTB
#define OV7620_HREF_PIN   0
//场中断
#define OV7620_VSYNC_PORT PTB
#define OV7620_VSYNC_PIN  1
//像素中断
#define OV7620_PCLK_PORT  PTC
#define OV7620_PCLK_PIN  17


typedef struct
{
	uint32_t FiledCnt;         //场计数
	uint8_t TransferCompleteFlag; //传输完成标志 需要用户清0
	uint8_t State;
	uint16_t HREFITCnt;           //行中断计数
	uint16_t HREFCnt;             //有效的行中断计数
}OV7620Dev_TypeDef;

extern OV7620Dev_TypeDef OV7620Dev;
extern uint8_t CCD_Image[CCD_IMAGE_H][CCD_IMAGE_W];   //图像数据

//接口函数
void OV7620_Init(void);
void OV7620_StartTransfer(void);
#endif
