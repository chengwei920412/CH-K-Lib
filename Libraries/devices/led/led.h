/**
  ******************************************************************************
  * @file    led.h
  * @author  YANDLD
  * @version V1.0
  * @date    2013.10.7
  * @brief   LED Components drivers
  ******************************************************************************
  */
#ifndef __LED_H__
#define __LED_H__

#include "gpio.h"

#ifdef __cplusplus
 extern "C" {
#endif

//! @brief LED Register Table
typedef struct
{
    uint32_t LED_PortMoudle; //!< LED PORT address eg: PTA_BASE
    uint32_t LED_PinIndex;   //!< LED port index   eg: GPIO_Pin_1
}LED_PinLookup_TypeDef;

//! @brief API functions
void LED_Init(const LED_PinLookup_TypeDef* LED_PinLookupStruct, uint32_t NumOfPins);
void LED_Ctrl(uint32_t LEDIndex, BitAction LEDState);
uint32_t LED_GetNumOfLED(void);
void LED_Set(uint32_t LEDIndex);
void LED_Reset(uint32_t LEDIndex);
void LED_Toggle(uint32_t LEDIndex);
uint8_t LED_GetState(uint32_t LEDIndex);

#ifdef __cplusplus
}
#endif

#endif
