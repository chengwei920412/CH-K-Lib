#include "ov7620.h"

DMA_InitTypeDef DMA_InitStruct1;
uint8_t DMABuffer[400]; 
OV7620Dev_TypeDef OV7620Dev;
uint8_t CCD_Image[CCD_IMAGE_H][CCD_IMAGE_W];   //图像数据 
//有效行数  只采集 68 71 74 77行 等
uint8_t  CCD_Valid_Line[CCD_IMAGE_H];

//场中断函数
void OV7620_StartTransfer()
{
	GPIO_InitTypeDef GPIO_InitStruct1;
	//将状态设置为开始传输
	if(OV7620Dev.State == OV7620_STATE_IDLE)
	{
		OV7620Dev.State = OV7620_STATE_START;
	}
	//开场中断
	GPIO_InitStruct1.GPIO_Pin = OV7620_VSYNC_PIN;
	GPIO_InitStruct1.GPIO_InitState = Bit_SET;
	GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_RISING;
	GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStruct1.GPIOx = OV7620_VSYNC_PORT;
	GPIO_Init(&GPIO_InitStruct1);
	//GPIO_SetInterrupt(OV7620_VSYNC_PORT,OV7620_VSYNC_PIN,GPIO_IRQC_RISING);
}

//初始化OV7620模块
void OV7620_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct1;
	uint32_t i = 0;
	//有效行数 
	for(i=0;i<sizeof(CCD_Valid_Line);i++)
	{
		CCD_Valid_Line[i] = i*2; //*2这个参数很重要，决定图像高度分辨 
	}
	//空闲状态
	OV7620Dev.State = OV7620_STATE_IDLE;
	//摄像头数据 D0-D7
	for(i=0;i<8;i++)
	{
		GPIO_InitStruct1.GPIO_Pin = i;
		GPIO_InitStruct1.GPIO_InitState = Bit_SET;
		GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DISABLE;
		GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStruct1.GPIOx = OV7620_DATAPORT;
		GPIO_Init(&GPIO_InitStruct1);
	}
	
	//像素中断 PCLK
	GPIO_InitStruct1.GPIO_Pin = OV7620_PCLK_PIN;
	GPIO_InitStruct1.GPIO_InitState = Bit_SET;
	GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DMA_RISING;
	GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStruct1.GPIOx = OV7620_PCLK_PORT;
	GPIO_Init(&GPIO_InitStruct1);
	//行中断 HREF
	GPIO_InitStruct1.GPIO_Pin = OV7620_HREF_PIN;
	GPIO_InitStruct1.GPIO_InitState = Bit_SET;
	GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_RISING;
	GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStruct1.GPIOx = OV7620_HREF_PORT;
	GPIO_Init(&GPIO_InitStruct1);
	// 场中断 VSYNC
	GPIO_InitStruct1.GPIO_Pin = OV7620_VSYNC_PIN;
	GPIO_InitStruct1.GPIO_InitState = Bit_SET;
	GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_RISING;
	GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStruct1.GPIOx = OV7620_VSYNC_PORT;
	GPIO_Init(&GPIO_InitStruct1);
	
	NVIC_EnableIRQ(PORTB_IRQn);
  //配置DMA
  DMA_InitStruct1.Channelx          = DMA_CH1;         //DMA 1通道
  DMA_InitStruct1.PeripheralDMAReq   =PORTC_DMAREQ;    //C端口(PCLK) 上升呀触发    
  DMA_InitStruct1.MinorLoopLength = 170;               //传输次数 超过摄像头每行像素数即可
	DMA_InitStruct1.TransferBytes = 2;                   //每次传输3个字节       
	DMA_InitStruct1.DMAAutoClose  = ENABLE;              //连续采集
	DMA_InitStruct1.EnableState = ENABLE;                //初始化后立即采集
	 
  DMA_InitStruct1.SourceBaseAddr =(uint32_t)&PTD->PDIR;//摄像头端口接D0-D7
	DMA_InitStruct1.SourceMajorInc = 0;                  //地址不增加
	DMA_InitStruct1.SourceDataSize = DMA_SRC_8BIT;       //8BIT数据
	DMA_InitStruct1.SourceMinorInc = 0;

  DMA_InitStruct1.DestBaseAddr =(uint32_t)DMABuffer;  //DMA 内存
	DMA_InitStruct1.DestMajorInc = 0;
	DMA_InitStruct1.DestDataSize = DMA_DST_8BIT;
	DMA_InitStruct1.DestMinorInc = 1;                   //每次传输 +1个字节
}


