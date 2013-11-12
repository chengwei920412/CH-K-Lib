#include "isr.h"



void UART4_RX_TX_IRQHandler(void)
{
	//中断发送处理过程
	UART_SendDataIntProcess(UART4);
	//如果成功接收到了数据

}

