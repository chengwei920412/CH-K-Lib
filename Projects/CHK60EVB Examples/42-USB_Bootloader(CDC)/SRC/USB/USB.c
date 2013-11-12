#include "USB.h"
#include "USB_Desc.h"
#include "usb_hid.h"
#include "usb_cdc.h"
//BDT缓冲区，理论上只要申请512字节，但是由于缓冲区地址的低9位必须为0（不然USB模块无法使用）
//所以申请1024从中找出地址低9位为0的缓冲区地址作为首地址。
__align(512) tBDT tBDTtable[16];													//内部SRAM内存池

//各个端点的数据缓冲区
u8 gu8EP0_OUT_ODD_Buffer[EP0_SIZE];
u8 gu8EP0_OUT_EVEN_Buffer[EP0_SIZE];
u8 gu8EP0_IN_ODD_Buffer[EP0_SIZE];
u8 gu8EP0_IN_EVEN_Buffer[EP0_SIZE];
u8 gu8EP1_OUT_ODD_Buffer[EP1_SIZE];
u8 gu8EP1_OUT_EVEN_Buffer[EP1_SIZE];
u8 gu8EP1_IN_ODD_Buffer[EP1_SIZE];
u8 gu8EP1_IN_EVEN_Buffer[EP1_SIZE];
u8 gu8EP2_OUT_ODD_Buffer[EP2_SIZE];
u8 gu8EP2_OUT_EVEN_Buffer[EP2_SIZE];
u8 gu8EP2_IN_ODD_Buffer[EP2_SIZE];
u8 gu8EP2_IN_EVEN_Buffer[EP2_SIZE];
u8 gu8EP3_OUT_ODD_Buffer[EP3_SIZE];
u8 gu8EP3_OUT_EVEN_Buffer[EP3_SIZE];
u8 gu8EP3_IN_ODD_Buffer[EP3_SIZE];
u8 gu8EP3_IN_EVEN_Buffer[EP3_SIZE];
//指向各个缓冲区的地址指针
u8 *BufferPointer[]=
{
    gu8EP0_OUT_ODD_Buffer,
    gu8EP0_OUT_EVEN_Buffer,
    gu8EP0_IN_ODD_Buffer,
    gu8EP0_IN_EVEN_Buffer,
    gu8EP1_OUT_ODD_Buffer,
    gu8EP1_OUT_EVEN_Buffer,
    gu8EP1_IN_ODD_Buffer,
    gu8EP1_IN_EVEN_Buffer,
    gu8EP2_OUT_ODD_Buffer,
    gu8EP2_OUT_EVEN_Buffer,
    gu8EP2_IN_ODD_Buffer,
    gu8EP2_IN_EVEN_Buffer,
    gu8EP3_OUT_ODD_Buffer,
    gu8EP3_OUT_EVEN_Buffer,
    gu8EP3_IN_ODD_Buffer,
    gu8EP3_IN_EVEN_Buffer
};
//每个缓冲区大小
const u8 cEP_Size[]=
{
    EP0_SIZE,    
    EP0_SIZE,    
    EP0_SIZE,    
    EP0_SIZE,    
    EP1_SIZE,    
    EP1_SIZE,    
    EP1_SIZE,    
    EP1_SIZE,    
    EP2_SIZE,    
    EP2_SIZE,    
    EP2_SIZE,    
    EP2_SIZE,    
    EP3_SIZE,
    EP3_SIZE,
    EP3_SIZE,
    EP3_SIZE
};
//StringDesc描述符指针
const u8* String_Table[4]=
{
    String_Descriptor0,
    String_Descriptor1,
    String_Descriptor2,
    String_Descriptor3
};
//SETUP包后面只能跟DATA0
//端点IN(2)状态
u8 vEP2State = kUDATA1;
//端点OUT(3)状态
u8 vEP3State = kUDATA0;

//保存端点OUT的数据
u8 RecDataBuff[EP3_SIZE];
//数据长度
u32 RecDataLength;

//USB模块内部全局变量
u8 gu8USBClearFlags;      //用于发送和接收 不需要关注
u8 *pu8IN_DataPointer;    //用于发送和接收 不需要关注
u8 gu8IN_Counter;         //用于发送和接收 不需要关注
u8 gu8USB_Toogle_flags;   //用于发送和接收 不需要关注
u8 gu8USB_State;          //保存USB端点0的状态
tUSB_Setup *Setup_Pkt;    //指向端点0OUT数据首地址  
u8 gu8USB_Flags;        //USB接收完成标志位


