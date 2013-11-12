/***********************************************************************************************
 CHKD内部测试版本V0.1
 2012.12.3
************************************************************************************************/
#include "uart.h"
#include "sys.h"
#include <stdio.h>

/***********************************************************************************************
 功能：UART3串口接收与发送中断服务函数               
 形参：uartch: 串口号           
       ch:    接收到的字节   
 返回：0失败  1 成功
 详解：当串口产生一个接收或者发送信号时，将产生一个中断信号，进入此函数，进行中断处理中断标记自动清除 
************************************************************************************************/
void UART3_RX_TX_IRQHandler(void)
{ 
   u8 ch;
   if(UART_Re1(UART3,&ch))
   {
   		//中断处理
	    //UART_Send1(UART3,ch);	  //例如 发回接收到得数据
   }
}
/***********************************************************************************************
 功能：初始化串口
 形参：uartch:串口号         
       baud:波特率，如9600，115200等      
 返回：0
 详解：在k10芯片上，一共有5个uart，分别为UART0,UART1,UART2,UART3,UART4  
       此函数自动计算主频，所以不需要填写频率
************************************************************************************************/
void UART_Init(UART_Type *uartch,u32 baud)
{	    
  //进行传输速率计算
  u16 sbr;
	u8 brfa; 
	u32 clock;
	GetCPUInfo();  //计算系统时钟
  clock=CPUInfo.BusClock;
	if((u32)uartch==UART0_BASE||(u32)uartch==UART1_BASE) clock=CPUInfo.CoreClock;; //UART0 UART1使用CoreClock
	sbr=(u16)((clock)/(baud*16));
	brfa=((clock*2)/baud-(sbr*32));
	//与uart相关的引脚的时钟使能
	switch((u32)uartch)
    {case UART0_BASE:
	                  SIM->SCGC5|=SIM_SCGC5_PORTD_MASK;  //与uart0相关的引脚的时钟使能，参见手册267页
					  //开启uart0针脚功能 参见手册239页
					  PORTD->PCR[6]&=~PORT_PCR_MUX_MASK;
					  PORTD->PCR[6]=PORT_PCR_MUX(0x2);   //在PTD6上使能UART0_TXD功能
	          PORTD->PCR[7]&=~PORT_PCR_MUX_MASK;
					  PORTD->PCR[7]|=PORT_PCR_MUX(0x2);  //在PTD7上使能UART0_RXD
					  SIM->SCGC4|=SIM_SCGC4_UART0_MASK;	 //使能uart0时钟模块，参见手册265页
				//	  NVIC_EnableIRQ(UART0_RX_TX_IRQn);  //开启接受中断，参见手册1221页,函数来自core_cm4.h
					  break; 
					  		
	  case UART1_BASE:
	                  SIM->SCGC5|=SIM_SCGC5_PORTC_MASK;  //与uart1相关的引脚的时钟使能，参见手册267页
					  //开启uart1针脚功能 参见手册239页
					  PORTC->PCR[4]&=~PORT_PCR_MUX_MASK;
					  PORTC->PCR[4]=PORT_PCR_MUX(0x3);   //在PTC4上使能UART1_TXD功能
	          PORTC->PCR[3]&=~PORT_PCR_MUX_MASK;
					  PORTC->PCR[3]|=PORT_PCR_MUX(0x3);  //在PTC3上使能UART1_RXD
					  SIM->SCGC4|=SIM_SCGC4_UART1_MASK;	 //使能uart1时钟模块，参见手册265页
					//  NVIC_EnableIRQ(UART1_RX_TX_IRQn);  //开启接受中断，参见手册1221页,函数来自core_cm4.h
					  break;
	  case UART2_BASE:
	                  SIM->SCGC5|=SIM_SCGC5_PORTD_MASK;  //与uart2相关的引脚的时钟使能，参见手册267页
					  //开启uart2针脚功能 参见手册239页
					  PORTD->PCR[3]&=~PORT_PCR_MUX_MASK;
					  PORTD->PCR[3]=PORT_PCR_MUX(0x3);   //在PTD3上使能UART2_TXD功能
	          PORTD->PCR[2]&=~PORT_PCR_MUX_MASK;
					  PORTD->PCR[2]|=PORT_PCR_MUX(0x3);  //在PTD2上使能UART2_RXD
					  SIM->SCGC4|=SIM_SCGC4_UART2_MASK;	 //使能uart2时钟模块，参见手册265页
					//  NVIC_EnableIRQ(UART2_RX_TX_IRQn);  //开启接受中断，参见手册1221页,函数来自core_cm4.h
					  break; 
	  case UART3_BASE:
	                  SIM->SCGC5|=SIM_SCGC5_PORTB_MASK;  //与uart3相关的引脚的时钟使能，参见手册267页
					  //开启uart3针脚功能 参见手册239页
					  PORTB->PCR[11]&=~PORT_PCR_MUX_MASK;
					  PORTB->PCR[11]=PORT_PCR_MUX(0x3);   //在PTB11上使能UART3_TXD功能
	          PORTB->PCR[10]&=~PORT_PCR_MUX_MASK;
					  PORTB->PCR[10]|=PORT_PCR_MUX(0x3);  //在PTD10上使能UART3_RXD功能
					  SIM->SCGC4|=SIM_SCGC4_UART3_MASK;	 //使能uart3时钟模块，参见手册265页
					 // NVIC_EnableIRQ(UART3_RX_TX_IRQn);  //开启接受中断，参见手册1221页,函数来自core_cm4.h
					  break; 
	  case UART4_BASE:
	                  SIM->SCGC5|=SIM_SCGC5_PORTC_MASK;  //与uart4相关的引脚的时钟使能，参见手册267页
					  //开启uart4针脚功能 参见手册239页
					  PORTC->PCR[15]&=~PORT_PCR_MUX_MASK;
					  PORTC->PCR[15]=PORT_PCR_MUX(0x3);   //在PTE24上使能UART4_TXD功能
	          PORTC->PCR[14]&=~PORT_PCR_MUX_MASK;
					  PORTC->PCR[14]|=PORT_PCR_MUX(0x3);  //在PTE25上使能UART4_RXD功能
					  SIM->SCGC1|=SIM_SCGC1_UART4_MASK;	 //使能uart4时钟模块，参见手册262页
					  //NVIC_EnableIRQ(UART4_RX_TX_IRQn);  //开启接受中断，参见手册1221页,函数来自core_cm4.h
					  break;
	  default : break;
	}
//配置uart控制寄存器，实现基本的八位传输功能
  uartch->C2&=~(UART_C2_RE_MASK|UART_C2_TE_MASK);	 //禁止发送接受,参见手册1221页
	uartch->C1&=~UART_C1_M_MASK;   //配置数据位数为8位，参见数据手册1220页
	uartch->C1&=~(UART_C1_PE_MASK);//配置为无奇偶校检位，参见数据手册1220页
	uartch->S2&=~UART_S2_MSBF_MASK;//配置为最低位优先传输
//配置传输频率
	uartch->BDH|=((sbr>>8)&UART_BDH_SBR_MASK);//设置高5位的数据，参加手册1218页
	uartch->BDL=(sbr&UART_BDL_SBR_MASK);//设置低8位数据
	uartch->C4|=brfa&(UART_BDL_SBR_MASK>>3);//设置小数位，参见手册1232页
//使能接收器与发送器
	uartch->C2|=(UART_C2_RE_MASK|UART_C2_TE_MASK);	 //开启数据发送接受,参见手册1221页
//接受中断使能
	//uartch->C2|=UART_C2_RIE_MASK; //开启接受中断，参见手册1221页
}
//重定义fputc函数 为了使用printf函数
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout; 
int fputc(int ch,FILE *f)
{
	UART_Send1(DEBUG_UART_PORT,ch);
	return ch;
}
/***********************************************************************************************
 功能：串口发送1个字节
 形参：uartch: 串口号         
       ch    : 发送的数据
 返回：0
 详解：利用串口发送1个字符串
************************************************************************************************/
void UART_Send1(UART_Type *uartch, u8 ch)
{
	//等待发送缓冲区空，参见手册1223页
	while(!(uartch->S1 & UART_S1_TDRE_MASK));
	//发送数据,参见手册1230页
	uartch->D=(u8)ch;
 }
