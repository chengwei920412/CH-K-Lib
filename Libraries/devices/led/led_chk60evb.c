#include "led_chk60evb.h"

//! @brief LEDs in CHK60EVB board
const LED_PinLookup_TypeDef LED_PinLookup_CHK60EVB[kNumOfLED] = 
{
    {(uint32_t)PTD_BASE, (uint32_t)GPIO_Pin_7},
    {(uint32_t)PTD_BASE, (uint32_t)GPIO_Pin_0},
};	


