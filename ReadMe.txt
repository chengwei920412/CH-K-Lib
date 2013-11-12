超核Kinetis K60 固件库V2.41

1.本店地址:http://upcmcu.taobao.com 
2.MDK工程请使用 MDK4.53以上版本打开 IAR 请用6.6以上版本打开
3.本文件夹请放在英文目录下，比如 不 要放到  /D/学习/...
4.相关软件及最新版本下载地址：http://pan.baidu.com/share/home?uk=2870907047


中文目录结构

CH-KinetisV2.40
│
├─Doc: 参考文档
│
├─Libraries(源代码库 轻易不要改动 固件库默认是只读的 如果非要改动 在文件夹属性中取消只读即可)
├─Projects(例程 及 工程模板 所在文件夹 每个例程支持MDK和IAR)
│  │  
│  │ 
│  │─Template(normal)(工程模板 使用JLink等标准下载器下载 
│  │ 
│  │─Template(bootloader)(如果使用Bootloader才请使用此模板编程)
│  │      
│  │─CHK60EVB Examples (超核K60开发板 例程 所有例程都是 普通版本下的,不是Bootloader版本)
│  │  ├─1-Warmup(GPIO)  V2.4固件库热身实验
│  │  ├─2-OutputControl(GPIO)               GPIO输出控制实验
│  │  ├─3-InputControl(GPIO)                GPIO输入控制实验
│  │  ├─4-LEDControl(GPIO)                  LED控制闪烁实验
│  │  ├─5-KeyInput(GPIO)                    按键输入实验
│  │  ├─6-WatchDogTest(WDOG)                看门狗实验
│  │  ├─7-SerialDebugOutput(UART)           串口输出入门实验
│  │  ├─8-SerialPollingSendData(UART)       串口轮询发送实验
│  │  ├─9-SerialIntrruptSendData(UART)      串口中断发送实验
│  │  ├─10-SerialPollingReceiveData(UART)   串口轮询接收实验
│  │  ├─11-SerialInterruptReceiveData(UART) 串口中断接收实验
│  │  ├─12-ExternalPinIntrrupt(GPIO)        外部引脚中断实验
│  │  ├─13-ProidicTimerTest(PIT)            周期性定时器实验
│  │  ├─14-FlexTimer-PWMOutput(FTM)         PWM输出实验
│  │  ├─15-RealTimeClock(RTC)               实时时钟实验
│  │  ├─16-LCDTest(FlexBus)                 TFTLCD实验
│  │  ├─17-AnalogToDigtialConvert(ADC)      模数转换实验
│  │  ├─18-InternalTempSensor(ADC)          芯片片内温度传感器实验
│  │  ├─19-ExternalSPIFlash(SPI)            外部SPI存储器实验
│  │  ├─20-TouchScreen(SPI)                 触摸屏实验
│  │  ├─21-BMPDecodeAndDisplay(CHGUI)       BMP图片解码显示实验
│  │  ├─22-SinglePulseCount(LPTMR)          低功耗定时器，脉冲计数实验
│  │  ├─23-CANBus(CAN)                      CAN总线实验
│  │  ├─24-InternalFlash(FLASH)             内部FLASH实验
│  │  ├─25-NRF2401(SPI)                     NRF2401 无线通讯实验
│  │  ├─26-DMAMemCopy(DMA)                  DMA内存拷贝入门实验
│  │  ├─27-DMAUARTSendData(DMA)             串口DMA发送数据实验
│  │  ├─28-SDCard(SDIO)                     SD卡实验
│  │  ├─29-FAT32FileSystem(SDIO)            FAT32文件系统实验
│  │  ├─30-FAT32DataRecord(SDIO)            FAT32文件系统做数据记录实验
│  │  ├─31-FAT32BMPDisplay(SDIO)            SD卡内BMP图片读取实验
│  │  ├─32-MiniShell(MINISHELL)             SHELL系统展示实验
│  │  ├─33-EnetMAC(ENET)                    以太网MAC层实验
│  │  ├─34-uIP(ENET)                        UIP移植实验
│  │  ├─35-USBMouse(USB)                    USB鼠标
│  │  ├─36-USBKeyBorad(USB)                 USB键盘
│  │  ├─37-USBHIDDevices(USB)               USB 自定义人体学输入设备
│  │  ├─38-TaskScheduling(uCOS)             uCOS 任务调度实验
│  │  ├─39-SoftwareTimer(uCOS)              uCOS 软件定时器实验
│  │  ├─40-MassageBox(uCOS)                 uCOS 消息邮箱实验
│  │  ├─41-uCGUI(uCGUI)                     uCGUI 移植实验
│  │  ├─42-USB_Bootloader(CDC)           USBBooltoader实验（配合Templete(Bootloader)模板
│  │ 
│  │ 
│  │─CHKATOM Examples (超核K60原子核 核心板 例程 
│  │  ├─1-Warmup(GPIO)                     V2.4固件库热身实验
│  │  ├─2-OutputControl(GPIO)               GPIO输出控制实验
│  │  ├─3-InputControl(GPIO)                GPIO输入控制实验
│  │  ├─4-LEDControl(GPIO)                  LED控制闪烁实验
│  │  ├─5-KeyInput(GPIO)                    按键输入实验
│  │  ├─6-WatchDogTest(WDOG)                看门狗实验
│  │  ├─7-SerialDebugOutput(UART)           串口输出入门实验
│  │  ├─8-SerialPollingSendData(UART)       串口轮询发送实验
│  │  ├─9-SerialIntrruptSendData(UART)      串口中断发送实验
│  │  ├─10-SerialPollingReceiveData(UART)   串口轮询接收实验
│  │  ├─11-SerialInterruptReceiveData(UART) 串口中断接收实验
│  │  ├─12-ExternalPinIntrrupt(GPIO)        外部引脚中断实验
│  │  ├─13-ProidicTimerTest(PIT)            周期性定时器实验
│  │  ├─14-FlexTimer-PWMOutput(FTM)         PWM输出实验
│  │  ├─15-RealTimeClock(RTC)               实时时钟实验
│  │  ├─16-LCDTest(FlexBus)                 TFTLCD实验
│  │  ├─17-AnalogToDigtialConvert(ADC)      模数转换实验
│  │  ├─18-InternalTempSensor(ADC)          芯片片内温度传感器实验
│  │  ├─19-MMA845xAccelerometer(I2C)        MMA845x加速度传感器
│  │  ├─20-OV7620(GPIO DMA I2C)             OV7620摄像头实验
│  │  ├─21-BMPDecodeAndDisplay(CHGUI)       BMP图片解码显示实验
│  │  ├─22-SinglePulseCount(LPTMR)          低功耗定时器，脉冲计数实验
│  │  ├─23-CANBus(CAN)                      CAN总线实验
│  │  ├─24-InternalFlash(FLASH)             内部FLASH实验
│  │  ├─25-NRF2401(SPI)                     NRF2401 无线通讯实验
│  │  ├─26-DMAMemCopy(DMA)                  DMA内存拷贝入门实验
│  │  ├─27-DMAUARTSendData(DMA)             串口DMA发送数据实验
│  │  ├─28-SDCard(SDIO)                     SD卡实验
│  │  ├─29-FAT32FileSystem(SDIO)            FAT32文件系统实验
│  │  ├─30-FAT32DataRecord(SDIO)            FAT32文件系统做数据记录实验
│  │  ├─31-FAT32BMPDisplay(SDIO)            SD卡内BMP图片读取实验
│  │  ├─32-MiniShell(MINISHELL)             SHELL系统展示实验
│  │  ├─33-EnetMAC(ENET)                    以太网MAC层实验
│  │  ├─34-uIP(ENET)                        UIP移植实验
│  │  ├─35-USBMouse(USB)                    USB鼠标
│  │  ├─36-USBKeyBorad(USB)                 USB键盘
│  │  ├─37-USBHIDDevices(USB)               USB 自定义人体学输入设备
│  │  ├─38-TaskScheduling(uCOS)             uCOS 任务调度实验
│  │  ├─39-SoftwareTimer(uCOS)              uCOS 软件定时器实验
│  │  ├─40-MassageBox(uCOS)                 uCOS 消息邮箱实验
│  │  ├─41-uCGUI(uCGUI)                     uCGUI 移植实验
│  │  ├─42-USB_Bootloader(CDC)           USBBooltoader实验（配合Templete(Bootloader)模板
