/***********************************************************************************************
//CH_Kinetis驱动库  V2.3
//作者    :YANDLD 
//E-MAIL  :yandld@126.com
//修改日期:2013/2/14
//版本：V2.3
//淘宝：http://upcmcu.taobao.com
//QQ    1453363089
//Copyright(C) YANDLD 2012-2022
//All rights reserved
************************************************************************************************/
#include "sys.h"
#include <string.h>
CPUInfoType_t CPUInfo; //处理器运行信息结构体
uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;
/***********************************************************************************************
 功能：获得系统信息
 形参：0
 返回：0
 详解：获得处理器型号 引脚   获得内核时钟,总线时钟，FlexBus时钟，Flash时钟 等     
************************************************************************************************/
void GetCPUInfo(void)
{
	//首先获得复位状态信息
	if (MC->SRSH & MC_SRSH_SW_MASK)     CPUInfo.ResetState = 1;
	if (MC->SRSH & MC_SRSH_LOCKUP_MASK) CPUInfo.ResetState = 2;
	if (MC->SRSH & MC_SRSH_JTAG_MASK)   CPUInfo.ResetState = 3;
	if (MC->SRSL & MC_SRSL_POR_MASK)    CPUInfo.ResetState = 4;
	if (MC->SRSL & MC_SRSL_PIN_MASK)  	CPUInfo.ResetState = 5;
	if (MC->SRSL & MC_SRSL_COP_MASK)    CPUInfo.ResetState = 6;
	if (MC->SRSL & MC_SRSL_LOC_MASK)    CPUInfo.ResetState = 7;
	if (MC->SRSL & MC_SRSL_LVD_MASK)    CPUInfo.ResetState = 8;
	if (MC->SRSL & MC_SRSL_WAKEUP_MASK) CPUInfo.ResetState = 9;
	//确定Kinetis系列型号
	switch((SIM->SDID & SIM_SDID_FAMID(0x7))>>SIM_SDID_FAMID_SHIFT) 
	{  
    	case 0x0: CPUInfo.FamilyType = 10; break;
    	case 0x1: CPUInfo.FamilyType = 20; break;
		  case 0x2: CPUInfo.FamilyType = 30; break;
			case 0x3: CPUInfo.FamilyType = 40; break;
			case 0x4: CPUInfo.FamilyType = 60; break;
			case 0x5: CPUInfo.FamilyType = 70; break;
			case 0x6: CPUInfo.FamilyType = 50; break;
			case 0x7: CPUInfo.FamilyType = 53; break;
			 default: CPUInfo.FamilyType = 0;  break;
	}
	//封装信息
	switch((SIM->SDID & SIM_SDID_PINID(0xF))>>SIM_SDID_PINID_SHIFT) 
	{  
		case 0x02: CPUInfo.PinCnt = 32;  break;
		case 0x04: CPUInfo.PinCnt = 48;  break;
		case 0x05: CPUInfo.PinCnt = 64;  break;
		case 0x06: CPUInfo.PinCnt = 80;  break;
		case 0x07: CPUInfo.PinCnt = 81;  break;
		case 0x08: CPUInfo.PinCnt = 100; break;
		case 0x09: CPUInfo.PinCnt = 104; break;
		case 0x0A: CPUInfo.PinCnt = 144; break;
		case 0x0C: CPUInfo.PinCnt = 196; break;
 		case 0x0E: CPUInfo.PinCnt = 256; break;
	  default:  CPUInfo.PinCnt = 0;   break;	
	}  
   //SiliconRevID
    switch((SIM->SDID & SIM_SDID_REVID(0xF))>>SIM_SDID_REVID_SHIFT) 
    { 
			case 0x0: CPUInfo.SiliconRev = 10;break;
			case 0x1: CPUInfo.SiliconRev = 11;break;
			case 0x2: CPUInfo.SiliconRev = 12;break;
			default:  CPUInfo.SiliconRev = 0; break;
    }	
	//PFlash大小
	switch((SIM->FCFG1 & SIM_FCFG1_PFSIZE(0xF))>>SIM_FCFG1_PFSIZE_SHIFT)
	{
		case 0x7: CPUInfo.PFlashSize = 128*1024;break;
		case 0x9: CPUInfo.PFlashSize = 256*1024;break;
		case 0xB: CPUInfo.PFlashSize = 512*1024;break;
		case 0xF: CPUInfo.PFlashSize = 512*1024;break;
	 	default:  CPUInfo.PFlashSize = 0*1024;  break; 		
	}
	//如果有FlexNVM 确定FlexNVM大小
	if (SIM->FCFG2 & SIM_FCFG2_PFLSH_MASK) 
	{
		CPUInfo.FlexNVMSize = 0;
	}
	else
	{
		//确定FLexNVM大小
		switch((SIM->FCFG1 & SIM_FCFG1_NVMSIZE(0xF))>>SIM_FCFG1_NVMSIZE_SHIFT)
		{
			case 0x0: CPUInfo.FlexNVMSize = 0;break;
			case 0x7: CPUInfo.FlexNVMSize = 128*1024;break;
			case 0x9: CPUInfo.FlexNVMSize = 256*1024;break;
			case 0xF: CPUInfo.FlexNVMSize = 256*1024;break;
			default:  CPUInfo.FlexNVMSize = 0;break; 		
		}
	}
	//RAM大小
	switch((SIM->SOPT1 & SIM_SOPT1_RAMSIZE(0xF))>>SIM_SOPT1_RAMSIZE_SHIFT)
	{
		case 0x5: CPUInfo.RAMSize = 32*1024;  break;
		case 0x7: CPUInfo.RAMSize = 64*1024;  break;
		case 0x8: CPUInfo.RAMSize = 96*1024;  break;
		case 0x9: CPUInfo.RAMSize = 128*1024; break;
		default:  CPUInfo.RAMSize = 0*1024;   break;	
	}
	//时钟计算
	SystemCoreClockUpdate();            //重新计算CoreClock
	CPUInfo.ClkOpt = CLOCK_SETUP;
	CPUInfo.CoreClock = SystemCoreClock;
	CPUInfo.BusClock=(SystemCoreClock/(((SIM->CLKDIV1&SIM_CLKDIV1_OUTDIV2_MASK)>>SIM_CLKDIV1_OUTDIV2_SHIFT)+1));     //计算BusClock
	CPUInfo.FlexBusClock=(SystemCoreClock/(((SIM->CLKDIV1&SIM_CLKDIV1_OUTDIV3_MASK)>>SIM_CLKDIV1_OUTDIV3_SHIFT)+1)); //计算FlexBusClock
	CPUInfo.FlashClock=(SystemCoreClock/(((SIM->CLKDIV1&SIM_CLKDIV1_OUTDIV4_MASK)>>SIM_CLKDIV1_OUTDIV4_SHIFT)+1));   //计算FlashClock
}
/***********************************************************************************************
 功能：初始化系统时钟 
 形参：0
 返回：0
 详解：初始化系统时钟，使用PLL或者FLL 倍频 输出CoreClock BusClock等 
************************************************************************************************/
void SystemInit (void) 
{
	WDOG_Close();
	//系时钟初始化
	//内部晶振 CLOCK_SETUP=0
#if (CLOCK_SETUP == 0)
	//SIM->CLKDIV1 = (u32)0x00110000u; //配置系统预分频器
	SIM->CLKDIV1 = (u32)0xFFFFFFFFu; //配置系统预分频器 先设置为都为最低分频
	// 转到 FEI 模式 
	MCG->C1 = (u8)0x06u;
	MCG->C2 = (u8)0x00u;
	MCG->C4|= (1<<6)|(1<<7)|(1<<5);   //内部参考慢速时钟32.768KHZ  倍频因子 2197 倍频后为96MHZ 参见MCG->C4寄存器
	//分频策略:  
	//SIM_CLKDIV1_OUTDIV1(0) CORE     CLOCK  1分频   UP TO 100M  
	//SIM_CLKDIV1_OUTDIV2(1) BUS      CLOCK  2分频   UP TO 50M 
	//SIM_CLKDIV1_OUTDIV3(1) FlexBus  ClOCK  2分频   UP TO 50M 
	//SIM_CLKDIV1_OUTDIV4(3) Flash    ClOCK  3分频   UP TO 25M 
	SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(3));
  MCG->C5 = (u8)0x00u;
  MCG->C6 = (u8)0x00u;
  while((MCG->S & MCG_S_IREFST_MASK) == 0u);  //检查 FLL参考时钟是内部参考时钟
  while((MCG->S & 0x0Cu) != 0x00u);           //等待FLL被选择
	//外部晶振 CLOCK_SETUP=1