/*
* USB_DeviceEnumed
*   判断当前CDC设备是否被主机枚举 非阻塞式
*/
u8 USB_IsDeviceEnumed(void)
{
	if(gu8USB_State == uENUMERATED) return 1;
	return 0;
}
/*
* EP_IN_Transfer
*   端点IN传送，在不同USB模式下IN的含义不同
*     在USB主机模式下（USB host）  IN表示  USB Host接收 USB Device的数据包
*     在USB设备模式下（USB Device）IN表示  USB Host向 USB Device发送数据包
*   当前在USB Device模式下，该函数的作用是Device设备发送数据包
*/
void USB_EP_IN_Transfer(u8 u8EP,u8 *pu8DataPointer,u8 u8DataSize)
{
  u8 *pu8EPBuffer;
  u8 u8EPSize;     //端点的数据长度
  u16 u16Lenght=0;    
  u8 u8EndPointFlag;    
  /*调整当前缓冲区的位置*/
  u8EndPointFlag=u8EP;
  if(u8EP)   //如果不是端点0,
    u8EP=(u8)(u8EP<<2);
  u8EP+=2; //EP值加2
  pu8EPBuffer=BufferPointer[u8EP];   //将新EP的BUFFER地址给pu8EPBuffer
  
  if(BIT_CHK(fIN,gu8USBClearFlags)) //如果gu8USBClearFlags = 1
  {
      pu8IN_DataPointer = pu8DataPointer; //将用户数据存储区的首地址给pu8IN_DataPointer
      gu8IN_Counter = u8DataSize;         //将用户数据长度给gu8IN_Counter

      u16Lenght=(Setup_Pkt->wLength_h<<8)+Setup_Pkt->wLength_l ;//将setup数据的长度给u16Lenght
      if((u16Lenght < u8DataSize) && (u8EP==2)) //如果所发送的数据长度大于setup所设定的长度 同时 端点值 == 2
      {
          gu8IN_Counter=Setup_Pkt->wLength_l; //只发送setup中的低8位长度
      }
  }
  /*检查发送长度*/
  if(gu8IN_Counter > cEP_Size[u8EP]) //如果发送数据包的长度 大于32字节时
  {
      u8EPSize = cEP_Size[u8EP];     //将此时端点的长度限制在端点的默认长度
      gu8IN_Counter-=cEP_Size[u8EP]; //将数据包的长度减少EP_Size
      BIT_CLR(fIN,gu8USBClearFlags);//将gu8USBClearFlags清零
  }
  else
  { 
      u8EPSize = gu8IN_Counter;      //如果小于
      gu8IN_Counter=0;            
      BIT_SET(fIN,gu8USBClearFlags);//将gu8USBClearFlags置一
  }
  /*将用户缓冲的区的值复制到EP 缓冲区中准备发送*/
  tBDTtable[u8EP].Cnt=(u8EPSize);    //复制所要发送的数据长度
  while(u8EPSize--)
       *pu8EPBuffer++=*pu8IN_DataPointer++; //将用户的数据赋值给EP存储区                                                        
  if(BIT_CHK(u8EndPointFlag,gu8USB_Toogle_flags)) //如果相应端点的gu8USB_Toogle_flags == 1
  {
      tBDTtable[u8EP].Stat._byte= kUDATA0;         
      BIT_CLR(u8EndPointFlag,gu8USB_Toogle_flags);//清零相应端点的gu8USB_Toogle_flags
  } 
  else
  {
      tBDTtable[u8EP].Stat._byte= kUDATA1;          
      BIT_SET(u8EndPointFlag,gu8USB_Toogle_flags);//置位gu8USB_Toogle_flags
  }
}

/*
* EP_OUT_Transfer
*   端点OUT传送，在不同USB模式下OUT的含义不同
*     在USB主机模式下（USB host）  OUT表示  USB Host向 USB Device发送数据包
*     在USB设备模式下（USB Device）OUT表示  USB Host从 USB Device接收数据包
*   当前在USB Device模式下，该函数的作用是Device设备接收数据包
*   返回值，返回接收到的数据的长度
*/
u8 EP_OUT_Transfer(u8 u8EP,u8 *pu8DataPointer)
{
    u8 *pu8EPBuffer;
    u8 u8EPSize; 
    /* 调整缓冲区的位置 */
    u8EP++;
    /* 分配合适的EP缓冲，EP0是控制点，EP1为输入中断点，EP2是输入BULK点 */
    pu8EPBuffer=BufferPointer[u8EP];
    
    u8EPSize=tBDTtable[u8EP].Cnt;
    u8EP=u8EPSize;
    /* 将EP缓冲的数据复制给用户区 */
    while(u8EPSize--)
      *pu8DataPointer++=*pu8EPBuffer++;
    return(u8EP);
}

