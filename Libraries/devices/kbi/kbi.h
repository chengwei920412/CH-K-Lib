/**
  ******************************************************************************
  * @file    key.h
  * @author  YANDLD
  * @version V2.4
  * @date    2013.5.23
  * @brief   超核原子核核心板 BSP构件 KEY驱动
  ******************************************************************************
  */
#ifndef _KBI_H_
#define _KBI_H_

#include "gpio.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define	KBI_STATE_DOWN                Bit_SET
#define KBI_STATE_UP                  !KBI_STATE_DOWN
#define KBI_SCAN_PERIOD_IN_US         (1000*10)
#define KBI_MAX_KEY_CNT               (8)
#define KBI_LONG_KEY_TIMEIN_US        (1000*500)
	 

typedef struct
{
    uint32_t KBI_PortMoudle;
    uint32_t KBI_PinIndex;
}KBI_PinLookup_TypeDef;
	 

typedef enum
{
    kKBI_NO_KEY,
    kKBI_SINGLE,
    kKBI_LONG,
}KBI_KeyState_TypeDef;

//本构件所实现的接口函数	 
void KBI_Init(const KBI_PinLookup_TypeDef* KBI_PinLookupStruct, uint32_t NumOfKEYs);
KBI_KeyState_TypeDef KBI_GetKeyState(uint32_t KeyIndex);
uint8_t KBI_GetKeyValue(uint32_t KeyIndex);
uint32_t KBI_GetNumOfKEY(void);
uint32_t KBI_GetScanPeriodInUs(void);
void  KBI_Scan(void);


#ifdef __cplusplus
}
#endif

#endif
