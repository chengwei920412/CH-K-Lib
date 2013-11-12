#include "lcd_chk60evb_ucGUI.h"
#include "delay.h"

static void LCD_WriteRegister(uint16_t RegisterIndex, uint16_t Data)
{
    WMLCDCOM(RegisterIndex);
    WMLCDDATA(Data);
}

static uint16_t LCD_ReadRegister(uint16_t RegisterIndex)
{
    WMLCDCOM(RegisterIndex);	
    return (*(uint32_t*)(&LCD_DATA_ADDRESS));
}

void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
#ifdef LCD_USE_HORIZONTAL
	Xpos = LCD_X_MAX - 1 -Xpos;
    LCD_WriteRegister(0x21, Xpos);
		LCD_WriteRegister(0x20, Ypos);
	
#else
		LCD_WriteRegister(0x20, Xpos);
		LCD_WriteRegister(0x21, Ypos);
#endif
}

    

__INLINE void LCD_DrawPoint(uint16_t XPos, uint16_t YPos, uint16_t Color)
{
  LCD_SetCursor(XPos, YPos);//设置光标位置 
	LCD_WriteRegister(0x22, Color);
}


static uint16_t LCD_BGR2RGB(uint16_t c)
{
	uint16_t  r,g,b,rgb;   
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;	 
	rgb=(b<<11)+(g<<5)+(r<<0);		 
	return(rgb);
} 

uint16_t LCD_ReadPoint(uint16_t x,uint16_t y)
{
    LCD_SetCursor(x,y);       //设置光标位置 
    LCD_ReadRegister(0x22);   //读第二次才能读到正确的值
    return LCD_BGR2RGB(LCD_ReadRegister(0x22));
}

uint32_t LCD_GetDeivceID(void)
{
    return LCD_ReadRegister(0x00);
}

void LCD_Clear(uint16_t color)
{
	uint32_t index=0;      
	uint32_t totalpoint = LCD_X_MAX*LCD_Y_MAX;
	LCD_SetCursor(0,0);	//设置光标位置 
	WMLCDCOM(0x22);     //开始写入GRAM	 	  
	for(index = 0; index < totalpoint; index++)
	{
		WMLCDDATA(color);	   
	}
}  

void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{ 
    uint16_t i,j;
    uint16_t len=0;
	  len = ex - sx + 1;	
    for(i = sy; i <= ey; i++)
    {
        LCD_SetCursor(sx, i);      				     
        WMLCDCOM(0x22);    			               
        for(j = 0; j < len; j++) WMLCDDATA(color);	    
    }
}

void MLCD_DrawHLine(uint16_t YPos, uint16_t XPos0, uint16_t XPos1, uint16_t Color)
{
    uint16_t temp;
    if(XPos1 > XPos0)
		{
        temp = XPos1;
			  XPos1 = XPos0;
			  XPos0 =  temp;
		}
    do
		{
			  LCD_DrawPoint(XPos1, YPos, Color);
        XPos1++;
		}while(XPos0 >= XPos1);
}

void MLCD_DrawVLine(uint16_t XPos, uint16_t YPos0, uint16_t YPos1, uint16_t Color)
{
    if(YPos0 > YPos1)
		{
			do
			{
				LCD_DrawPoint(XPos, YPos1, Color);
				YPos1++;
			}while(YPos0 >= YPos1);
		}
		else
		{
			do
			{
				LCD_DrawPoint(XPos, YPos0, Color);
				YPos0++;
			}while(YPos1 >= YPos0);
		}
}


static void LCD_FlexBusInit(void)
{
    uint8_t div = 0;
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
  
    SIM->SOPT2 |= SIM_SOPT2_FBSL(3);
    SIM->SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
    //实测LCD数据频率不能超过12M
    div = (CPUInfo.CoreClock/12000000)-1;
    SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV3(div);
  
    // FB->CS[0].CSMR =1;
    PORTC->PCR[12] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;
    //PORTB->PCR[17] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;
    PORTB->PCR[18] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  fb_ad[15]
    PORTC->PCR[0]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  fb_ad[14]
    PORTC->PCR[1]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  fb_ad[13]
    PORTC->PCR[2]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  fb_ad[12]
    PORTC->PCR[4]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  fb_ad[11]
    PORTC->PCR[5]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  fb_ad[10]
    PORTC->PCR[6]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  fb_ad[9]
    PORTC->PCR[7]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  fb_ad[8]
    PORTC->PCR[8]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  fb_ad[7]
    PORTC->PCR[9]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  fb_ad[6]
    PORTC->PCR[10] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  fb_ad[5]
    PORTD->PCR[2]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  fb_ad[4]
    PORTD->PCR[3]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  fb_ad[3]
    PORTD->PCR[4]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  fb_ad[2]
    PORTD->PCR[5]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  fb_ad[1]
    PORTD->PCR[6]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;           //  fb_ad[0]
 
    //control signals
    PORTB->PCR[19] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;          // fb_oe_b
    PORTD->PCR[1]  = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;          // fb_cs0_b
    PORTC->PCR[16] = PORT_PCR_MUX(5)|PORT_PCR_DSE_MASK;
	
    FB->CS[0].CSAR=FLEXBUS_BASE_ADDRESS;
 
    FB->CS[0].CSMR  =  FB_CSMR_BAM(0x0800) | FB_CSMR_V_MASK; 
    FB->CS[0].CSCR  = FB_CSCR_BLS_MASK   //右对齐
            | FB_CSCR_PS(2)     //16Byte数据  
            | FB_CSCR_AA_MASK;             
  
    FB->CSPMCR = FB_CSPMCR_GROUP3(2); //16-23位输出 BE 选择
}