void HIsr(void); 
void VIsr(void);
/***********************************************************************************************
 功能：PORTB外部中断入口
 形参：0       
 返回：0
 详解：0
************************************************************************************************/
void PORTB_IRQHandler(void)
{
	uint8_t i = 31;
	for(i=31;i>0;i--)  //判断哪个引脚的中断发生，i表示哪个引脚发生的中断
	{
	  if((PORTB->ISFR>>i)==1)break;
	}
	switch(i)
	{ //中断处理
	  case OV7620_HREF_PIN: //行中断
				HIsr();
			break;
	  case OV7620_VSYNC_PIN://场中断
				VIsr();
			break;
    // ...		
	  default : break;
	}
    PORTB->ISFR|=PORT_ISFR_ISF_MASK;   //清除标志位 
}

//场中断
void VIsr()
{
	GPIO_InitTypeDef GPIO_InitStruct1;
	switch(OV7620Dev.State)
	{
		case OV7620_STATE_IDLE:  //空闲
			break;
		case OV7620_STATE_START: //开始后第一个场中断信号到来
			//开行中断
			GPIO_InitStruct1.GPIO_Pin = OV7620_HREF_PIN;
			GPIO_InitStruct1.GPIO_InitState = Bit_SET;
			GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_RISING;
			GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_IPD;
			GPIO_InitStruct1.GPIOx = OV7620_HREF_PORT;
			GPIO_Init(&GPIO_InitStruct1);
		  //计数清0
			OV7620Dev.HREFCnt = 0;
			OV7620Dev.HREFITCnt = 0;
			OV7620Dev.State = OV7620_STATE_COMPLETE; 
			break;
		case OV7620_STATE_COMPLETE: //开始后第二个场中断信号到来，采集完成
			OV7620Dev.TransferCompleteFlag = TRUE;
			//关闭行中断
			GPIO_InitStruct1.GPIO_Pin = OV7620_HREF_PIN;
			GPIO_InitStruct1.GPIO_InitState = Bit_SET;
			GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DISABLE;
			GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_IPD;
			GPIO_InitStruct1.GPIOx = OV7620_HREF_PORT;
			GPIO_Init(&GPIO_InitStruct1);
      //关闭场中断
			GPIO_InitStruct1.GPIO_Pin = OV7620_VSYNC_PIN;
			GPIO_InitStruct1.GPIO_InitState = Bit_SET;
			GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DISABLE;
			GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_IPD;
			GPIO_InitStruct1.GPIOx = OV7620_VSYNC_PORT;
			GPIO_Init(&GPIO_InitStruct1);
		  //传输完成，进入空闲态
			OV7620Dev.State = OV7620_STATE_IDLE;
			OV7620Dev.FiledCnt ++; //场计数
			break;
		default:break;
	}
}

//行同步回调中断函数
void HIsr() //调用行中段回调函数
{
	uint16_t i;
	if(OV7620Dev.HREFITCnt == (CCD_Valid_Line[OV7620Dev.HREFCnt]-1))
	{
		//启动DMA传输
		DMA_Init(&DMA_InitStruct1); //传输一行数据
	}
	if(OV7620Dev.HREFITCnt == CCD_Valid_Line[OV7620Dev.HREFCnt])
	{
		for(i=0;i<CCD_IMAGE_W;i++)
		{
			CCD_Image[OV7620Dev.HREFCnt][i] = DMABuffer[i*2]; //*3这个参数很重要 决定图像宽度
		}
		OV7620Dev.HREFCnt++;
	}
	//行计数
	OV7620Dev.HREFITCnt++;
}
