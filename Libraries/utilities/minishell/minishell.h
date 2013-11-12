#ifndef __MINISHELL_H__
#define __MINISHELL_H__

#include "uart.h"


//! @brief Computes the number of elements in an array.
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))
#endif


//SHELL操作连接器
typedef struct
{
    void     (*ctrl_putchar)(uint8_t);
    uint8_t  (*ctrl_getchar)(void);
}MINISHELL_InstallTypeDef;

//用户注册函数结构
typedef struct
{
    char *name;                 //!< Command name
    int  maxargs;               //!< max args
    int (*cmd)(int, char *[]);  //!< Implementation function
    char *usage;                //!< Usage message (short)
}MINISHELL_CommandTableTypeDef;


//API Functions
void MINISHELL_Init(void);
void MINISHELL_Install(MINISHELL_InstallTypeDef* pInstall);
int MINISHELL_printf(const char *format,...);
void MINISHELL_CmdHandleLoop(char *name);
void MINISHELL_Register(MINISHELL_CommandTableTypeDef* SHELL_CommandTableStruct, uint16_t NumberOfFunction);


#endif


