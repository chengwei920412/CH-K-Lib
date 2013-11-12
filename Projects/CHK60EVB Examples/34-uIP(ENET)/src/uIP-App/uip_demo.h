#ifndef __UIP_DEMO_H_
#define	__UIP_DEMO_H_

#include "uipopt.h"
#include "sys.h"


//通信程序状态字(用户可以自己定义)  
enum
{
	STATE_CMD		= 0,	//命令接收状态 
	STATE_TX_TEST	= 1,	//连续发送数据包状态(速度测试)  
	STATE_RX_TEST	= 2		//连续接收数据包状态(速度测试)  
};	 

//定义 uip_tcp_appstate_t 数据类型，用户可以添加应用程序需要用到
//成员变量。不要更改结构体类型的名字，因为这个类型名会被uip引用。
//uip.h 中定义的 	struct uip_conn  结构体中引用了 uip_tcp_appstate_t		  
struct tcp_demo_appstate
{
	uint8_t state;
	uint8_t *textptr;
	uint16_t textlen;
};	 

typedef struct tcp_demo_appstate uip_tcp_appstate_t;

#ifndef UIP_APPCALL
#define UIP_APPCALL uip_demo //定义回调函数为 tcp_demo_appcall 
#endif
//本构件实现的函数接口
//TCP服务器
void tcp_server_demo_appcall(void);  
void tcp_server_aborted(void);
void tcp_server_timedout(void);
void tcp_server_closed(void);
void tcp_server_connected(void);
void tcp_server_acked(void);
void tcp_server_senddata(void);
//TCP 服务器 用户接口
uint8_t app_tcp_server_senddata(uint8_t* data,uint16_t len);
uint16_t app_tcp_server_recdata(uint8_t* data);

//TCP客户端
void tcp_client_demo_appcall(void);  
void tcp_client_aborted(void);
void tcp_client_timedout(void);
void tcp_client_closed(void);
void tcp_client_connected(void);
void tcp_client_acked(void);
void tcp_client_senddata(void);
void tcp_client_reconnect(void);
//TCP客户端用户程序接口
uint8_t app_tcp_client_senddata(uint8_t* data,uint16_t len);
uint16_t app_tcp_client_recdata(uint8_t* data);
//本构件实现的接口程序
void uip_demo(void);
#endif

