/***********************************************************************************************
 CHKD内部测试版本V0.1
 2012.12.3
************************************************************************************************/
 #include "gpio.h"
/***********************************************************************************************
 功能：初始化引脚GPIO功能
 形参：port:   PTA PTB  PTC PTD PTE
       index:  需要控制的引脚号 0-31
			 dir:    方向  0 输入 1 输出
			 data:   当输出时： 0低电平 1高电平  当输入时： 0 下拉电阻 1 上拉电阻
 返回：0
 详解：GPIO初始化
************************************************************************************************/
void GPIO_Init(GPIO_Type *port,int index,int dir,int data)
{  
	 PORT_Type *p;
	 switch((u32)port)
	 {
	  case PTA_BASE:p=PORTA;SIM->SCGC5|=SIM_SCGC5_PORTA_MASK;break; //开启PORTA口使能时钟，在设置前首先开启使能时钟参见k10手册268页，
	  case PTB_BASE:p=PORTB;SIM->SCGC5|=SIM_SCGC5_PORTB_MASK;break;	//开启PORTB口使能时钟
	  case PTC_BASE:p=PORTC;SIM->SCGC5|=SIM_SCGC5_PORTC_MASK;break;	//开启PORTC口使能时钟
	  case PTD_BASE:p=PORTD;SIM->SCGC5|=SIM_SCGC5_PORTD_MASK;break;	//开启PORTD口使能时钟
	  case PTE_BASE:p=PORTE;SIM->SCGC5|=SIM_SCGC5_PORTE_MASK;break;	//开启PORTE口使能时钟
	  default : break;
	 } 
     p->PCR[index]&=~(PORT_PCR_MUX_MASK);    
     p->PCR[index]|=PORT_PCR_MUX(1);   //配置PORT口的MUX位为GPIO模式，参见k10手册238页
	 if(dir==1)  //设置为输出口
	 {					
	  port->PDDR|=(1<<index);	//配置PORTn口的第index引脚为输出，参见k10手册1484页
	  p->PCR[index]&=~(PORT_PCR_PE_MASK); //作为输出口时关闭该引脚的上下拉电阻功能，参见k10手册240页
	  if(data==1) //设置输出值，参见k10手册1482 页
	    port->PDOR|=(1<<index); //端口输出值为1
	  else
	    port->PDOR&=~(1<<index);//端口输出值为0
	 }
	 else//设置为输入口
	 {
	  port->PDDR&=~(1<<index);//配置PORTn口的第index引脚为输入，参见k10手册1484页
	  if(data==1) //上拉电阻，参见k10手册240页
		p->PCR[index]|=PORT_PCR_PS_MASK;
	  else	//下拉电阻，参见k10手册240页
	  	p->PCR[index]&=~(PORT_PCR_PS_MASK);  
	    p->PCR[index]|=PORT_PCR_PE_MASK; 
	 }
}
/***********************************************************************************************
 功能：控制GPIO引脚的电平
 形参：port:   PTA  PTB  PTC  PTD  PTE
       index:  需要控制的引脚号 0-31
			 data:   0低电平 1高电平
 返回：0
 详解：控制GPIO引脚的电平
************************************************************************************************/
void GPIO_Ctrl(GPIO_Type *port,int index,int data)
{	 
	if(data==1)//设置引脚输出为1，参考k10手册1482页
	{ 	 
		port->PSOR|=1<<index;
	}
	else //设置引脚输出为0，参考k10手册1483页
	{
		port->PCOR|=1<<index;
	}
}
/***********************************************************************************************
 功能：翻转GPIO引脚的电平
 形参：port:   PTA  PTB  PTC  PTD  PTE
       index:  需要控制的引脚号 0-31
 返回：0
 详解：翻转GPIO引脚的电平
************************************************************************************************/
void GPIO_Toggle(GPIO_Type *port,int index)
{
	 port->PTOR|=1<<index;	//将引脚的电平进行翻转，参考k10手册1483页
}
/***********************************************************************************************
 功能：获取引脚的电平状态
 形参：port:   PTA  PTB  PTC  PTD  PTE
       index:  需要控制的引脚号 0-31
 返回：GPIO引脚的电平
 详解：获取引脚的电平状态
************************************************************************************************/
u8 GPIO_Get(GPIO_Type *port,int index)
{	 
	 return((port->PDIR>>index)&1);//获得引脚的电平状态，尽量在输入状态下使用，参考k10手册1484页
}
