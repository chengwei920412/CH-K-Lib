#include "led_chkatom.h"

//! @brief LEDs in CHK Atom core board
const LED_PinLookup_TypeDef LED_PinLookup_CHKATOM[kNumOfLED] = 
{
    {(uint32_t)PTC_BASE, (uint32_t)GPIO_Pin_16},
    {(uint32_t)PTC_BASE, (uint32_t)GPIO_Pin_17},
    {(uint32_t)PTC_BASE, (uint32_t)GPIO_Pin_18},
    {(uint32_t)PTD_BASE, (uint32_t)GPIO_Pin_1},
};	


