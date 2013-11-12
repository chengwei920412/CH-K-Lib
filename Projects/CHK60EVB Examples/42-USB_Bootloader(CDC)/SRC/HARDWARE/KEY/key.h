#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
#include "gpio.h"

//LED头文件
//IO 口
#define KEY1_PORT  PTC
#define KEY2_PORT  PTC
//引脚
#define KEY1_PIN  (17)
#define KEY2_PIN  (18)
//位宏
#define KEY1      PCin(KEY1_PIN)
#define KEY2      PCin(KEY2_PIN)
//本构件实现的接口函数
void KEY_Init(void);

//连续按键模式
typedef enum KEY_MODE  //按键扫描模式
{
	KEY_MODE_ONCES,
  KEY_MODE_CONTINUE,
}KEY_MODE;
//按键返回结果枚举
typedef enum KRY_RES
{
	KEY_RES_KEY1 = 1,
	KEY_RES_KEY2 = 2,
	KEY_RES_NO_KEY = 0,
}KEY_RES;
//本构件实现的接口函数
KEY_RES KEY_Scan(KEY_MODE mode);
void KEY_Init(void);

#endif

