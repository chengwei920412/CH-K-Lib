#ifndef __KBI_CHK60EVB_H__
#define __KBI_CHK60EVB_H__

#include "kbi.h"

//!< CHK60EB板 超核K60开发板上 KEY资源
typedef enum
{ 
    kKEY1,
    kKEY2,
    kNumOfKEY,
}KBI_Pin_TypeDef;

extern  const KBI_PinLookup_TypeDef KBI_PinLookup_CHK60EVB[];

#endif

