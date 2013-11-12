#ifndef FLASH_OPERATE_H_INCLUDED
#define FLASH_OPERATE_H_INCLUDED

#include <stdint.h>

typedef enum
{
    OP_OVERFLOW = -1,
    OP_SUCCESS,
    OP_BUSY,
    OP_ERROR,
    OP_TIMEOUT
} OPERATE_Status;

extern void Flash_Init(void);
extern OPERATE_Status Fn_WriteFlash(const uint32_t start_addr, uint32_t *data_ptr, const uint32_t length);
extern OPERATE_Status Fn_EreaseFlash(const uint32_t start_addr, const uint8_t erase_pages);




#endif // FLASH_OPERATE_H_INCLUDED
