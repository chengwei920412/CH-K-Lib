#include "sys.h"
#include "uart.h"
#include "gpio.h"
#include "adc.h"
#include "can.h"
#include "dac.h"
#include "delay.h"
#include "dma.h"
#include "enet.h"
#include "flash.h"
#include "ftm.h"
#include "i2c.h"
#include "lptm.h"
#include "pdb.h"
#include "pit.h"
#include "rtc.h"
#include "sd.h"
#include "spi.h"
#include "tsi.h"
#include "wdog.h"

//LED Devices
#include "led.h"
#include "led_chk60evb.h"
//KBI Devices
#include "kbi.h"
#include "kbi_chk60evb.h"
//SPI Flash Devices;
#include "spiflash.h"
//LCD Devices
#include "lcd_chk60evb.h"  
#include "ads7843.h"

//CHGUI 
#include "chgui.h"         
#include "chgui_char.h"    
#include "chgui_bmp.h"     
#include "chgui_touch.h"

//MiniShell
#include "minishell.h"

//uIP
#include "uip.h"
#include "uip_arp.h"
#include "timer.h"
#include "tapdev.h"
#include "uip_demo.h"

//超核 飞思卡尔 Kinetis固件库例程
//固件库版本 V2.4
//startup:        启动文件
//devices:        外设芯片支持库(部分实验用到)
//utilities:      软件模块支持库(部分实验用到)
//drivers:        固件库源码

/*
* 入门知识:
* 1- ENET 以太网模块 Internet的物理基础 传输的单位为以太帧
* 本实验 移植TCP/IP协议栈 uIP 实现 TCP Server TCP Client 和 web功能
*  web: PC IP设置为 192.168.0.2  打开浏览器输入 192.168.0.201
*  PC Server:  PC IP设置为 192.168.0.2  打开网络调试助手 IP设置为 192.168.0.2   选择 TCP Server 端口1400 
*  PC Client:  PC IP设置为 192.168.0.2  打开网络调试助手 IP设置为 192.168.0.201 选择 TCP Client 端口1200
*/

const uint8_t LocalMacAdress[6]={0x00,0xCF,0x52,0x35,0x00,0x01};	//MAC地址
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])	

static void uip_polling(void);

//初始化uIP
void uIPInit(void)
{
	uip_ipaddr_t ipaddr;
		tapdev_init();
		uip_init();				//uIP初始化	  
		uip_arp_init();
		uip_ipaddr(ipaddr, 192,168,0,201);	//设置本地设置IP地址
		uip_sethostaddr(ipaddr);					    
		uip_ipaddr(ipaddr, 192,168,0,1); 	//设置网关IP地址(其实就是你路由器的IP地址)
		uip_setdraddr(ipaddr);						 
		uip_ipaddr(ipaddr, 255,255,255,0);	//设置网络掩码
		uip_setnetmask(ipaddr);

		uip_listen(HTONS(1200));			//监听1200端口,用于TCP Server
		uip_listen(HTONS(80));				//监听80端口,用于Web Server
		tcp_client_reconnect(); //端口为1400 尝试连接1400端口  用于 TCP client 

}

//字符串缓冲区
uint8_t strBuffer[30];

int main(void)
{
	  uint8_t res;
    uint32_t len;
    uint32_t i;
    ENET_InitTypeDef ENET_InitStruct1;
	  PIT_InitTypeDef PIT_InitStruct1;
	  ADC_InitTypeDef ADC_InitStruct1;
    SystemClockSetup(ClockSource_EX50M,CoreClock_100M);
    DelayInit();
    LED_Init(LED_PinLookup_CHK60EVB, kNumOfLED);
	  KBI_Init(KBI_PinLookup_CHK60EVB, kNumOfKEY);
    UART_DebugPortInit(UART4_RX_C14_TX_C15, 115200);
	  DisplayCPUInfo();
    //初始化以太网
	  ENET_InitStruct1.pMacAddress = (uint8_t*)LocalMacAdress;
	  ENET_Init(&ENET_InitStruct1);
	  if(ENET_MiiLinkState() == TRUE)
		{
        UART_printf("ENET Phy Connected succ.\r\n");
		}
		else
		{
			  UART_printf("ENET Phy Connected failed.\r\n");
		}
		//初始化ADC
    ADC_InitStruct1.ADCxMap = ADC0_TEMP_SENSOR_DIFF;     
    ADC_InitStruct1.ADC_Precision = ADC_PRECISION_12BIT; 
    ADC_InitStruct1.ADC_TriggerSelect = ADC_TRIGGER_SW;
    ADC_Init(&ADC_InitStruct1);
		//初始化PIT 用作uIP定时
		PIT_InitStruct1.PITx = PIT2;
		PIT_InitStruct1.PIT_Interval = 10;
		PIT_Init(&PIT_InitStruct1);
		PIT_ITConfig(PIT2, PIT_IT_TIF, ENABLE);
		NVIC_EnableIRQ(PIT2_IRQn);
		//初始化uIP
    uIPInit();
    //开启一个定时器 为uIP提供时钟
    while(1) 
    {
        if(KBI_GetKeyState(kKEY1) == kKBI_SINGLE)
        {
            res = app_tcp_server_senddata("TCP Server Send Message", 24); //发送TCP数据
            break;
        }
        if(KBI_GetKeyState(kKEY1) == kKBI_LONG)
        {
            res = app_tcp_client_senddata("TCP client Send Message", 23);	//发送TCP数据
            break;
        }
        len = app_tcp_server_recdata(strBuffer); //有数据从PC机(Client) 传过来
        if(len > 0) //有数据从PC机传过来
        {
            UART_printf("\r\nData from client(PC):\r\n");
            for(i=0;i<len;i++)
            {
                UART_printf("%c",strBuffer[i]);
            }
        }
				len = app_tcp_client_recdata(strBuffer); //有数据从PC机(server) 传过来
				if(len > 0) //有数据从PC机传过来
				{
						UART_printf("\r\nData from server(PC):\r\n");	
						for(i=0;i<len;i++)
						{
								UART_printf("%c",strBuffer[i]);
						}
				}
				uip_polling();	//处理uip事件，必须插入到用户程序的循环体中  
    }
}

