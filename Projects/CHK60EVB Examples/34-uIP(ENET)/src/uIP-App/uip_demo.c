#include "uip_demo.h"
#include "uip.h"
#include "httpd.h"

void uip_demo(void)
{
	switch(uip_conn->lport)//本地监听端口80和1200 
	{
		case HTONS(80):
			httpd_appcall();  //HTTP web处理
			break;
		case HTONS(1200):
	    tcp_server_demo_appcall();  //远程1200端口处理 TCP server
			break;
		default:break;					  
		    
	}		    
	switch(uip_conn->rport)	//远程连接1400端口
	{
	    case HTONS(1400):
				tcp_client_demo_appcall(); // 1400端口 TCP client
	      break;
	    default:break;
	}   
}


