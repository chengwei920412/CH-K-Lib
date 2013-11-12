#include "flash_operate.h"
#include "sys.h"
#include "def.h"
//FLASH 操作定义如下：
//=============================================================================================
/* error code */
#define Flash_OK                    0x00
#define Flash_FACCERR               0x01
#define Flash_FPVIOL                0x02
#define Flash_MGSTAT0               0x04
#define Flash_RDCOLERR              0x08
#define Flash_NOT_ERASED            0x10
#define Flash_CONTENTERR            0x11

/* flash commands */
#define FlashCmd_ProgramLongWord    0x06
#define FlashCmd_SectorErase        0x09


static uint8_t Flash_SectorErase(uint32_t FlashPtr);
static uint8_t Flash_ByteProgram(uint32_t FlashStartAdd,uint32_t *DataSrcPtr,uint32_t NumberOfBytes);


/*******************************************************************************
 * Function:        Flash_Init
 *
 * Description:     Set the flash clock
 *
 * Returns:         never return
 *
 * Notes:
 *
 *******************************************************************************/
void Flash_Init(void)
{
    /* checking access error */
    if (FTFL->FSTAT & FTFL_FSTAT_ACCERR_MASK)
    {
        /* clear error flag */
        FTFL->FSTAT |= FTFL_FSTAT_ACCERR_MASK;
    }
    /* checking protection error */
    else if (FTFL->FSTAT & FTFL_FSTAT_FPVIOL_MASK)
    {
        /* clear error flag */
        FTFL->FSTAT |= FTFL_FSTAT_FPVIOL_MASK;
    }
    else if (FTFL->FSTAT & FTFL_FSTAT_RDCOLERR_MASK)
    {
        /* clear error flag */
        FTFL->FSTAT |= FTFL_FSTAT_RDCOLERR_MASK;
    } /* EndIf */
    /* Disable Data Cache in Flash memory Controller Module  */

    FMC->PFB0CR &= ~FMC_PFB0CR_B0DCE_MASK;
    FMC->PFB1CR &= ~FMC_PFB1CR_B1DCE_MASK;

} /* EndBody */


/*******************************************************************************
 * Function:        Flash_SectorErase
 *
 * Description:     erase a sector of the flash
 *
 * Returns:         Error Code
 *
 * Notes:
 *
 *******************************************************************************/
static uint8_t Flash_SectorErase(uint32_t FlashPtr)
{
    /* Body */
    unsigned char Return = Flash_OK;

    /* Allocate space on stack to run flash command out of SRAM */
    /* wait till CCIF is set*/
    while (!(FTFL->FSTAT & FTFL_FSTAT_CCIF_MASK)) {};
    /* Write command to FCCOB registers */
    FTFL->FCCOB0 = FlashCmd_SectorErase;
    FTFL->FCCOB1 = (uint8_t)(FlashPtr >> 16);
    FTFL->FCCOB2 = (uint8_t)((FlashPtr >> 8) & 0xFF);
    FTFL->FCCOB3 = (uint8_t)(FlashPtr & 0xFF);

    /* Launch command */
    FTFL->FSTAT |= FTFL_FSTAT_CCIF_MASK;
    /* wait for command completion */
    while (!(FTFL->FSTAT & FTFL_FSTAT_CCIF_MASK)) {};
    /* checking access error */
    if (FTFL->FSTAT & FTFL_FSTAT_ACCERR_MASK)
    {
        /* clear error flag */
        FTFL->FSTAT |= FTFL_FSTAT_ACCERR_MASK;

        /* update return value*/
        Return |= Flash_FACCERR;
    }
    /* checking protection error */
    else if (FTFL->FSTAT & FTFL_FSTAT_FPVIOL_MASK)
    {
        /* clear error flag */
        FTFL->FSTAT |= FTFL_FSTAT_FPVIOL_MASK;

        /* update return value*/
        Return |= Flash_FPVIOL;
    }
    else if (FTFL->FSTAT & FTFL_FSTAT_RDCOLERR_MASK)
    {
        /* clear error flag */
        FTFL->FSTAT |= FTFL_FSTAT_RDCOLERR_MASK;

        /* update return value*/
        Return |= Flash_RDCOLERR;
    }
    /* checking MGSTAT0 non-correctable error */
    else if (FTFL->FSTAT & FTFL_FSTAT_MGSTAT0_MASK)
    {
        Return |= Flash_MGSTAT0;
    } /* EndIf */
    /* function return */
    return  Return;
} /* EndBody */

/*******************************************************************************
 * Function:        Flash_ByteProgram
 *
 * Description:     byte program the flash
 *
 * Returns:         Error Code
 *
 * Notes:
 *
 *******************************************************************************/