/*
* USB_EP_OUT_SizeCheck
*   端点输入数据长度检查
*
*/
u16 USB_EP_OUT_SizeCheck(u8 u8EP)
{
  u8 u8EPSize; 
   /* 读取缓冲区的长度 */
  u8EPSize = tBDTtable[u8EP<<2].Cnt;
  return(u8EPSize & 0x03FF);
}
//=========================================================================
//函数名: hw_usb_set_interface
//功  能: USB模块的描述符 
//参  数: 无
//返  回: 接收到的数据长度
//=========================================================================
void USB_EnableInterface(void)
{
  /* 端点寄存器设置 */
  USB0->ENDPOINT[1].ENDPT= EP1_VALUE | USB_ENDPT_EPHSHK_MASK; //设置端点1为中断端点                       
  USB0->ENDPOINT[2].ENDPT= EP2_VALUE | USB_ENDPT_EPHSHK_MASK; //设置端点2为输入 BULK端点                     
  USB0->ENDPOINT[3].ENDPT= EP3_VALUE | USB_ENDPT_EPHSHK_MASK; //设置端点3为输出 BULK端点                         
  USB0->ENDPOINT[4].ENDPT= EP4_VALUE | USB_ENDPT_EPHSHK_MASK; //禁止端点                        
  USB0->ENDPOINT[5].ENDPT= EP5_VALUE | USB_ENDPT_EPHSHK_MASK; //禁止端点                         
  USB0->ENDPOINT[6].ENDPT= EP6_VALUE | USB_ENDPT_EPHSHK_MASK; //禁止端点    	                   
  
    // 设置 1 BDT 设置
    // 把控制权交给MCU 
    tBDTtable[bEP1IN_ODD].Stat._byte= kUDATA1;                //发送DATA1数据包
    tBDTtable[bEP1IN_ODD].Cnt = 0x00;                         //计数器清空
    tBDTtable[bEP1IN_ODD].Addr =(u32)gu8EP1_IN_ODD_Buffer;    //地址指向对应的缓冲区

    // 设置 2 BDT 设置
    // 把控制权交给MCU 
    tBDTtable[bEP2IN_ODD].Stat._byte= kUDATA1;
    tBDTtable[bEP2IN_ODD].Cnt = 0x00;
    tBDTtable[bEP2IN_ODD].Addr =(u32  )gu8EP2_IN_ODD_Buffer;            

    // 设置 3 BDT 设置
    // 把控制权交给MCU 
    tBDTtable[bEP3OUT_ODD].Stat._byte= kUDATA0;
    tBDTtable[bEP3OUT_ODD].Cnt = 0xFF;
    tBDTtable[bEP3OUT_ODD].Addr =(u32)gu8EP3_OUT_ODD_Buffer;            
}