/***********************************************************************************************
 功能：串口发送N个字节
 形参：uartch  : 串口号         
       buff    : 数据指针
			 len     : 数据长度
 返回：0
 详解：它可以用来发送一个文件
************************************************************************************************/
void UART_SendN (UART_Type *uartch,u8 *buff,u32 len)
{
	u32 i;
	for(i=0;i<len;i++)
	{
		UART_Send1(uartch,buff[i]);
	}
}
/***********************************************************************************************
 功能：串口接受1个字节 
 形参：uartch: 串口号           
       ch:    接收到的字节   
 返回：0失败  1 成功
 详解：此处采用查询的方式进行数据接收，有不利因素，尽量使用中断接收的方式     
************************************************************************************************/
uint8_t UART_Re1(UART_Type *UARTx, uint8_t *ch)
{
    if((UARTx->S1 & UART_S1_RDRF_MASK)!= 0)//判断接收缓冲区是否满,参见手册1223页
    {
        *ch =(uint8_t)(UARTx->D);	//接受数据,参见手册1230页
        return 1; 			//接受成功
    }
    return 0;			//如果超时，接受失败
}

///////////////////////////////////////////////
//函数名: UART3_RX_TX_IRQHandler                                                         
//功  能: UART3串口接收与发送中断服务函数                                                 
//参  数: 无                                                 
//返  回: 无                                                    
//说  明: 当串口产生一个接收或者发送信号时，将产生一个中断信号，进入此函数，进行中断处理
//         中断标记自动清除                                                                   
//////////////////////////////////////////////
void UART4_RX_TX_IRQHandler(void)
{ 
   u8 ch;
   if(UART_Re1(UART4,&ch))
   {
   		//中断处理
	    //UART_Send1(UART3,ch);	  //例如 发回接收到得数据
   }
}

