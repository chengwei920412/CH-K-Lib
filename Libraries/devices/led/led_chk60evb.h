#ifndef __LED_CHK60EVB_H__
#define __LED_CHK60EVB_H__
#include "led.h"

//! @brief enum of LEDs in CHK60EVB board
typedef enum
{ 
    kLED1,
    kLED2,
    kNumOfLED,
}LED_Pin_TypeDef;

extern  const LED_PinLookup_TypeDef LED_PinLookup_CHK60EVB[];

#endif