//描述符处理函数
void USB_GetDescHandler(void)
{
	switch((Setup_Pkt->wValue_h) & 0xFF)
	{
		case DEVICE_DESCRIPTOR:
			#if (DEBUG_PRINT == 1)
			printf("设备描述符\r\n");
			#endif
			USB_EP_IN_Transfer(EP0,(u8*)Device_Descriptor,sizeof(Device_Descriptor));//发送设备描述符
			break;
		case CONFIGURATION_DESCRIPTOR:
			#if (DEBUG_PRINT == 1)
			printf("配置描述符\r\n");
			#endif
			USB_EP_IN_Transfer(EP0,(u8*)Configuration_Descriptor,sizeof(Configuration_Descriptor)); //发送配置描述符
			break;
		case STRING_DESCRIPTOR:
			#if (DEBUG_PRINT == 1)
			printf("字符串描述符-%x ",Setup_Pkt->wValue_l);
			#endif
			switch(Setup_Pkt->wValue_l)  //根据wValue的低字节（索引值）散转
			{
				case 0:  //获取语言ID
					#if (DEBUG_PRINT == 1)
					printf("-语言ID\r\n");
					#endif
					USB_EP_IN_Transfer(EP0,(u8*)String_Table[Setup_Pkt->wValue_l],sizeof(String_Table[Setup_Pkt->wValue_l]));
					break;
				case 1:  //厂商字符串的索引值为1，所以这里为厂商字符串
					#if (DEBUG_PRINT == 1)
					printf("-厂商字符串\r\n");
					#endif
					USB_EP_IN_Transfer(EP0,(u8*)String_Table[Setup_Pkt->wValue_l],sizeof(String_Table[Setup_Pkt->wValue_l]));
					break;
				case 2:  //产品字符串的索引值为2，所以这里为产品字符串
					#if (DEBUG_PRINT == 1)
					printf("-产品字符串\r\n");
					#endif
					USB_EP_IN_Transfer(EP0,(u8*)String_Table[Setup_Pkt->wValue_l],sizeof(String_Table[Setup_Pkt->wValue_l]));
					break;
				case 3:  //产品序列号的索引值为3，所以这里为序列号
					#if (DEBUG_PRINT == 1)
					printf("-序列号\r\n");
					#endif
					USB_EP_IN_Transfer(EP0,(u8*)String_Table[Setup_Pkt->wValue_l],sizeof(String_Table[Setup_Pkt->wValue_l]));
					break;
				default: 
					#if (DEBUG_PRINT == 1)
					printf("-位置的索引值\r\n");
					#endif
					break; 
			}
			break;
				case REPORT_DESCRIPTOR:
					#if (DEBUG_PRINT == 1)
					printf("-报告描述符\r\n");
					#endif
				  #if (USB_DEVICE_CLASS == USB_DEVICE_CLASS_HID)
					USB_EP_IN_Transfer(EP0,(u8*)Report_Descriptor,sizeof(Report_Descriptor));
					#endif
					break;
		default:
			#if (DEBUG_PRINT == 1)
			printf("其他描述符,描述符代码:0x%x\r\n",Setup_Pkt->wValue_h);
			#endif
			break;
	}
}
/***********************************************************************************************
 功能：从USB_Handler 中断散转而来，继续散转 USB_EP0_OUT_Handler
 形参：0
 返回：0
 详解：对SETUP包解包和处理
		(1)只有接收到SETUP包才调用该函数
		(2)SETUP包中8字节数据
			bmRequestType:1
			bRequest:1
			wValue.H:1 :描述符的类型
			wValue.L:1 :描述符的索引
			wIndex:2
			wLength:2
************************************************************************************************/
void USB_EP0_OUT_Handler(void)
{
	u8 u8State;
	u8 *p =(u8*)Setup_Pkt;
	u8 i;
	// 从DATA0 开始传输。
	//BIT_CLR(0,gu8USB_Toogle_flags); //这个取消也可以呀
	//打印接收到的数据
	#if (DEBUG_PRINT == 1)
	for(i=0;i<8;i++,p++) printf("0x%x ",*p); 	printf("\r\n");
	#endif
	if((Setup_Pkt->bmRequestType & 0x80) == 0x80)
	{
		//根据bmRequestType的D6-5位散转，D6-5位表示请求的类型
		//0为标准请求，1为类请求，2为厂商请求
	  switch((Setup_Pkt->bmRequestType>>5) & 0x03)
		{
			case 0:
				#if (DEBUG_PRINT == 1)
				printf("USB标准输入请求-");
				#endif
				//USB协议定义了几个标准输入请求，实现这些标准输入请求即可
				//请求的代码在bRequest中，对不同的请求代码进行散转
				switch(Setup_Pkt->bRequest)
				{
					case mGET_CONFIG:
						#if (DEBUG_PRINT == 1)
						printf("获取配置\r\n");
						#endif
						break;	
					case mGET_DESC:
						#if (DEBUG_PRINT == 1)
						printf("获取描述符-");
						#endif
						USB_GetDescHandler(); //执行获取描述符
						break;
					case mGET_INTF:
						#if (DEBUG_PRINT == 1)
						printf("获取接口\r\n");
						#endif
						break;
					case mGET_STATUS:
						#if (DEBUG_PRINT == 1)
						printf("获取状态\r\n");
						#endif
						break;
					case mSYNC_FRAME:
						#if (DEBUG_PRINT == 1)
						printf("同步帧\r\n");
						#endif
						break;
						default:
						#if (DEBUG_PRINT == 1)
						printf("错误：未定义的标准输入请求\r\n");
						#endif
						break;
				}
				break;
			case 1:
				#if (DEBUG_PRINT == 1)
				printf("USB类输入请求-");
				#endif
					switch(Setup_Pkt->bRequest)
					{
						case GET_LINE_CODING: //GET_LINE_CODING请求
							#if (DEBUG_PRINT == 1)
							printf("GET_LINE_CODING\r\n");
							#endif
							USB_CDC_InterfaceReq_Handler();
						break;
						case SERIAL_STATE: //获取SERIAL_STATE请求
							#if (DEBUG_PRINT == 1)
							printf("SERIAL_STATE\r\n");
							#endif
							#if (USB_DEVICE_CLASS == USB_DEVICE_CLASS_CDC)
							USB_CDC_InterfaceReq_Handler();
						  #endif
						break;
						default:
							#if (DEBUG_PRINT == 1)
							printf("未知 代码:%d\r\n",Setup_Pkt->bRequest);
							#endif
						break;
					}
				break;
			case 2:
				#if (DEBUG_PRINT == 1)
				printf("USB厂商输入请求\r\n");
				#endif
				break;
			default:
				#if (DEBUG_PRINT == 1)
				printf("错误：未定义的输入请求\r\n");
				#endif
				break;
		}	
	}
	else
	{
		//根据bmRequestType的D6-5位散转,D6-5位表示请求的类型
		//0为标准请求，1为类型求 2为厂商请求
		switch((Setup_Pkt->bmRequestType>>5) & 0x03)
		{
			case 0:
				#if (DEBUG_PRINT == 1)
				printf("USB 标准输出请求-");
				#endif
				switch(Setup_Pkt->bRequest)
				{
					case mCLR_FEATURE:
						#if (DEBUG_PRINT == 1)
						printf("清除特性\r\n");
						#endif
						break;
					case mSET_ADDRESS:
						#if (DEBUG_PRINT == 1)
						printf("设置地址 地址:%x\r\n",Setup_Pkt->wValue_l);
						#endif
						gu8USB_State=uADDRESS;
						USB_EP_IN_Transfer(EP0,0,0); //为什么？？？
						break;
					case mSET_CONFIG:
						#if (DEBUG_PRINT == 1)
						printf("设置配置\r\n");
						#endif
            if(Setup_Pkt->wValue_h+Setup_Pkt->wValue_l) 
            {
                //使能1 、2 、3 端点 
							USB_EnableInterface();
							USB_EP_IN_Transfer(EP0,0,0);
							gu8USB_State=uENUMERATED;
            }
						break;
					case  mSET_DESC:
						#if (DEBUG_PRINT == 1)
						printf("设置描述符\r\n");
						#endif
						break;
					case mSET_INTF:
						#if (DEBUG_PRINT == 1)
						printf("设置接口\r\n");
						#endif
						break;
					default:
						#if (DEBUG_PRINT == 1)
						printf("错误：未定义的标准输出请求\r\n");
						#endif
					break;
				}
				break;
				case 1:
					#if (DEBUG_PRINT == 1)
					printf("USB类输出请求-");
					#endif
					switch(Setup_Pkt->bRequest)
					{
						case 0x0A:
							USB_EP_IN_Transfer(EP0,0,0); //等待，发送0数据包即可
							printf("设置空闲\r\n");
							break;
						case SET_CONTROL_LINE_STATE:
							#if (DEBUG_PRINT == 1)
							printf("SET_CONTROL_LINE_STATE\r\n");
							#endif
							USB_CDC_InterfaceReq_Handler();
							break;
						default:
							USB_CDC_InterfaceReq_Handler();
							#if (DEBUG_PRINT == 1)
							printf("未知的请求代码:%d\r\n",Setup_Pkt->bRequest);
							#endif
							break;
					}
					break;
				case 2:
					#if (DEBUG_PRINT == 1)
					printf("USB厂商输出请求r\n");
					#endif
					break;
				default:
					#if (DEBUG_PRINT == 1)
					printf("错误：未定义的输出请求\r\n");
					#endif
				break;
		}
	}
	tBDTtable[bEP0OUT_ODD].Stat._byte= kUDATA0;
	BIT_CLR(USB_CTL_TXSUSPENDTOKENBUSY_SHIFT,USB0->CTL); // 为0 时: SIE 继续处理令牌
}
/***********************************************************************************************
 功能：USB_Init 初始化USB模块   必须保证USB模块输入频率为96M
 形参：0
 返回：0 成功  其他 错误代码
 详解：
************************************************************************************************/
u8 USB_Init(void)
{
	Setup_Pkt=(tUSB_Setup*)BufferPointer[bEP0OUT_ODD];                       //将Setup数据结构体指向对应的缓冲区
  gu8USB_State=uPOWER;                                                     //标记状态为启动阶段
	BIT_SET(FMC_PFAPR_M4AP_MASK,FMC->PFAPR);       //准许写入FLASH
	BIT_SET(SIM_SOPT1_USBREGEN_SHIFT,SIM->SOPT1);  //启动USB电源稳压器
	BIT_CLR(SIM_SOPT1_USBSTBY_SHIFT,SIM->SOPT1);   //USB稳压器停止休眠
  MPU->CESR=0;																	 //取消MPU保护
  BIT_SET(SIM_SOPT2_USBSRC_SHIFT,SIM->SOPT2);    // 使能PLL/FLL为时钟源
	GetCPUInfo();                                  //获得CPU运行数据
	if((CPUInfo.CoreClock < 95000000) || (CPUInfo.CoreClock > 97000000)) //主频不符合
	{
		return 1;
	}
//	if(PUInfo.ClkOpt != 0) BIT_SET(SIM_SOPT2_PLLFLLSEL_SHIFT,SIM->SOPT2);  // 使能PLL输出
//	else                 		BIT_CLR(SIM_SOPT2_PLLFLLSEL_SHIFT,SIM->SOPT2);  // 使能FLL输出
	BIT_CLR(SIM_SOPT2_PLLFLLSEL_SHIFT,SIM->SOPT2);  // 使能FLL输出
  SIM->CLKDIV2|=USB_FARCTIONAL_VALUE;              //USB分频因子设置  //主频96M 分频后50M
  SIM->SCGC4|=(SIM_SCGC4_USBOTG_MASK);             //USB模块时钟门使能
	NVIC_EnableIRQ(USB0_IRQn); //使能USB模块IRQ中断
	//USB模块寄存器配置
	
	USB0->USBTRC0|=USB_USBTRC0_USBRESET_MASK;        //复位USB模块
	while(BIT_CHK(USB_USBTRC0_USBRESET_SHIFT,USB0->USBTRC0)){}; //等待复位完成

	//设置BDT基址寄存器
	//( 低9 位是默认512 字节的偏移) 512 = 16 * 4 * 8 。
	//8 位表示: 4 个字节的控制状态，4 个字节的缓冲区地址 。
	USB0->BDTPAGE1=(u8)((u32)tBDTtable>>8);
	USB0->BDTPAGE2=(u8)((u32)tBDTtable>>16);
	USB0->BDTPAGE3=(u8)((u32)tBDTtable>>24);
	BIT_SET(USB_ISTAT_USBRST_MASK,USB0->ISTAT);    //清除USB模块复位标志
	BIT_SET(USB_INTEN_USBRSTEN_SHIFT,USB0->INTEN); //使能USB模块复位中断
	USB0->USBCTRL=0x40;                             //D-  D+ 下拉
	USB0->USBTRC0|=0x40;                            //强制设置第6位为1  真是纠结，DS上就这么写的
	USB0->CTL|=0x01;                                //USB模块使能
	USB0->CONTROL|=USB_CONTROL_DPPULLUPNONOTG_MASK; //DPP上拉使能
	#if (USB_DEVICE_CLASS == USB_DEVICE_CLASS_CDC)
		USB_CDC_Init();
	#endif
}
//断开USB
void USB_DisConnect()
{
	#if (DEBUG_PRINT == 1)
	printf("USB断开\r\n");
	#endif
	USB0->CONTROL&=~USB_CONTROL_DPPULLUPNONOTG_MASK; //DPP上拉取消
}
//连接USB
void USB_Connect()
{
	#if (DEBUG_PRINT == 1)
	printf("USB连接\r\n");
	#endif
	USB0->CONTROL|=USB_CONTROL_DPPULLUPNONOTG_MASK; //DPP上拉使能
}
/***********************************************************************************************
 功能：USB 复位中断
 形参：0
 返回：0
 详解：禁止所有EP(端点寄存器)，同时将EP0初始为控制端点
       并对EP0的输入、输出奇偶缓冲进行初始化，
			 开启USB令牌中断、USB第一帧传输中断、USB错误中断、USB复位中断
************************************************************************************************/
void USB_ResetHandler(void)
{
	//清标志
	gu8USBClearFlags=0xFF;
	gu8USB_Toogle_flags=0xFE;
	//禁止所有端点 0端点除外
	USB0->ENDPOINT[1].ENDPT=0x00;
	USB0->ENDPOINT[2].ENDPT=0x00;
	USB0->ENDPOINT[3].ENDPT=0x00;
	USB0->ENDPOINT[4].ENDPT=0x00;
	USB0->ENDPOINT[5].ENDPT=0x00;
	USB0->ENDPOINT[6].ENDPT=0x00;
  /*端点0 BDT 启动端点设置*/
  tBDTtable[bEP0OUT_ODD].Cnt = EP0_SIZE;   // EP0 OUT BDT 设置
  tBDTtable[bEP0OUT_ODD].Addr =(u32)gu8EP0_OUT_ODD_Buffer;
  tBDTtable[bEP0OUT_ODD].Stat._byte = kUDATA1;//USB-FS(硬件USB模块) 有专有权访问 BD
                                              //使能USB-FS去扮演数据翻转同步
                                              //定义DATA1允许发送或者接收        
   
  tBDTtable[bEP0OUT_EVEN].Cnt = EP0_SIZE; // EP0 OUT BDT 设置
  tBDTtable[bEP0OUT_EVEN].Addr =(u32)gu8EP0_OUT_EVEN_Buffer;
  tBDTtable[bEP0OUT_EVEN].Stat._byte = kUDATA1;//USB-FS(硬件USB模块) 有专有权访问 BD
                                               //使能USB-FS去扮演数据翻转同步
                                               //定义DATA1允许发送或者接收       
   
  tBDTtable[bEP0IN_ODD].Cnt = EP0_SIZE;   // EP0 IN BDT 设置     
  tBDTtable[bEP0IN_ODD].Addr =(u32)gu8EP0_IN_ODD_Buffer;      
  tBDTtable[bEP0IN_ODD].Stat._byte = kUDATA0;//USB-FS(硬件USB模块) 有专有权访问 BD
                                             //使能USB-FS去扮演数据翻转同步
                                             //定义DATA0允许发送或者接收 
   
  tBDTtable[bEP0IN_EVEN].Cnt = EP0_SIZE;  // EP0 IN BDT 设置            
  tBDTtable[bEP0IN_EVEN].Addr =(u32)gu8EP0_IN_EVEN_Buffer;      
  tBDTtable[bEP0IN_EVEN].Stat._byte = kUDATA0;//USB-FS(硬件USB模块) 有专有权访问 BD
                                              //使能USB-FS去扮演数据翻转同步
                                              //定义DATA0允许发送或者接收          
	USB0->ENDPOINT[0].ENDPT=0x0D; // 使能 EP0 开启发送 接受时能
	USB0->ERRSTAT=0xFF;           // 清除所有的错误
	USB0->ISTAT=0xFF;             // 清除所有的中断标志
	USB0->ADDR=0x00;  					  // USB枚举时的默认设备地址0
	USB0->ERREN=0xFF;             // 使能所有的错误中断
	// USB模块中断使能
	BIT_SET(USB_INTEN_TOKDNEEN_SHIFT,USB0->INTEN);
	BIT_SET(USB_INTEN_SOFTOKEN_SHIFT,USB0->INTEN);
	BIT_SET(USB_INTEN_ERROREN_SHIFT,USB0->INTEN); 
	BIT_SET(USB_INTEN_USBRSTEN_SHIFT,USB0->INTEN);    
}

