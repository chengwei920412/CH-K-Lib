#include "isr.h"


void UART4_RX_TX_IRQHandler(void)
{
    uint8_t ch;
    //中断发送处理过程
    UART_SendDataIntProcess(UART4);
    //如果成功接收到了数据
    if(UART_ReceiveData(UART4, &ch))
    {
        //把接收到的信息发送回去 echo
        UART_SendData(UART4,ch);
    }
}

void PORTC_IRQHandler(void)
{
    if(GPIO_GetITStates(PTC, GPIO_Pin_17) == SET)
		{
        LED_Toggle(kLED1);
        GPIO_ClearITPendingBit(PTC, GPIO_Pin_17);
		}
    if(GPIO_GetITStates(PTC, GPIO_Pin_18) == SET)
		{
        LED_Toggle(kLED2);
        GPIO_ClearITPendingBit(PTC, GPIO_Pin_18);
		}
}

void PIT0_IRQHandler(void)
{
    LED_Toggle(kLED1);
    UART_printf("Enter PIT0 Int!\r\n");
    PIT_ClearITPendingBit(PIT0, PIT_IT_TIF);
}

void RTC_IRQHandler(void)
{
    RTC_SecondIntProcess();
}


void DMA0_IRQHandler(void)
{
	UART_printf("DMA Transfer complete!\r\n");
	DMA_ClearITPendingBit(DMA0,DMA_IT_MAJOR,DMA_CH0);
}