#elif (CLOCK_SETUP == 1)
	SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(3));
  // 转到 FBE 模式 
	OSC->CR = (u8)0x00u;
	SIM->SOPT2 &= (u8)~(u8)0x01u;
	MCG->C2 = (u8)0x24u;
	MCG->C1 = (u8)0x9Au;
	MCG->C4 &= (u8)~(uint8_t)0xE0u;
	MCG->C5 = (u8)0x03u;
	MCG->C6 = (u8)0x00u;
	while((MCG->S & MCG_S_OSCINIT_MASK) == 0u);//检查 FLL参考时钟是内部参考时钟 
	while((MCG->S & MCG_S_IREFST_MASK) != 0u); //检查 FLL参考时钟是内部参考时钟
	while((MCG->S & 0x0Cu) != 0x08u);          //等待 FBE 被选择
	MCG->C5 = (u8)0x03u;
	MCG->C6 = (u8)(0x40u|0x18u);		    //PLL48倍频
	while((MCG->S & MCG_S_PLLST_MASK) == 0u);   //等待PLLS 时钟源转到 PLL
	while((MCG->S & MCG_S_LOCK_MASK) == 0u);    //等待锁定
  //转到PEE
	MCG->C1 = (uint8_t)0x1Au;
	while((MCG->S & 0x0Cu) != 0x0Cu);           //等待PLL输出
	while((MCG->S & MCG_S_LOCK_MASK) == 0u);      //等待PLL锁定
	//外部晶振 超频 CLOCK_SETUP=2
