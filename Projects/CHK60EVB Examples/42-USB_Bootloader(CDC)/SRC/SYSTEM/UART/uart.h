/***********************************************************************************************
 CHKD内部测试版本V0.1
 2012.12.3
************************************************************************************************/
#ifndef __UART_H__
#define __UART_H__
#include "sys.h"

//定义printf打印口
#define DEBUG_UART_PORT  UART4       //指定那个串口作为printf输出口
#define DEBUG_UART_BAUD  (115200)    //printf 波特率

//本构件实现的接口函数列表
void UART_Init(UART_Type *uartch,u32 baud);
void UART_Send1(UART_Type *uartch, u8 ch);                                                                 
void UART_SendN (UART_Type *uartch,u8 *buff,u32 len);
u8 UART_Re1 (UART_Type *uartch,u8 *ch);
void UART1_RX_TX_IRQHandler(void);
#endif
