#include "bootloader.h"
#include "bootloader_util.h"
#include "sys.h"
#include "uart.h"
#include "led.h"
#include "USB.h"
#include "USB_CDC.h" 
#include "def.h"
#include "key.h"

int main(void)
{   
	u32 i=0;



	KEY_Init();
	//UART_Init(UART4,115200);
	//按下了
	if((KEY1 == 0) || (KEY2 == 0))
	{	
		GetCPUInfo();
	  DelayInit();   //初始化延时函数
		LED_Init();
		USB_Init(); //一定需要96M内部FLL倍频
		while(USB_IsDeviceEnumed() == 0);
		//点亮一颗LED表示 USB枚举成功
		LED0 = 1;
		BootloaderInit(UART3,115200,2000);
		while(1)
		{ 
			BootloaderProc(); //执行Boot主进程
		}
	}
	else
	{
		Fn_GoToUserApplication(APP_START_ADDR);
	}
}