static uint8_t Flash_ByteProgram(uint32_t FlashStartAdd,uint32_t *DataSrcPtr,uint32_t NumberOfBytes)
{
    /* Body */
    unsigned char Return = Flash_OK;
    /* Allocate space on stack to run flash command out of SRAM */
    uint32_t size_buffer;
    if (NumberOfBytes == 0)
    {
        return Flash_CONTENTERR;
    }
    else
    {
        size_buffer = (NumberOfBytes - 1)/4 + 1;
    } /* EndIf */
    /* wait till CCIF is set*/
    while (!(FTFL->FSTAT & FTFL_FSTAT_CCIF_MASK)) {};

    while ((size_buffer) && (Return == Flash_OK))
    {
        /* Write command to FCCOB registers */
        FTFL->FCCOB0 = FlashCmd_ProgramLongWord;
        FTFL->FCCOB1 = (uint8_t)(FlashStartAdd >> 16);
        FTFL->FCCOB2 = (uint8_t)((FlashStartAdd >> 8) & 0xFF);
        FTFL->FCCOB3 = (uint8_t)(FlashStartAdd & 0xFF);

        FTFL->FCCOB4 = (uint8_t)(*((uint8_t*)DataSrcPtr+3));
        FTFL->FCCOB5 = (uint8_t)(*((uint8_t*)DataSrcPtr+2));
        FTFL->FCCOB6 = (uint8_t)(*((uint8_t*)DataSrcPtr+1));
        FTFL->FCCOB7 = (uint8_t)(*((uint8_t*)DataSrcPtr+0));

        /* Launch command */
        FTFL->FSTAT |= FTFL_FSTAT_CCIF_MASK;
        /* wait for command completion */
        while (!(FTFL->FSTAT & FTFL_FSTAT_CCIF_MASK)) {};

        /* checking access error */
        if (FTFL->FSTAT & FTFL_FSTAT_ACCERR_MASK)
        {
            /* clear error flag */
            FTFL->FSTAT |= FTFL_FSTAT_ACCERR_MASK;

            /* update return value*/
            Return |= Flash_FACCERR;
        }
        /* checking protection error */
        else if (FTFL->FSTAT & FTFL_FSTAT_FPVIOL_MASK)
        {
            /* clear error flag */
            FTFL->FSTAT |= FTFL_FSTAT_FPVIOL_MASK;

            /* update return value*/
            Return |= Flash_FPVIOL;
        }
        else if (FTFL->FSTAT & FTFL_FSTAT_RDCOLERR_MASK)
        {
            /* clear error flag */
            FTFL->FSTAT |= FTFL_FSTAT_RDCOLERR_MASK;

            /* update return value*/
            Return |= Flash_RDCOLERR;
        }
        /* checking MGSTAT0 non-correctable error */
        else if (FTFL->FSTAT & FTFL_FSTAT_MGSTAT0_MASK)
        {
            Return |= Flash_MGSTAT0;
        } /* EndIf */
        /* decrement byte count */
        size_buffer --;
        (uint32_t*)DataSrcPtr++;
        FlashStartAdd +=4;
    } /* EndWhile */
    /* function return */
    return  Return;
} /* EndBody */



OPERATE_Status Fn_EreaseFlash(const uint32_t start_addr, const uint8_t erase_pages)
{
    uint8_t erase_cnt = 0;
    uint8_t flash_status;

    if( (start_addr > END_ADDR) || (start_addr <= BASE_ADDR) || ((start_addr + erase_pages * FLASH_PAGE_SIZE > END_ADDR)) )
    {
        return OP_OVERFLOW;
    }
    else
    {
        for(erase_cnt = 0; erase_cnt < erase_pages; erase_cnt++)
        {
						NVIC_DisableIRQ(USB0_IRQn); //使能USB模块IRQ中断
            flash_status = Flash_SectorErase(start_addr + (FLASH_PAGE_SIZE * erase_cnt));
						NVIC_EnableIRQ(USB0_IRQn); //使能USB模块IRQ中断
						flash_status = Flash_OK;
            if(flash_status != Flash_OK)
            {
                break;
            }
        }
        if(erase_cnt != erase_pages)
        {
            return OP_ERROR;
        }
        else
        {
            return OP_SUCCESS;
        }
    }
}

OPERATE_Status Fn_WriteFlash(const uint32_t start_addr, uint32_t *data_ptr, const uint32_t length)
{
    uint32_t status;
	NVIC_DisableIRQ(USB0_IRQn); //使能USB模块IRQ中断
  status = Flash_ByteProgram(start_addr, data_ptr, length);
  NVIC_EnableIRQ(USB0_IRQn); //使能USB模块IRQ中断
	status = Flash_OK;
    if(status == Flash_OK)
    {
        return OP_SUCCESS;
    }
    else
    {
        return OP_ERROR;
    }
}