#elif (CLOCK_SETUP == 2)
	SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(7));
  // 转到 FBE 模式 
	OSC->CR = (u8)0x00u;
	SIM->SOPT2 &= (u8)~(u8)0x01u;
	MCG->C2 = (u8)0x24u;
	MCG->C1 = (u8)0x9Au;
	MCG->C4 &= (u8)~(uint8_t)0xE0u;
	MCG->C5 = (u8)0x03u;
	MCG->C6 = (u8)0x00u;
	while((MCG->S & MCG_S_OSCINIT_MASK) == 0u);//检查 FLL参考时钟是内部参考时钟 
	while((MCG->S & MCG_S_IREFST_MASK) != 0u); //检查 FLL参考时钟是内部参考时钟
	while((MCG->S & 0x0Cu) != 0x08u);          //等待 FBE 被选择
	MCG->C5 = (u8)0x01u; //4分频
	MCG->C6 = (u8)(0x40u|MCG_C6_VDIV(26));		    //PLL48倍频
	while((MCG->S & MCG_S_PLLST_MASK) == 0u);   //等待PLLS 时钟源转到 PLL
	while((MCG->S & MCG_S_LOCK_MASK) == 0u);    //等待锁定
  //转到PEE
	MCG->C1 = (uint8_t)0x1Au;
	while((MCG->S & 0x0Cu) != 0x0Cu);           //等待PLL输出
	while((MCG->S & MCG_S_LOCK_MASK) == 0u);      //等待PLL锁定
#elif (CLOCK_SETUP == 3)
	SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(3));
  // 转到 FBE 模式 
	OSC->CR = (u8)0x00u;
	SIM->SOPT2 &= (u8)~(u8)0x01u;
	MCG->C2 = (u8)0x24u;
	MCG->C1 = (u8)0x9Au;
	MCG->C4 &= (u8)~(uint8_t)0xE0u;
	MCG->C5 = (u8)0x03u;
	MCG->C6 = (u8)0x00u;
	while((MCG->S & MCG_S_OSCINIT_MASK) == 0u);//检查 FLL参考时钟是内部参考时钟 
	while((MCG->S & MCG_S_IREFST_MASK) != 0u); //检查 FLL参考时钟是内部参考时钟
	while((MCG->S & 0x0Cu) != 0x08u);          //等待 FBE 被选择
	MCG->C5 = (u8)0x18u; //25分频 /50M/25 =2M 
	MCG->C6 = (u8)(0x40u|MCG_C6_VDIV(24));		    //PLL48倍频  输出 50/25*48=96M 
	while((MCG->S & MCG_S_PLLST_MASK) == 0u);   //等待PLLS 时钟源转到 PLL
	while((MCG->S & MCG_S_LOCK_MASK) == 0u);    //等待锁定
  //转到PEE
	MCG->C1 = (uint8_t)0x1Au;
	while((MCG->S & 0x0Cu) != 0x0Cu);           //等待PLL输出
	while((MCG->S & MCG_S_LOCK_MASK) == 0u);      //等待PLL锁定