void USB_EP0_IN_Handler(void)
{
    if(gu8USB_State==uADDRESS)
    {
        USB0->ADDR = Setup_Pkt->wValue_l; //写入地址
        gu8USB_State=uREADY;              //变为Ready状态
        BIT_SET(fIN,gu8USBClearFlags);   //
				#if (DEBUG_PRINT == 1)
				printf("新地址：%d\r\n",USB0->ADDR);
				#endif
    }
		USB_EP_IN_Transfer(EP0,0,0); 
}
void USB_EP2_IN_Handler(void)
{
	if(USB_DEVICE_CLASS == USB_DEVICE_CLASS_HID) //如果是HID设备
	{
		USB_HID_SendReport();
	}
// tBDTtable[bEP2IN_ODD].Stat._byte = kUDATA0;
//	BIT_CLR(USB_CTL_TXSUSPENDTOKENBUSY_SHIFT,USB0->CTL);
}
void USB_EP2_OUT_Handler(void)
{
	
}
void USB_EP3_IN_Handler(void)
{
	
}
extern u8 RecFlag;
void USB_EP3_OUT_Handler(void)
{
	if(USB_DEVICE_CLASS == USB_DEVICE_CLASS_HID) //如果是HID设备
	{
		RecFlag = 1; 
	}
	if(USB_DEVICE_CLASS == USB_DEVICE_CLASS_CDC) //CDC设备
	{
		
	}
//	tBDTtable[EP3<<2].Stat._byte= kSIE;
//  tBDTtable[bEP3OUT_ODD].Cnt = EP3_SIZE;
}
/***********************************************************************************************
 功能：从USB0_IRQHandler 硬件中断散转而来，继续散转
 形参：0
 返回：0
 详解：从USB0_IRQHandler 硬件中断散转而来，继续进行散转
************************************************************************************************/
void USB_Handler(void)
{
	u8 u8EndPoint;
	u8 u8IN;
  u8IN = USB0->STAT & 0x08;    //获得当前的传输状态，1发送；0接收
  u8EndPoint = USB0->STAT >> 4;//获得当前接收令牌的端点地址
	if(u8EndPoint == 0) //端点0
	{
		if(u8IN) //IN
		{
			#if (DEBUG_PRINT == 1)
			printf("端点0输入中断处理\r\n");
			#endif
			USB_EP0_IN_Handler(); //端点0发送包
		}
		else    //OUT
		{
			#if (DEBUG_PRINT == 1)
			printf("端点0输出中断处理-");
			#endif
			BIT_SET(u8EndPoint,gu8USB_Flags);  //设置 gu8USB_Flags = 1   
	//		usbMCU_CONTROL(u8EndPoint);   
			USB_EP0_OUT_Handler();
		}
	}
	if(u8EndPoint == 1) //端点1
	{
		if(u8IN) //IN
		{
			#if (DEBUG_PRINT == 1)
			printf("端点1输入中断处理\r\n");
			#endif
			//USB_EP1_IN_Handler();
		}
		else //OUT
		{
			#if (DEBUG_PRINT == 1)
			printf("端点1输出中断处理\r\n");
			#endif
			//USB_EP1_OUTHandler();	
			BIT_SET(u8EndPoint,gu8USB_Flags);  //设置 gu8USB_Flags = 1   
	//		usbMCU_CONTROL(u8EndPoint);    
		}
	}
	if(u8EndPoint == 2)//端点2
	{
		if(u8IN) //IN
		{
			#if (DEBUG_PRINT == 1)
			printf("端点2输入中断处理\r\n");
			#endif
			USB_EP2_IN_Handler();
		}
		else //OUT
		{
			#if (DEBUG_PRINT == 1)
			printf("端点2输出中断处理\r\n");
			#endif
			USB_EP2_OUT_Handler();
			BIT_SET(u8EndPoint,gu8USB_Flags);  //设置 gu8USB_Flags = 1   
		}	
	}
	if(u8EndPoint == 3)//端点3
	{
		if(u8IN) //IN
		{
			#if (DEBUG_PRINT == 1)
			printf("端点3输入中断处理\r\n");
			#endif
			USB_EP3_IN_Handler();
		}
		else //OUT
		{
			#if (DEBUG_PRINT == 1)
			printf("端点3输出中断处理\r\n");
			#endif
			USB_EP3_OUT_Handler();
			BIT_SET(u8EndPoint,gu8USB_Flags);  //设置 gu8USB_Flags = 1    
		}	
		
	}
}