//获得时间
void get_time(uint8_t* msg)
{
	sprintf((char*)msg,"%s %s", __DATE__,  __TIME__);
}
//获得温度
void get_temperature(uint8_t *msg)
{
    float temp;
    float vtemp;
    uint32_t ADC_Value = 0;
    ADC_Value = ADC_GetConversionValue(ADC0_TEMP_SENSOR_DIFF) & 0x0FFF; 
    vtemp=((float)ADC_Value/4096)*3.3;
    if(vtemp>=.7012)
    {
        temp=(vtemp-.7012)/.001646;
    }
    else
    {
        temp=(vtemp-.7012)/.001769;
    }
    temp=25-temp;
    sprintf((char *)msg,"%0.3f C\r\n",temp);	
}
//uip事件处理函数
//必须将该函数插入用户主循环,循环调用.
void uip_polling(void)
{
	uint8_t i;
	static struct timer periodic_timer, arp_timer;
	static uint8_t timer_ok=0;	 
	if(timer_ok==0)//仅初始化一次
	{
		timer_ok = 1;
		timer_set(&periodic_timer,CLOCK_SECOND/2);  //创建1个0.5秒的定时器 
		timer_set(&arp_timer,CLOCK_SECOND*10);	   	//创建1个10秒的定时器 
	}				 
	uip_len=tapdev_read();	//从网络设备读取一个IP包,得到数据长度.uip_len在uip.c中定义
	if(uip_len>0) 			//有数据
	{   
		 //printf("接收到一帧len:%d\r\n",uip_len);
		//处理IP数据包(只有校验通过的IP包才会被接收) 
		if(BUF->type == htons(UIP_ETHTYPE_IP))//是否是IP包? 
		{
			uip_arp_ipin();	//去除以太网头结构，更新ARP表
			uip_input();   	//IP包处理
			//当上面的函数执行后，如果需要发送数据，则全局变量 uip_len > 0
			//需要发送的数据在uip_buf, 长度是uip_len  (这是2个全局变量)		
			if(uip_len>0)//需要回应数据
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		}else if (BUF->type==htons(UIP_ETHTYPE_ARP))//处理arp报文,是否是ARP请求包?
		{
			uip_arp_arpin();
 			//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len(这是2个全局变量)
 			if(uip_len>0)tapdev_send();//需要发送数据,则通过tapdev_send发送	 
		}
	}else if(timer_expired(&periodic_timer))	//0.5秒定时器超时
	{
		timer_reset(&periodic_timer);		//复位0.5秒定时器 
		//轮流处理每个TCP连接, UIP_CONNS缺省是40个  
		for(i=0;i<UIP_CONNS;i++)
		{
			uip_periodic(i);	//处理TCP通信事件  
	 		//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len (这是2个全局变量)
	 		if(uip_len>0)
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		}
#if UIP_UDP	//UIP_UDP 
		//轮流处理每个UDP连接, UIP_UDP_CONNS缺省是10个
		for(i=0;i<UIP_UDP_CONNS;i++)
		{
			uip_udp_periodic(i);	//处理UDP通信事件
	 		//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len (这是2个全局变量)
			if(uip_len > 0)
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		}
#endif 
		//每隔10秒调用1次ARP定时器函数 用于定期ARP处理,ARP表10秒更新一次，旧的条目会被抛弃
		if(timer_expired(&arp_timer))
		{
			timer_reset(&arp_timer);
			uip_arp_timer();
		}
	}
}