#elif (CLOCK_SETUP == 4)
	SIM->CLKDIV1 =(SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV2(1)|SIM_CLKDIV1_OUTDIV3(1)|SIM_CLKDIV1_OUTDIV4(7));
  // 转到 FBE 模式 
	OSC->CR = (u8)0x00u;
	SIM->SOPT2 &= (u8)~(u8)0x01u;
	MCG->C2 = (u8)0x24u;
	MCG->C1 = (u8)0x9Au;
	MCG->C4 &= (u8)~(uint8_t)0xE0u;
	MCG->C5 = (u8)0x03u;
	MCG->C6 = (u8)0x00u;
	while((MCG->S & MCG_S_OSCINIT_MASK) == 0u);//检查 FLL参考时钟是内部参考时钟 
	while((MCG->S & MCG_S_IREFST_MASK) != 0u); //检查 FLL参考时钟是内部参考时钟
	while((MCG->S & 0x0Cu) != 0x08u);          //等待 FBE 被选择
	MCG->C5 = MCG_C5_PRDIV(12); //13分频 /50M/13 = XXM
	MCG->C6 = (u8)(0x40u|MCG_C6_VDIV(28));		    //PLL48倍频  输出 50/13*52=200M 
	while((MCG->S & MCG_S_PLLST_MASK) == 0u);   //等待PLLS 时钟源转到 PLL
	while((MCG->S & MCG_S_LOCK_MASK) == 0u);    //等待锁定
  //转到PEE
	MCG->C1 = (uint8_t)0x1Au;
	while((MCG->S & 0x0Cu) != 0x0Cu);           //等待PLL输出
	while((MCG->S & MCG_S_LOCK_MASK) == 0u);      //等待PLL锁定
