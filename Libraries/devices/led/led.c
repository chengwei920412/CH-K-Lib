/**
  ******************************************************************************
  * @file    led.c
  * @author  YANDLD
  * @version V1.0
  * @date    2013.10.7
  * @brief   LED Components drivers
  ******************************************************************************
  */
#include "led.h"


//! @brief global pointer of LED install struct
static LED_PinLookup_TypeDef* gLED_PinLookupStruct = NULL;

//! @brief global variable indicate the number of LEDs
static uint32_t gNumOfLED = 0;
  /**
  * @brief  init LED controller
  * @param  LED_PinLookupStruct: pointer to a LED_PinLookup_TypeDef struct
  * @param  number of LED in the struct
  * @retval None
  */
void LED_Init(const LED_PinLookup_TypeDef* LED_PinLookupStruct, uint32_t NumOfPins)
{	
    uint8_t i;
    GPIO_InitTypeDef GPIO_InitStruct1;
    uint32_t temp = (uint32_t)LED_PinLookupStruct;
    if((LED_PinLookupStruct != NULL) && (NumOfPins != 0))
		{
        gLED_PinLookupStruct = (LED_PinLookup_TypeDef*)temp;
        gNumOfLED = NumOfPins;
        for(i = 0; i < NumOfPins; i++)
        {
            GPIO_InitStruct1.GPIOx = (GPIO_Type*)LED_PinLookupStruct[i].LED_PortMoudle;
            GPIO_InitStruct1.GPIO_InitState = Bit_RESET;
            GPIO_InitStruct1.GPIO_IRQMode = GPIO_IT_DISABLE;
            GPIO_InitStruct1.GPIO_Mode = GPIO_Mode_OPP;
            GPIO_InitStruct1.GPIO_Pin = LED_PinLookupStruct[i].LED_PinIndex;
            GPIO_Init(&GPIO_InitStruct1);
        }
		}
}
  /**
  * @brief  turn on or turn off the LED
  * @param  index of LED, this value must equal or less then NumOfLEDs
  * @param  LEDState:
  *         @arg Bit_SET: high state
  *         @arg Bit_RESET: low state
  * @retval None
  */
void LED_Ctrl(uint32_t LEDIndex, BitAction LEDState)
{
    GPIO_WriteBit((GPIO_Type*)gLED_PinLookupStruct[LEDIndex].LED_PortMoudle,
	                 gLED_PinLookupStruct[LEDIndex].LED_PinIndex,
	                 LEDState);
}
  /**
  * @brief  get number of LED
  * @param  none
  * @retval number of LED
  */
uint32_t LED_GetNumOfLED(void)
{
    return gNumOfLED;
}
  /**
  * @brief  change a LED to high state
  * @param  index of LED, this value must equal or less then NumOfLEDs
  * @retval None
  */
void LED_Set(uint32_t LEDIndex)
{
    LED_Ctrl(LEDIndex, Bit_SET);
}
  /**
  * @brief  change a LED to low state
  * @param  index of LED, this value must equal or less then NumOfLEDs
  * @retval None
  */
void LED_Reset(uint32_t LEDIndex)
{
    LED_Ctrl(LEDIndex, Bit_RESET);
}
  /**
  * @brief  toggle a LED state
  * @param  index of LED, this value must equal or less then NumOfLEDs
  * @retval None
  */
void LED_Toggle(uint32_t LEDIndex)
{
    GPIO_ToggleBit((GPIO_Type*)gLED_PinLookupStruct[LEDIndex].LED_PortMoudle,
	                 gLED_PinLookupStruct[LEDIndex].LED_PinIndex);
}
  /**
  * @brief  get the state of an LED
  * @param  index of LED, this value must equal or less then NumOfLEDs
  * @retval None
  */
uint8_t LED_GetState(uint32_t LEDIndex)
{
    uint8_t BitState;
    BitState = GPIO_ReadOutputDataBit((GPIO_Type*)gLED_PinLookupStruct[LEDIndex].LED_PortMoudle, 
	                                    gLED_PinLookupStruct[LEDIndex].LED_PinIndex);
    return BitState;
}