/***********************************************************************************************
 功能：硬件USB0 中断
 形参：0
 返回：0
 详解：开始USB散转操作  
************************************************************************************************/
void USB0_IRQHandler(void)
{
	u8 err;
	//检测USB 模块是否解析到有效的复位。
	if(BIT_CHK(USB_ISTAT_USBRST_SHIFT,USB0->ISTAT)) 
	{
		#if (DEBUG_PRINT == 1)
		printf("USB总线复位\r\n");
		#endif
		USB_ResetHandler();
	}
	//收到SOF包
	if(BIT_CHK(USB_ISTAT_SOFTOK_SHIFT,USB0->ISTAT)) 
	{
		#if (DEBUG_PRINT == 1)
		//printf("收到SOF包\r\n");
		#endif
		USB0->ISTAT = USB_ISTAT_SOFTOK_MASK;   
	}
	//收到STALL包
	if(BIT_CHK(USB_ISTAT_STALL_SHIFT,USB0->ISTAT))
	{
		#if (DEBUG_PRINT == 1)
		printf("收到STALL包\r\n");
		#endif
		if(BIT_CHK(USB_ENDPT_EPSTALL_SHIFT,USB0->ENDPOINT[0].ENDPT))
		BIT_CLR(USB_ENDPT_EPSTALL_SHIFT,USB0->ENDPOINT[0].ENDPT);
    BIT_SET(USB_ISTAT_STALL_SHIFT,USB0->ISTAT);
	}
	//令牌完成中断
	if(BIT_CHK(USB_ISTAT_TOKDNE_SHIFT,USB0->ISTAT)) 
	{
		BIT_SET(USB_CTL_ODDRST_SHIFT,USB0->CTL);//指向BDT EVEN 组
		USB_Handler(); //调用USB Handler
		//清除令牌完成中断
		BIT_SET(USB_ISTAT_TOKDNE_SHIFT,USB0->ISTAT);
	}
	//SLEEP 
	if(BIT_CHK(USB_ISTAT_SLEEP_SHIFT,USB0->ISTAT)) 
	{
		#if (DEBUG_PRINT == 1)
		printf("SLEEP中断\r\n");
		#endif
		BIT_SET(USB_ISTAT_SLEEP_SHIFT,USB0->ISTAT);        
	}
	// 错误
	if(BIT_CHK(USB_ISTAT_ERROR_SHIFT,USB0->ISTAT))
	{
		err = USB0->ERRSTAT;
		#if (DEBUG_PRINT == 1)
		printf("错误 错误代码:%d\r\n",err);
		#endif
		BIT_SET(USB_ISTAT_ERROR_SHIFT,USB0->ISTAT);
		USB0->ERRSTAT=0xFF; //清除所有中断错误
	}
}