void MLCD_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct1;
    LCD_FlexBusInit();//初始化FlexBus总线
    
    //RST及背光
    GPIO_InitStruct1.GPIOx = LCD_RST_PORT;
    GPIO_InitStruct1.GPIO_InitState = Bit_RESET;
    GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DISABLE;
    GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_OPP;
    GPIO_InitStruct1.GPIO_Pin = LCD_RST_PIN;
    GPIO_Init(&GPIO_InitStruct1);
	
    GPIO_InitStruct1.GPIOx = LCD_BK_PORT;
    GPIO_InitStruct1.GPIO_InitState = Bit_SET;
    GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DISABLE;
    GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_OPP;
    GPIO_InitStruct1.GPIO_Pin = LCD_BK_PIN;
    GPIO_Init(&GPIO_InitStruct1);
	
    GPIO_WriteBit(LCD_RST_PORT, LCD_RST_PIN, Bit_RESET);
    LCD_DelayMs(10);
    GPIO_WriteBit(LCD_RST_PORT, LCD_RST_PIN, Bit_SET);
    LCD_DelayMs(10); 
    //initializing funciton 1    
    //LCD_WR_REG(0xe5,0x8000);  // Set the internal vcore voltage    
    LCD_WriteRegister(0x00,0x0001);  // start OSC    
    LCD_WriteRegister(0x2b,0x0010);  //Set the frame rate as 80 when the internal resistor is used for oscillator circuit    
    LCD_WriteRegister(0x01,0x0100);  //s720  to  s1 ; G1 to G320    
    LCD_WriteRegister(0x02,0x0700);  //set the line inversion    
    //LCD_WR_REG(0x03,0x1018);  //65536 colors     
    LCD_WriteRegister(0x03,0x1030);    
    //横屏
    #ifdef LCD_USE_HORIZONTAL
    LCD_WriteRegister(0x03,(0<<5)|(0<<4)|(1<<3)|(1<<12));
    #else
    LCD_WriteRegister(0x03,(1<<5)|(1<<4)|(0<<3)|(1<<12));
    #endif

    LCD_WriteRegister(0x04,0x0000);   
    LCD_WriteRegister(0x08,0x0202);  //specify the line number of front and back porch periods respectively    
    LCD_WriteRegister(0x09,0x0000);   
    LCD_WriteRegister(0x0a,0x0000);   
    LCD_WriteRegister(0x0c,0x0000);  //select  internal system clock    
    LCD_WriteRegister(0x0d,0x0000);   
    LCD_WriteRegister(0x0f,0x0000);    
    LCD_WriteRegister(0x50,0x0000);  //0x50 -->0x53 set windows adress    
    LCD_WriteRegister(0x51,0x00ef);   
    LCD_WriteRegister(0x52,0x0000);   
    LCD_WriteRegister(0x53,0x013f);   
    LCD_WriteRegister(0x60,0x2700);   
    LCD_WriteRegister(0x61,0x0001);   
    LCD_WriteRegister(0x6a,0x0000);   
    LCD_WriteRegister(0x80,0x0000);   
    LCD_WriteRegister(0x81,0x0000);   
    LCD_WriteRegister(0x82,0x0000);   
    LCD_WriteRegister(0x83,0x0000);   
    LCD_WriteRegister(0x84,0x0000);   
    LCD_WriteRegister(0x85,0x0000);   
    LCD_WriteRegister(0x90,0x0010);   
    LCD_WriteRegister(0x92,0x0000);   
    LCD_WriteRegister(0x93,0x0003);   
    LCD_WriteRegister(0x95,0x0110);   
    LCD_WriteRegister(0x97,0x0000);   
    LCD_WriteRegister(0x98,0x0000);    
 
    //power setting function    
    LCD_WriteRegister(0x10,0x0000);   
    LCD_WriteRegister(0x11,0x0000);   
    LCD_WriteRegister(0x12,0x0000);   
    LCD_WriteRegister(0x13,0x0000);   
    LCD_DelayMs(20);   
    LCD_WriteRegister(0x10,0x17b0);   
    LCD_WriteRegister(0x11,0x0004);   
    LCD_DelayMs(5);   
    LCD_WriteRegister(0x12,0x013e);   
    LCD_DelayMs(5);   
    LCD_WriteRegister(0x13,0x1f00);   
    LCD_WriteRegister(0x29,0x000f);   
    LCD_DelayMs(5);   
    LCD_WriteRegister(0x20,0x0000);   
    LCD_WriteRegister(0x21,0x0000);   
 
    //initializing function 2    
 
    LCD_WriteRegister(0x30,0x0204);   
    LCD_WriteRegister(0x31,0x0001);   
    LCD_WriteRegister(0x32,0x0000);   
    LCD_WriteRegister(0x35,0x0206);   
    LCD_WriteRegister(0x36,0x0600);   
    LCD_WriteRegister(0x37,0x0500);   
    LCD_WriteRegister(0x38,0x0505);   
    LCD_WriteRegister(0x39,0x0407);   
    LCD_WriteRegister(0x3c,0x0500);   
    LCD_WriteRegister(0x3d,0x0503);   

    //开启显示   
    LCD_WriteRegister(0x07,0x0173);
    //清屏
    //LCD_Clear(lcddev.bcolor);
}





