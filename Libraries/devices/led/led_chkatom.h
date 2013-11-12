#ifndef __LED_CHKATOM_H__
#define __LED_CHKATOM_H__
#include "led.h"

//! @brief LEDs in CHK Atom core board
typedef enum
{ 
    kLED1,
    kLED2,
    kLED3,
    kLED4,
    kNumOfLED,
}LED_Pin_TypeDef;

extern  const LED_PinLookup_TypeDef LED_PinLookup_CHKATOM[];

#endif

