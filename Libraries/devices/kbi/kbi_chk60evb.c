#include "kbi_chk60evb.h"


//CHK60开发板上KEY资源
const KBI_PinLookup_TypeDef KBI_PinLookup_CHK60EVB[kNumOfKEY] = 
{
    {(uint32_t)PTC_BASE, (uint32_t)GPIO_Pin_17},
    {(uint32_t)PTC_BASE, (uint32_t)GPIO_Pin_18},
};	

