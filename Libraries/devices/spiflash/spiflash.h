#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__
#include "sys.h"
#include "spi.h"

//与触摸屏芯片连接引脚	   
#define SPI_FLASH_CS_GPIO_PORT   PTB
#define SPI_FLASH_CS_GPIO_PIN    GPIO_Pin_10
//定义硬件SPI端口
#define SPI_FLASH_PORT      SPI1_SCK_PB11_SOUT_PB16_SIN_PB17
#define SPI_FLASH_CS_PORT   SPI1_PCS0_PB10

#define SPI_CS_HIGH        GPIO_WriteBit(SPI_FLASH_CS_GPIO_PORT, SPI_FLASH_CS_GPIO_PIN, Bit_SET)
#define SPI_CS_LOW         GPIO_WriteBit(SPI_FLASH_CS_GPIO_PORT, SPI_FLASH_CS_GPIO_PIN, Bit_RESET)

//W25X系列/Q系列芯片列表	   
//W25Q80 ID  0XEF13  //8Mbit  /1M   Byte
//W25Q16 ID  0XEF14  //16Mbit /2M   Byte
//W25Q32 ID  0XEF15  //32Mbit /4M   Byte
//W25Q64 ID  0XEF16	 //64Mbit /8M   Byte
//W25W10 ID  0XEF10  //1M bit /128K Byte
//W25W20 ID  0XEF11  //2M bit /256K Byte
//W25W40 ID  0XEF12  //4M bit /512K Byte
//W25W80 ID  0XEF13  //8M bit /1M   Byte
#define W25Q80 	(0XEF13)	
#define W25Q16 	(0XEF14)
#define W25Q32 	(0XEF15)
#define W25Q64 	(0XEF16)
#define W25X10  (0XEF10)
#define W25X20  (0XEF11)
#define W25X40  (0XEF12)
#define W25X80  (0XEF13)
//指令表
#define W25X_WriteEnable		 	    (0x06) 
#define W25X_WriteDisable		      (0x04)  
#define W25X_ReadStatusReg	    	(0x05)  
#define W25X_WriteStatusReg	    	(0x01)  
#define W25X_ReadData			        (0x03)  
#define W25X_FastReadData	  	    (0x0B)  
#define W25X_FastReadDual		      (0x3B)  
#define W25X_PageProgram		      (0x02)  
#define W25X_BlockErase		      	(0xD8)  
#define W25X_SectorErase	  	    (0x20)  
#define W25X_ChipErase		      	(0xC7)  
#define W25X_PowerDown			      (0xB9)  
#define W25X_ReleasePowerDown	    (0xAB)  
#define W25X_DeviceID		        	(0xAB)  
#define W25X_ManufactDeviceID	    (0x90)  
#define W25X_JedecDeviceID		    (0x9F)  
//本构件实现的接口函数
uint8_t SPI_FLASH_Init(void);
uint8_t SPI_FLASH_ReadWriteByte(uint8_t dat);
uint16_t SPI_FLASH_ReadID(void);
uint8_t SPI_FLASH_ReadSR(void);
void SPI_FLASH_WriteSR(uint8_t sr);
void SPI_FLASH_WaitBusy(void);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WriteDisable(void);
void SPI_FLASH_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);
void SPI_FLASH_WritePage(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void SPI_FLASH_EraseSector(uint32_t Dst_Addr);
void SPI_FLASH_WriteNoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void SPI_FLASH_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void SPI_FLASH_PowerDown(void);
void SPI_FLASH_PowerUp(void);
uint32_t SPI_FLASH_GetCapacity(void);
void SPI_FLASH_EraseChip(void);
#endif