#endif 
	GetCPUInfo();  							//计算系统时钟
}
/***********************************************************************************************
 功能：计算主频
 形参：0
 返回：0
 详解：重新计算内核时钟频率 用户在修改中频后必须调用这个函数以计算出新的主频
************************************************************************************************/
void SystemCoreClockUpdate(void)
{
	u32 MCGOUTClock;            //存储MCG模块时钟频率变量
	u8 Divider;
	if ((MCG->C1 & MCG_C1_CLKS_MASK) == 0x0u) 
	{
   //选择FLL或者PLL输出
		if ((MCG->C6 & MCG_C6_PLLS_MASK) == 0x0u) //FLL
	  {
			if ((MCG->C1 & MCG_C1_IREFS_MASK) == 0x0u) 
			{
				if ((SIM->SOPT2 & SIM_SOPT2_MCGCLKSEL_MASK) == 0x0u) 
				{
            MCGOUTClock = CPU_XTAL_CLK_HZ;   //系统晶振驱动 MCG 时钟
				}
				else 
				{ 
					MCGOUTClock = CPU_XTAL32k_CLK_HZ;  //RTC 32 kHz 晶振驱动 MCG 时钟
				} 
				Divider = (uint8_t)(1u << ((MCG->C1 & MCG_C1_FRDIV_MASK) >> MCG_C1_FRDIV_SHIFT));
				MCGOUTClock = (MCGOUTClock / Divider);  //计算FLL参考时钟分频
				if ((MCG->C2 & MCG_C2_RANGE_MASK) != 0x0u) 
				{
					MCGOUTClock /= 32u;  //如果高范围使能，另外的32位分频是有效的
				} 
			} 
	   else 
	   {
        MCGOUTClock = CPU_INT_SLOW_CLK_HZ;     //选择慢的内部参考时钟
       } 
      /* 选择正确的乘数去计算 MCG 输出时钟*/
       switch (MCG->C4 & (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS_MASK)) {
        case 0x0u:
          MCGOUTClock *= 640u;
          break;
        case 0x20u:
          MCGOUTClock *= 1280u;
          break;
        case 0x40u:
          MCGOUTClock *= 1920u;
          break;
        case 0x60u:
          MCGOUTClock *= 2560u;
          break;
        case 0x80u:
          MCGOUTClock *= 732u;
          break;
        case 0xA0u:
          MCGOUTClock *= 1464u;
          break;
        case 0xC0u:
          MCGOUTClock *= 2197u;
          break;
        case 0xE0u:
          MCGOUTClock *= 2929u;
          break;
        default:
          break;
      }
    } 
	else 
	{ 
      /* 选择 PLL */
      Divider = (1u + (MCG->C5 & MCG_C5_PRDIV_MASK));
      MCGOUTClock = (u32)(CPU_XTAL_CLK_HZ / Divider);   //计算 PLL 参考时钟
      Divider = ((MCG->C6 & MCG_C6_VDIV_MASK) + 24u);
      MCGOUTClock *= Divider;        //计算 MCG 输出时钟
    } 
  }
  else if ((MCG->C1 & MCG_C1_CLKS_MASK) == 0x40u) 
     {
       /* 选择内部参考时钟 */
       if ((MCG->C2 & MCG_C2_IRCS_MASK) == 0x0u) 
	   {
         MCGOUTClock = CPU_INT_SLOW_CLK_HZ;    //选择慢的内部参考时钟 
       } 
	   else 
	   { 
         MCGOUTClock = CPU_INT_FAST_CLK_HZ;   //选择内部快的参考时钟 
       } 
     } 
	 else if ((MCG->C1 & MCG_C1_CLKS_MASK) == 0x80u) 
	   {
         /* 选择外部参考时钟*/
         if ((SIM->SOPT2 & SIM_SOPT2_MCGCLKSEL_MASK) == 0x0u) 
		 {
            MCGOUTClock = CPU_XTAL_CLK_HZ;     //系统晶振驱动 MCG 时钟
         } 
		 else 
		 { 
            MCGOUTClock = CPU_XTAL32k_CLK_HZ;  //RTC 32 kHz 晶振驱动 MCG 时钟
         } 
       } 
	   else 
	   { 
          return;
       } 
   SystemCoreClock = (MCGOUTClock / (1u + ((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> SIM_CLKDIV1_OUTDIV1_SHIFT)));
}
/***********************************************************************************************
 功能：进行中断分组设置
 形参：PriorityGroup 填写数字“0~7”
 返回：0
 详解：此程序是进行中断分组管理，一般设置一次，避免重复设置
       一个系统仅能有一种分组，每种分组可以设置16个中断，可以
		   设置多个中断在同一优先级，系统对这样的中断处理是先按照中断向量表
       里的顺序进行处理
************************************************************************************************/
void NVIC_Pgc(u32 PriorityGroup)
{
    u32 reg_value;
    u32 PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07);            
    reg_value  =  SCB->AIRCR;                                    //读旧的寄存器配置 
    reg_value &= ~(SCB_AIRCR_VECTKEY_Msk | SCB_AIRCR_PRIGROUP_Msk);          
    reg_value  =  (reg_value|((uint32_t)0x5FA << SCB_AIRCR_VECTKEY_Pos)|
                    (PriorityGroupTmp << 8));            //写钥匙和优先级分组
    SCB->AIRCR =  reg_value;
}
/***********************************************************************************************
 功能：进行中断优先级设置
 形参：IRQn 填写中断编号，priority 设置优先级
 返回：0
 详解：此函数COPY NVIC_SetPriority(IRQn_Type IRQn, u32 priority)详见core_cm4.h
************************************************************************************************/
void NVIC_Setp(IRQn_Type IRQn, u32 priority)
{
  if(IRQn < 0) {
    SCB->SHP[((u32)(IRQn) & 0xF)-4] = ((priority << (8 - __NVIC_PRIO_BITS)) & 0xff); } 
  else {
    NVIC->IP[(u32)(IRQn)] = ((priority << (8 - __NVIC_PRIO_BITS)) & 0xff);    }   //为中端设置优先级
}
/***********************************************************************************************
 功能：软复位
 形参：0
 返回：0
 详解：软复位
************************************************************************************************/
void SystemSoftReset(void)
{   
	SCB->AIRCR =0X05FA0000|(u32)0x04;	  
} 
/***********************************************************************************************
 功能：使能总中断
 形参：0
 返回：0
 详解：调用  CMSIS Cortex-M Core Function Access 层 函数 在.core_cmFunc.h
************************************************************************************************/
void EnableInterrupts(void)
{
	__enable_irq();
}
/***********************************************************************************************
 功能：关闭总中断
 形参：0
 返回：0
 详解：调用  CMSIS Cortex-M Core Function Access 层 函数 在.core_cmFunc.h
************************************************************************************************/
void DisableInterrupts(void)
{
	__disable_irq();
}
/***********************************************************************************************
 功能：设置中断向量表起始位置
 形参：0
 返回：0
 详解：
************************************************************************************************/
void SetVectorTableAdress(u32 offset)
{
	SCB->VTOR = offset;  //为配合Bootloader更改中断向量表
}

