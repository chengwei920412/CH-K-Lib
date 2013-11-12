#include "usb_hid.h"
#include "usb.h"
u8 USB_HID_SendBuf[32]; //HID 发送缓冲
u8 USB_HID_SendReportLen;
//发送报告
void USB_HID_SendReport(void)
{
	u8 i;
	USB_EP_IN_Transfer(EP2,USB_HID_SendBuf,USB_HID_SendReportLen);
	for(i=0;i<USB_HID_SendReportLen;i++) //清空报告缓冲区
	{
		USB_HID_SendBuf[i] = 0;
	}
}
//USB接收报告
u8 USB_HID_RecReport(u8* buf)
{
	u8 *pu8EPBuffer;
	u8 temp;
	u8 len = 0;
	len = USB_EP_OUT_SizeCheck(EP3);
	temp = len;
	if(len)
	{
		pu8EPBuffer = BufferPointer[bEP3OUT_ODD];
		while(len--) *buf++=*pu8EPBuffer++;
	}
	len = temp;
	return len;
}
//鼠标数据发送函数
void USB_HID_SetMouse(u8 HOffset,u8 VOffset,u8 SOffset,u8 Key)
{
		USB_HID_SendReportLen = 4;
		USB_HID_SendBuf[1] = HOffset;
		USB_HID_SendBuf[2] = VOffset;
		USB_HID_SendBuf[3] = SOffset;
		USB_HID_SendBuf[0] = Key;
}
//通过上面的报告描述符的定义，我们知道返回的输入报告具有8字节。
//第一字节的8个bit用来表示特殊键是否按下（例如Shift、Alt等键）。
//第二字节为保留值，值为常量0。第三到第八字节是一个普通键键值的
//数组，当没有键按下时，全部6个字节值都为0。当只有一个普通键按下时，
//这六个字节中的第一字节值即为该按键的键值（具体的键值请看HID的
//用途表文档），当有多个普通键同时按下时，则同时返回这些键的键值。
//如果按下的键太多，则这六个字节都为0xFF（不能返回0x00，这样会让
//操作系统认为所有键都已经释放）。至于键值在数组中的先后顺序是
//无所谓的，操作系统会负责检查是否有新键按下。我们应该在中断端点1
//中按照上面的格式返回实际的键盘数据。另外，报告中还定义了一个字节
//的输出报告，是用来控制LED情况的。只使用了低7位，高1位是保留值0。
//当某位的值为1时，则表示对应的LED要点亮。操作系统会负责同步各个
//键盘之间的LED，例如你有两块键盘，一块的数字键盘灯亮时，另一块
//也会跟着亮。键盘本身不需要判断各种LED应该何时亮，它只是等待主机
//发送报告给它，然后根据报告值来点亮相应的LED。我们在端点1输出中断
//中读出这1字节的输出报告，然后对它取反（因为学习板上的LED是低电平时
//亮），直接发送到LED上。这样main函数中按键点亮LED的代码就不需要了。
//键盘数据发送函数

//USBHID 常用键值 不够用百度搜
/*
从HID文档里摘出来的，供大家参考一下
0 00 Reserved (no event indicated)9 N/A √ √ √ 4/101/104
1 01 Keyboard ErrorRollOver9 N/A √ √ √ 4/101/104
2 02 Keyboard POSTFail9 N/A √ √ √ 4/101/104
3 03 Keyboard ErrorUndefined9 N/A √ √ √ 4/101/104
4 04 Keyboard a and A4 31 √ √ √ 4/101/104
5 05 Keyboard b and B 50 √ √ √ 4/101/104
6 06 Keyboard c and C4 48 √ √ √ 4/101/104
7 07 Keyboard d and D 33 √ √ √ 4/101/104
8 08 Keyboard e and E 19 √ √ √ 4/101/104
9 09 Keyboard f and F 34 √ √ √ 4/101/104
10 0A Keyboard g and G 35 √ √ √ 4/101/104
11 0B Keyboard h and H 36 √ √ √ 4/101/104
12 0C Keyboard i and I 24 √ √ √ 4/101/104
13 0D Keyboard j and J 37 √ √ √ 4/101/104
14 0E Keyboard k and K 38 √ √ √ 4/101/104
15 0F Keyboard l and L 39 √ √ √ 4/101/104
16 10 Keyboard m and M4 52 √ √ √ 4/101/104
17 11 Keyboard n and N 51 √ √ √ 4/101/104
18 12 Keyboard o and O4 25 √ √ √ 4/101/104
19 13 Keyboard p and P4 26 √ √ √ 4/101/104
20 14 Keyboard q and Q4 17 √ √ √ 4/101/104
21 15 Keyboard r and R 20 √ √ √ 4/101/104
22 16 Keyboard s and S4 32 √ √ √ 4/101/104
23 17 Keyboard t and T 21 √ √ √ 4/101/104
24 18 Keyboard u and U 23 √ √ √ 4/101/104
25 19 Keyboard v and V 49 √ √ √ 4/101/104
26 1A Keyboard w and W4 18 √ √ √ 4/101/104
27 1B Keyboard x and X4 47 √ √ √ 4/101/104
28 1C Keyboard y and Y4 22 √ √ √ 4/101/104
29 1D Keyboard z and Z4 46 √ √ √ 4/101/104
30 1E Keyboard 1 and !4 2 √ √ √ 4/101/104
31 1F Keyboard 2 and @4 3 √ √ √ 4/101/104
32 20 Keyboard 3 and #4 4 √ √ √ 4/101/104
33 21 Keyboard 4 and $4 5 √ √ √ 4/101/104
34 22 Keyboard 5 and %4 6 √ √ √ 4/101/104
35 23 Keyboard 6 and ^4 7 √ √ √ 4/101/104
36 24 Keyboard 7 and &4 8 √ √ √ 4/101/104
37 25 Keyboard 8 and *4 9 √ √ √ 4/101/104
38 26 Keyboard 9 and (4 10 √ √ √ 4/101/104
39 27 Keyboard 0 and )4 11 √ √ √ 4/101/104

*/
#define NULL   0
//设置KeyBoard参数
//Fn Key是特殊功能键， 最低位1Ctrl 后面分别Alt Tab
//后面是键盘缓冲区， 每次只持6个按键同时按下
u8 USB_HID_SetKeyBoard(u8 FnKey,u8 *Keybuf)
{
	u8 i;
	if(Keybuf == NULL) return 1;
	USB_HID_SendReportLen = 8;
	USB_HID_SendBuf[0] = FnKey;
	for(i=2;i<8;i++)
	{
		USB_HID_SendBuf[i] = Keybuf[i-2]; 
	}
}


